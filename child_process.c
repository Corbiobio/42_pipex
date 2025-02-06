/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   child_process.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: edarnand <edarnand@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/06 18:50:17 by edarnand          #+#    #+#             */
/*   Updated: 2025/02/06 19:13:02 by edarnand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"
#include "libft.h"
#include <fcntl.h>
#include <unistd.h>

static void	close_all_fd(int fd1, int fd2, int fd3, int fd4)
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

static void	first_child(int *pipe_fd, char **av, char **paths)
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

static void	last_child(int *pipe_fd, int i, char **av, char **paths)
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

static void	mid_child(int *pipe_fd, int i, char **av, char **paths)
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
