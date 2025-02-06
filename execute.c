/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: edarnand <edarnand@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/06 18:54:04 by edarnand          #+#    #+#             */
/*   Updated: 2025/02/06 19:09:44 by edarnand         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
#include "pipex.h"
#include <unistd.h>
#include <stdio.h>

static char	*join_cmd_to_path(char *path, char *cmd)
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

static char	**get_cmd(char *cmd_str, char **paths)
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
