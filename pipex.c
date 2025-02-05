/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: edarnand <edarnand@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/21 12:08:06 by edarnand          #+#    #+#             */
/*   Updated: 2025/02/05 19:12:44 by edarnand         ###   ########.fr       */
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
	{
		perror("Failed to split");//dprintf ????
		free_double_pt(paths);
		exit(EXIT_FAILURE);
	}
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
	//perror("Command not found" + cmd[0]);//dprintf ????
	free_double_pt(cmd);
	free_double_pt(paths);
	exit(EXIT_FAILURE);
}

void	close_all_fd(int fd1, int fd2, int fd3, int fd4)
{
	if (fd1 != -1)
		close(fd1);
	if (fd2 != -1)
		close(fd2);
	if (fd3 != -1)
		close(fd3);
	if (fd4 != -1)
		close(fd4);
}

void	pipex(int ac, char **av, char **paths)
{
	int	i;
	int	pid;
	int	pipe_fd[2];
	int	next_read_fd;

	i = 1;
	while (i < ac - 1)
	{
		pipe(pipe_fd);//protect
		pid = fork();//protect
		if (pid < 0)
			return ;//pas sure
		if (pid == 0)
		{
			if (i == 1)
			{
				int	file1 = open(av[0], O_RDONLY); //protect
				dup2(file1, STDIN_FILENO);//protect
				dup2(pipe_fd[1], STDOUT_FILENO);//protect
				close_all_fd(file1, pipe_fd[0], pipe_fd[1], -1);
				execute(av[1], paths);
				//protect if exev fail
			}
			else if (av[i + 2] == NULL)
			{
				int	file2 = open(av[i + 1], O_WRONLY | O_CREAT | O_APPEND); //protect | replace append by trunc
				dup2(next_read_fd, STDIN_FILENO);//protect
				dup2(file2, STDOUT_FILENO);//protect
				close_all_fd(file2, pipe_fd[0], pipe_fd[1], next_read_fd);
				execute(av[i], paths);
				//protect if exev fail
			}
			else
			{
				dup2(next_read_fd, STDIN_FILENO);//protect
				dup2(pipe_fd[1], STDOUT_FILENO);//protect
				close_all_fd(pipe_fd[0], pipe_fd[1], next_read_fd, -1);
				execute(av[i], paths);
				//protect if exev fail
			}
			free_double_pt(paths);
			exit(EXIT_FAILURE);
		}
		else
		{
			if (i != 1)
				close(next_read_fd);
			next_read_fd = pipe_fd[0];
			if (i == ac - 2)//if last close all
				close(pipe_fd[0]);
			close(pipe_fd[1]);
		}
		i++;
	}
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
		pipex(ac, av, paths);
		while (wait(NULL) > 0);
		free_double_pt(paths);
	}
	return (EXIT_SUCCESS);
}
