/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: edarnand <edarnand@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/21 12:08:06 by edarnand          #+#    #+#             */
/*   Updated: 2025/02/03 18:46:51 by edarnand         ###   ########.fr       */
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

void	first_pipe(int *pipe_fd, char *file_name)
{
	int	file;

	file = open(file_name, O_RDONLY);//close
	dup2(file, STDIN_FILENO);//protect
	dup2(pipe_fd[1], STDOUT_FILENO);//protect
	close(file);
}

void	last_pipe(int next_fd, char *file_name)
{
	int	file;

	file = open(file_name, O_CREAT | O_WRONLY | O_APPEND, 777);//close | replace append by trunc
	dup2(next_fd, STDIN_FILENO);//protect
	dup2(file, STDOUT_FILENO);//protect
	close(file);
}

void	mid_pipe(int *pipe_fd, int next_fd)
{
	dup2(next_fd, STDIN_FILENO);//protect
	dup2(pipe_fd[1], STDOUT_FILENO);//protect
}

void	pipe_and_execute(int *pipe_fd, int index, char **av, char **paths, int next_fd)
{
	if (index == 0)
	{
		first_pipe(pipe_fd, av[0]);//verif
		av++;
	}
	else if (av[index + 2] == NULL)
		last_pipe(next_fd, av[index + 1]);//verif
	else
		mid_pipe(pipe_fd, next_fd);//verif
	close(pipe_fd[0]);
	close(pipe_fd[1]);
	execute(av[index], paths);
}

void	pipex(char **paths, char **av, int ac)
{
	int	i;
	int	pipe_fd[2];
	int	pid;
	int	next_fd;

	i = 0;
	next_fd = -1;
	while (i < ac - 1)
	{
		pipe(pipe_fd);
		pid = fork();
		if (pid < 0)
			perror("cant fork");//mh
		else if (pid == 0)
			pipe_and_execute(pipe_fd, i, av, paths, next_fd);
		else
		{
			close(pipe_fd[1]);
			next_fd = pipe_fd[0];
		}
		i++;
	}
	int	t = wait(NULL);
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
		pipex(paths, av, ac);
		free_double_pt(paths);
	}
	return (EXIT_SUCCESS);
}







//void	get_pipe(int *pipe_fd, int next_fd, int index, char **av)
//{
//	int	file;

//	if (index == 0)
//	{
//		file = open(av[0], O_RDONLY);//close
//		dup2(file, STDIN_FILENO);//protect
//		dup2(pipe_fd[1], STDOUT_FILENO);//protect
//		//close(pipe_fd[0]);
//	}
//	else if (av[index + 2] == NULL)
//	{
//		file = open(av[index + 1], O_CREAT | O_WRONLY | O_APPEND);//close | replace append by trunc
//		dup2(next_fd, pipe_fd[0]);//protect
//		dup2(file, STDOUT_FILENO);//protect
//		//close(next_fd);
//		//close(pipe_fd[1]);
//	}
//	else
//	{
//		dup2(next_fd, pipe_fd[0]);//protect
//		dup2(pipe_fd[1], STDOUT_FILENO);//protect
//		close(next_fd);
//	}
//}
// void	child_process(char **av, int pipe_fd[2], char **paths, int is_first)
//{
//	int	pid;

//	pid = fork();
//	if (pid == -1)
//		perror("Failed to fork");
//	if (pid > 0)
//	{
//	}
//	else if (pid == 0)
//	{
//		if (is_first == 1)
//		{
//			redirect_pipe_extremity(pipe_fd, av[0], is_first);
//			av++;
//		}
//		else if (av + 2 == NULL)
//			redirect_pipe_extremity(pipe_fd, av[1], 0);
//		execute(av[0], paths);
//		// if (av + 2 != NULL)
//		//	child_process(av + 1, pipe_fd, paths, 0);
//	}
//	close(pipe_fd[0]);
//	close(pipe_fd[1]);
//}
//void	dup2_first_or_last_pipe(t_pipe p, int read_fd)
//{
//	if (read_fd <= -1)
//	{
//		if (dup2(p.file, STDIN_FILENO) == -1)
//			return (perror("Cant dup2"));
//		if (dup2(p.pipe_fd[1], STDOUT_FILENO) == -1)
//			return (perror("Cant dup2"));
//	}
//	else
//	{
//		if (dup2(p.pipe_fd[0], read_fd) == -1)
//			return (perror("Cant dup2"));
//		if (dup2(p.file, STDOUT_FILENO) == -1)
//			return (perror("Cant dup2"));
//	}
//}

//t_pipe	init_p(char *file_name, int first_last)
//{
//	t_pipe	p;

//	if (file_name != NULL && first_last <= 0)
//		p.file = open(file_name, O_RDONLY, 777);
//	else if (file_name != NULL && first_last >= 1)
//		p.file = open(file_name, O_CREAT | O_WRONLY | O_APPEND, 777);
//	if (p.file == -1)
//		perror(file_name);
//	if (file_name == NULL)
//		p.file = -1;
//	if (pipe(p.pipe_fd) == -1)
//	{
//		perror("Can't pipe");
//		p.pipe_fd[0] = -1;
//		p.pipe_fd[1] = -1;
//	}
//}

//int	child_process(int index, char **av, char **paths, int new_fd)
//{
//	int		pipe_fd[2];
//	int		file;
//	t_pipe	p;

//	if (index == 0)
//	{
//		p = init_p(av[0], 0);
//		if (p.file != -1 && p.pipe_fd != -1)
//			dup2_first_or_last_pipe(p, -1);
//		execute(av[1], paths);
//	}
//	else if (av[index + 2] == NULL)
//	{
//		p = init_p(av[index], 1);
//		if (p.file != -1 && p.pipe_fd != -1)
//			dup2_first_or_last_pipe(p, new_fd);
//		execute(av[index + 1], paths);
//	}
//	return (-1);
//}
// int input = open(av[0], O_RDONLY);
// int output = open(av[3], O_CREAT | O_WRONLY | O_APPEND, 0664);
// if (pid < 0)
//	exit(1);
// if (pid == 0)
//{
//	redirect_first_pipe("input", pipe_fd);
//	dup2(input, STDIN_FILENO);
//	dup2(pipe_fd[1], STDOUT_FILENO);
//	close(pipe_fd[1]);
//	close(pipe_fd[0]);
//	execute(av[1], paths);
//}
// else
//{
//	waitpid(pid, NULL, 0);
//	dup2(pipe_fd[0], STDIN_FILENO);
//	dup2(output, STDOUT_FILENO);
//	close(pipe_fd[1]);
//	close(pipe_fd[0]);
//	fork_fd = fork();
//	if (fork_fd < 0)
//		exit(1);
//	if (fork_fd == 0)
//		execute(av[2], paths);
//	else
//		waitpid(fork_fd, NULL, 0);
//}
// close(input);
// close(output);