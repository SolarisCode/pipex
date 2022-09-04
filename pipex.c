/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: melkholy <melkholy@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/08/31 18:40:17 by melkholy          #+#    #+#             */
/*   Updated: 2022/09/04 13:43:42 by melkholy         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <sys/unistd.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
#include <string.h>
#include <stdlib.h>
#include "../Libft/libft.h"

char	*ft_add_path(char *cmd, char **path)
{
	int		count;
	char	*cmd_path;

	count = -1;
	cmd_path = NULL;
	while (path[++count])
	{
		cmd_path = ft_strjoin(path[count], cmd);
		if (!access(cmd_path, F_OK | X_OK))
			break ;
		free(cmd_path);
	}
	count = -1;
	while (path[++count])
		free(path[count]);
	free(path);
	return (cmd_path);
}

char	*ft_get_path(char *cmd, char *envp[])
{
	int		count;
	char	*str;
	char	**path;

	count = -1;
	str = NULL;
	while (envp[++count])
		if (ft_strnstr(envp[count], "PATH=", 5))
			str = ft_strtrim(ft_strnstr(envp[count], "PATH=", 5), "PATH=");
	if (str)	
		path = ft_split(str, ':');
	else
		return (NULL);
	free(str);
	count = -1;
	while (path[++count])
	{
		str = path[count];
		path[count] = ft_strjoin(path[count], "/");
		free(str);
	}
	return (ft_add_path(cmd, path));
}

int	main(int argc, char *argv[], char *envp[])
{
	int		mypipe[2];
	int		pip;
	int		fnum;
	char	*cmd[2];
	char	*cmd1[2];

	pip = pipe(mypipe);
	cmd[0] = ft_get_path(argv[1], envp);
	cmd1[0] = ft_get_path(argv[2], envp);
	cmd[1] = NULL;
	cmd1[1] = NULL;
	fnum = fork();

	if (pip < 0)
		perror("We have an error in Pipe");
	if (fnum < 0)
		perror("We have an error in fork");
	if (fnum == 0)
	{
		close(mypipe[0]);
		dup2(mypipe[1], 1);
		close(mypipe[1]);
		execve(cmd[0], cmd, envp);
	}
	if (fnum > 0)
	{
		fnum = fork();
		if (fnum == 0)
		{
			close(mypipe[1]);
			dup2(mypipe[0], 0);
			close(mypipe[0]);
			execve(cmd1[0], cmd1, NULL);
		}
	}
	free(cmd[0]);
	free(cmd1[0]);
	return (0);
}
