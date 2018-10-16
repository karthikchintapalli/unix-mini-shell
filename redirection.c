#include "redirection.h"
int redirec(char *comm, int commind, int indesc, int outdesc)
{
	char *args[100];
	char outfile[100];
	char inpfile[100];
	char *str1, *saveptr1, *token;
	char delim[2] = " ";
	int outred = 0;
	int inpred = 0;
	int outexists = 0;
	int inexists = 0;
	int j;
	for (j = 0, str1 = comm; ; j++, str1 = NULL) 
	{	
       	token = strtok_r(str1, delim, &saveptr1);
       	if(outred)
       	{
       		strcpy(outfile, token);
       		outred = 0;
       		j--;
       		continue;
       	}
       	if(inpred)
       	{
       		strcpy(inpfile, token);
       		inpred = 0;
       		j--;
       		continue;
       	}
       	if (token == NULL)
       	{
       	   	args[j] = (char *)0;
          	break;
       	}
       	if(strcmp(token, ">") == 0)
       	{
       		outred = 1;
       		outexists = 1;
       		j--;
       		continue;
       	}
       	if(strcmp(token, "<") == 0)
       	{
       		inpred = 1;
       		inexists = 1;
       		j--;
       		continue;
       	}
   		args[j] = token;
   	
    }
    pid_t pid = fork();

	if (pid == -1) 
	{
	  	fprintf(stderr, "Command failed");
	  	exit(EXIT_FAILURE);
	}
	else if (pid == 0) 
	{
		int inpfd, outfd;
		if(outexists)
		{
			close(1);
			outfd = open(outfile, O_WRONLY | O_CREAT | O_TRUNC, 0777);
		}
		if(inexists)
		{
			close(0);
			inpfd = open(inpfile, O_RDONLY);
			if(inpfd == -1)
			{
				fprintf(stderr, "%s: File not found\n", inpfile);
				return 0;
			}
		}
		if (indesc != 0)
        {
			dup2 (indesc, 0);
			close (indesc);
        }

      	if (outdesc != 1)
        {
			dup2 (outdesc, 1);
			close (outdesc);
        }
		int suc = execvp(args[0], args);
		if(inpfd != -1)
			close(inpfd);
		if (outfd != -1)
			close(outfd);
	  	if(suc == -1)
	  		fprintf(stderr, "%s: Command not found\n", args[0]);
	  	_exit(EXIT_SUCCESS);
	}
	else 
	{
	  	int status;
		(void)waitpid(pid, &status, 0);	  	
	}
	return EXIT_SUCCESS;
    
}