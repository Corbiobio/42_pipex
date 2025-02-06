/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: edarnand <edarnand@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/21 11:25:49 by edarnand          #+#    #+#             */
/*   Updated: 2025/02/06 19:12:48 by edarnand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PIPEX_H
# define PIPEX_H

typedef struct s_pipe
{
	int	file;
	int	pipe_fd[2];
	int	pipe_next;
}	t_pipe;

//pipex
void	pipex(int ac, char **av, char **paths);
void	perror_free_exit(char *error, char **to_free);

//child_process
void	child_process(int *pipe_fd, int i, char **av, char **paths);

//execute
void	execute(char *cmd_str, char **paths);

#endif
