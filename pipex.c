/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: edarnand <edarnand@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/21 12:08:06 by edarnand          #+#    #+#             */
/*   Updated: 2025/02/05 11:45:23 by edarnand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
#include "pipex.h"
#include <fcntl.h> //open
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>

char	*find_paths_in_env(char **env)
{
	int		i;
	char	*paths;

	i = 0;
	paths = NULL;
	while (env[i] != NULL)
	{
		
		if (ft_strnstr(env[i], "PATH=", 5) != NULL)
		{
			paths = env[i] + 5;
			break ;
		}
		i++;
	}
	return (paths);
}

char	*join_cmd_to_path(char *path, char *cmd)
{
	char	*tmp;

	tmp = ft_strjoin(path, "/");
	if (tmp == NULL)
		return (NULL);
	path = ft_strjoin(tmp, cmd);
	free(tmp);
	return (path);
}

void	execute(char *cmd_str, char **paths)
{
	char	**cmd;
	char	*path;
	int		i;

	cmd = ft_split(cmd_str, ' ');
	if (cmd == NULL)
		perror("Failed to split cmd");
	i = 0;
	while (paths[i] != NULL)
	{
		path = join_cmd_to_path(paths[i], cmd[0]);
		if (path == NULL)
			continue ;
		if (access(path, X_OK) != -1)
		{
			if (execve(path, cmd, NULL) == -1)
				perror(path);
			free(path);
			break ;
		}
		i++;
		free(path);
	}
	free_double_pt(cmd);
}

void	pipex(char **av, char **paths)
{
	int	i;
	int	pid;
	int	pipe_fd[2];
	int	next_read_fd  = -1;
	(void)paths;
	(void)av;

	i = 0;
	while (i < 3)
	{
		pipe(pipe_fd);
		pid = fork();//protect
		if (pid == 0)
		{
			if (i == 0)
			{
				int	file1 = open("input", O_RDONLY); //protect
				dup2(file1, STDIN_FILENO);
				dup2(pipe_fd[1], STDOUT_FILENO);
				close(file1);
				close(pipe_fd[0]);
				close(pipe_fd[1]);
				execute("cat", paths);
				//protect if exev fail
			}
			else if (i == 1)
			{
				dup2(next_read_fd, STDIN_FILENO);
				dup2(pipe_fd[1], STDOUT_FILENO);
				close(next_read_fd);
				close(pipe_fd[0]);
				close(pipe_fd[1]);
				execute("wc -l", paths);
			}
			else if (i == 2)
			{
				int	file2 = open("output", O_WRONLY | O_CREAT | O_APPEND); //protect | replace append by trunc
				dup2(next_read_fd, STDIN_FILENO);
				dup2(file2, STDOUT_FILENO);
				close(file2);
				close(next_read_fd);
				close(pipe_fd[0]);
				close(pipe_fd[1]);
				execute("wc -m", paths);
				//protect if exev fail
			}
		}
		else if (pid > 0)
		{
			if (i != 0)
				close(next_read_fd);
			next_read_fd = pipe_fd[0];
			if (i == 2)//if last close all
				close(pipe_fd[0]);
			close(pipe_fd[1]);
		}
		i++;
	}
	int t = wait(NULL);
	while (t > 0)
		t = wait(NULL);
}

int	main(int ac, char **av, char **env)
{
	char	**paths;

	if (ac <= 4)
		ft_putstr_fd("Not enough param", STDERR_FILENO);
	else
	{
		av++;
		ac--;
		paths = ft_split(find_paths_in_env(env), ':');
		if (paths == NULL)
			exit(EXIT_FAILURE);
		pipex(av, paths);
		free_double_pt(paths);
	}
	return (EXIT_SUCCESS);
}
