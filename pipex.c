/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: edarnand <edarnand@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/21 12:08:06 by edarnand          #+#    #+#             */
/*   Updated: 2025/02/06 18:58:04 by edarnand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
#include "pipex.h"
#include <sys/wait.h>
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

static char	*find_paths_in_env(char **env)
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
