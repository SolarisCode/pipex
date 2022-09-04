/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: melkholy <melkholy@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/08/31 18:40:17 by melkholy          #+#    #+#             */
/*   Updated: 2022/09/04 18:36:27 by melkholy         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <fcntl.h>
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
	if (access(cmd_path, F_OK | X_OK))
		perror("No such a command or you don't have permission");
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

char	**ft_check_path(char *cmd, char *envp[])
{
	char	**cmd_path;
	char	*tmp;

	if (ft_strchr(cmd, ' '))
	{
		cmd_path = ft_split(cmd, ' ');
		tmp = ft_get_path(cmd_path[0], envp);
		free(cmd_path[0]);
		cmd_path[0] = tmp;
	}
	else
	{
		tmp = ft_get_path(cmd, envp);
		cmd_path = ft_split(tmp, '\0');
		free(tmp);
	}
	return (cmd_path);
}

int	main(int argc, char *argv[], char *envp[])
{
	int		mypipe[2];
	int		pip;
	int		file;
	int		file1;
	int		fnum;
	int		count;
	char	**cmd;
	char	**cmd1;

	pip = pipe(mypipe);
	if (access(argv[1], F_OK | R_OK))
		perror("No such a command or you don't have permission");
	else
		file = open(argv[1], O_RDONLY);
	if (access(argv[4], F_OK | R_OK))
		perror("No such a command or you don't have permission");
	else
		file1 = open(argv[4], O_RDWR);
	cmd = ft_check_path(argv[2], envp);
	cmd1 = ft_check_path(argv[3], envp);
	fnum = fork();

	if (pip < 0)
		perror("We have an error in Pipe");
	if (fnum < 0)
		perror("We have an error in fork");
	if (fnum == 0)
	{
		close(mypipe[0]);
		dup2(file, 0);
		close(file);
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
			dup2(file1, 1);
			close(file1);
			execve(cmd1[0], cmd1, NULL);
		}
	}
	wait(NULL);
	count = -1;
	while (cmd[++count])
		free(cmd[count]);
	count = -1;
	while (cmd1[++count])
		free(cmd1[count]);
	free(cmd);
	free(cmd1);
	return (0);
}
