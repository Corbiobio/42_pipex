/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: edarnand <edarnand@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/21 12:08:06 by edarnand          #+#    #+#             */
/*   Updated: 2025/02/06 18:05:11 by edarnand         ###   ########.fr       */
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

	if (ft_strchr_index(cmd, '/') != -1)
		return (cmd);
	tmp = ft_strjoin(path, "/");
	if (tmp == NULL)
		return (NULL);
	path = ft_strjoin(tmp, cmd);
	free(tmp);
	return (path);
}

char	**get_cmd(char *cmd_str, char **paths)
{
	char	**cmd;

	cmd = ft_split(cmd_str, ' ');
	if (cmd == NULL)
	{
		free_double_pt(paths);
		exit(EXIT_FAILURE);
	}
	return (cmd);
}

void	execute(char *cmd_str, char **paths)
{
	char	**cmd;
	char	*path;
	int		i;

	cmd = get_cmd(cmd_str, paths);
	i = 0;
	while (paths[i] != NULL)
	{
		path = join_cmd_to_path(paths[i], cmd[0]);
		if (path == NULL)
			continue ;
		if (access(path, X_OK) != -1)
		{
			if (execve(path, cmd, NULL) == -1)
				free(path);
			break ;
		}
		i++;
		free(path);
	}
	perror(cmd[0]);
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

void	perror_free_exit(char *error, char **to_free)
{
	perror(error);
	free_double_pt(to_free);
	exit(EXIT_FAILURE);
}

void	first_child(int *pipe_fd, char **av, char **paths)
{
	int	infile;

	infile = open(av[0], O_RDONLY);
	if (infile == -1)
	{
		close_all_fd(infile, pipe_fd[0], pipe_fd[1], -1);
		perror_free_exit(av[0], paths);
	}
	if (dup2(infile, STDIN_FILENO) == -1)
	{
		close_all_fd(infile, pipe_fd[0], pipe_fd[1], -1);
		perror_free_exit("dup2 error", paths);
	}
	if (dup2(pipe_fd[1], STDOUT_FILENO) == -1)
	{
		close_all_fd(infile, pipe_fd[0], pipe_fd[1], -1);
		perror_free_exit("dup2 error", paths);
	}
	close_all_fd(infile, pipe_fd[0], pipe_fd[1], -1);
	execute(av[1], paths);
}

void	last_child(int *pipe_fd, int i, char **av, char **paths)
{
	int	outfile;

	outfile = open(av[i + 1], O_CREAT | O_WRONLY | O_TRUNC, 0777);
	if (outfile == -1)
	{
		close_all_fd(outfile, pipe_fd[0], pipe_fd[1], pipe_fd[2]);
		perror_free_exit(av[0], paths);
	}
	if (dup2(pipe_fd[2], STDIN_FILENO) == -1)
	{
		close_all_fd(outfile, pipe_fd[0], pipe_fd[1], pipe_fd[2]);
		perror_free_exit("dup2 error", paths);
	}
	if (dup2(outfile, STDOUT_FILENO) == -1)
	{
		close_all_fd(outfile, pipe_fd[0], pipe_fd[1], pipe_fd[2]);
		perror_free_exit("dup2 error", paths);
	}
	close_all_fd(outfile, pipe_fd[0], pipe_fd[1], pipe_fd[2]);
	execute(av[i], paths);
}

void	mid_child(int *pipe_fd, int i, char **av, char **paths)
{
	if (dup2(pipe_fd[2], STDIN_FILENO) == -1)
	{
		close_all_fd(pipe_fd[0], pipe_fd[1], pipe_fd[2], -1);
		perror_free_exit("dup2 error", paths);
	}
	if (dup2(pipe_fd[1], STDOUT_FILENO) == -1)
	{
		close_all_fd(pipe_fd[0], pipe_fd[1], pipe_fd[2], -1);
		perror_free_exit("dup2 error", paths);
	}
	close_all_fd(pipe_fd[0], pipe_fd[1], pipe_fd[2], -1);
	execute(av[i], paths);
}

void	child_process(int *pipe_fd, int i, char **av, char **paths)
{
	if (i == 1)
		first_child(pipe_fd, av, paths);
	else if (av[i + 2] == NULL)
		last_child(pipe_fd, i, av, paths);
	else
		mid_child(pipe_fd, i, av, paths);
}

void	pipex(int ac, char **av, char **paths)
{
	int	i;
	int	pid;
	int	pipe_fd[3];

	i = 1;
	while (i < ac - 1)
	{
		if (pipe(pipe_fd) == -1)
			return ;
		pid = fork();
		if (pid < 0)
			return ;
		if (pid == 0)
			child_process(pipe_fd, i, av, paths);
		else
		{
			if (i != 1)
				close(pipe_fd[2]);
			pipe_fd[2] = pipe_fd[0];
			if (i == ac - 2)
				close(pipe_fd[0]);
			close(pipe_fd[1]);
		}
		i++;
	}
}

int	main(int ac, char **av, char **env)
{
	char	**paths;
	int		status;

	status = 0;
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
		while (wait(&status) > 0)
			;
		free_double_pt(paths);
	}
	return (WEXITSTATUS(status));
}
