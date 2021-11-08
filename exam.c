#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

int main(int arc, char **arv, char **env)
{
	if (arc > 1)
	{
		int		i = 1;
		int		in = 0;
		int		p_in = 0;
		int		out = 1;
		int		fd[2];
		pid_t	pid[500];
		int		pp = 0;

		while (i < arc)
		{
			char	*args[500];
			int		cc = 0;
			while (i < arc && strcmp(arv[i], "|") && strcmp(arv[i], ";"))
			{
				args[cc] = arv[i];
				cc++;
				i++;
				args[cc] = 0;
			}
			if (cc == 0)
			{
				i++;
				continue ;
			}
			if (i < arc && !strcmp(arv[i], "|"))
			{
				if (pipe(fd) < 0)
				{
					write(2, "error : fatal\n", 14);
					exit(0);
				}
				p_in = fd[0];
				out = fd[1];
				i++;
			}
			if (!strcmp(args[0], "cd"))
			{
				if (cc != 2)
					write(2, "error : cd : args\n", 18);
				else if (chdir(args[1]) < 0)
					write(2, "error : cd : can't change directory to ", 39), write(2, args[1], strlen(args[1])), write(2, "\n", 1);
			}
			else
			{
				pid[pp] = fork();
				if (pid[pp] < 0)
				{
					write(2, "error : fatal\n", 14);
					exit(0);
				}
				if (pid[pp] == 0)
				{
					dup2(in, 0);
					dup2(out, 1);
					if (p_in != 0)
						close(p_in);
					if (in != 0)
						close(in);
					if (out != 1)
						close(out);
					execve(args[0], args, env);
					write(2, "error : couldn't execute ", 25), write(2, args[0], strlen(args[0])), write(2, "\n", 1);
					exit(0);
				}
				else
				{
					if (out != 1)
						close(out);
					if (in != 0)
						close(in);
					in = p_in;
					pp++;
				}
				if (i >= arc || (i < arc && !strcmp(arv[i], ";")))
				{
					while(pp)
					{
						waitpid(pid[pp], 0, 0);
						pp--;
					}
					in = 0;
					p_in = 0;
					out = 1;
					i++;
				}
			}
		}
	}
}
