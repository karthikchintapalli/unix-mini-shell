#include "commands.h"
#include "parsing.h"
#include "redirection.h"
char *jobnames[100];
int jobids[100];
int numback;
int backind;
char pseudohome[102];
int backproc;
int curproc;
char curname[100];
void callcd(char *comm, int commind)
{
	char target[100];
	char *c = comm + commind;
	strcpy(target, c);
	if(comm[commind] == 0)
		strcpy(target, pseudohome);
	
	else if(comm[commind] == '~')
	{
		char homedir[100];
		strcpy(homedir, pseudohome);
		c = comm + commind + 1;
		if(comm[commind + 1] != 0)
		{
		strcat(homedir, c);
		strcpy(target, homedir);
		}
		else
			strcpy(target, pseudohome);
		
	}
	int success = chdir(target);
	if (success == -1)
		fprintf(stderr, "Path doesn't exist\n");
}
void callpwd(char *comm, int commind)
{
	char curdir[102];
	printf("%s\n", getcwd(curdir, 100));
	if(comm[commind] == ';')
		parsecomm(comm + commind + 1, 0, 1);
}
void callecho(char *comm, int commind)
{
	int i = 0;
	int escape = 0;
	char *c = comm + commind;
	for(i = 0; i < strlen(c); i++)
	{
		if(escape == 1)
		{
			if(c[i] == 'n')
				printf("\n");
			else 
				printf("%c", c[i]);
			escape = 0;
		}
		else if(c[i] == '\"' || c[i] == '\'')
			continue;
		else if(c[i] == '\\')
			escape = 1;
		else
			printf("%c", c[i]);
	}
	printf("\n");
	if(comm[commind] == ';')
		parsecomm(comm + commind + 1, 0, 1);
}
void callpinfo(char *comm, int commind)
{
	char *c = comm + commind;
	char pid[10];
	strcpy(pid, c);
	char execcomm[100];
	strcpy(execcomm, "ps -o pid= -p ");
	strcat(execcomm, pid);
	fprintf(stderr, "pid -- ");
	callbuiltin(execcomm, 0, 0, 1);
	strcpy(execcomm, "ps -o state= -p ");
	strcat(execcomm, pid);
	fprintf(stderr, "Process Status -- ");
	callbuiltin(execcomm, 0, 0, 1);
	strcpy(execcomm, "ps -o rss= -p ");
	strcat(execcomm, pid);
	fprintf(stderr, "Memory Usage -- ");
	callbuiltin(execcomm, 0, 0, 1);
	strcpy(execcomm, "ps -o comm= -p ");
	strcat(execcomm, pid);
	fprintf(stderr, "Executable Path -- ");
	callbuiltin(execcomm, 0, 0, 1);
}
void calllistjobs(char *comm, int commind)
{
	int i;
	for(i = 0; i < 100; i++)
	{
		if(jobids[i] != -1000)
			printf("[%d] %s [%d]\n", i + 1, jobnames[i], jobids[i]);
	}
}
void callsendsig(char *comm, int commind)
{
	int i = commind;
	int flag = 0;
	while(comm[i] != 0)
	{
		if(comm[i] == ' ')
		{
			flag = 1;
			comm[i] = 0;
			break;
		}
		i++;
	}
	if(!flag)
	{
		printf("sendsig: Not enough arguments\n");
		return;
	}
	int jid = atoi(comm + commind);
	int signo = atoi(comm + i + 1);
	if(jid == 0 || signo == 0 || signo > 31)
	{
		printf("sendsig: Invalid arguments\n");
		return;
	}
	if(kill(jobids[jid - 1], 0) == 0)
		kill(jobids[jid - 1], signo);
	else
		printf("sendsig: Job doesn't exist\n");
	

}
void callkillallbg(char *comm, int commind)
{
	int i;
	for(i = 0; i < 100; i++)
	{
		if(jobids[i] != -1000)
			kill(jobids[i], 9);
	}
} 
void callfg(char *comm, int commind)
{	
	int i = commind;
	while(comm[i] == ' ')
		i++;
	
	if(comm[i] == 0)
	{
		printf("fg: Not enough arguments\n");
		return;
	}
	int jid = atoi(comm + i);
	if(jid == 0)
	{
		printf("sendsig: Invalid arguments\n");
		return;
	}
	if(kill(jobids[jid - 1], 0) == 0)
	{
		kill(jobids[jid - 1], SIGCONT);
		jobids[jid - 1] = -1000;
	}
	else
		printf("sendsig: Job doesn't exist\n");
	

}
void handleterm(int sig)
{
	pid_t pid;
	int status;
	while(1)
	{	
		pid = waitpid(-1, &status, WNOHANG);
		if(pid <= 0)
			break;
		else if(backproc == 1)
		{
			int i;
			for(i = 0; i < 100; i++)
			{
				if(jobids[i] == pid)
				{
					jobids[i] = -1000;
					break;
				}
			}
			fprintf(stderr, "Process exited with pid: [%d]\n", pid);
			return;
		}
	}

}
int callbuiltin(char *comm, int commind, int indesc, int outdesc)
{
	char dupcomm[100];
	strcpy(dupcomm, comm);
	char *args[100];
	char *str1, *saveptr1, *token;
	char delim[2] = " ";
	backproc = 0;
	int j;
	for (j = 0, str1 = comm; ; j++, str1 = NULL) 
	{	
       	token = strtok_r(str1, delim, &saveptr1);

       	if (token == NULL)
       	{
       	   	args[j] = (char *)0;
          	break;
       	}
       	else if(strcmp(token, "&") == 0)
       		backproc = 1;
       	else
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
	  	if(suc == -1)
	  		fprintf(stderr, "%s: Command not found\n", args[0]);
	  	else if(backproc)
	  		printf("%d\n", suc);
	  	_exit(EXIT_SUCCESS);
	}
	else 
	{
	  	int status;
	  	if(!backproc)
	  	{
	  		curproc = pid;
	  		strcpy(curname, dupcomm);
	  		(void)waitpid(pid, &status, WUNTRACED);
	  	}
	  	
	  	else
	  	{
	  		signal(SIGCHLD, handleterm);
	  		printf("Background process started with id : [%d]\n", pid);
	  		int i;
	  		int count = 0;
	  		for(i = 0; i < 100; i++)
	  		{
	  			if(kill(jobids[i], 0) == 0)
	  			{
	  				count++;
	  				break;
	  			}
	  			else
	  				jobids[i] = -1000;
	  			
	  		}
	  		if(count > 0)
	  		{
	  			jobids[backind++] = pid;
	  			jobnames[backind - 1] = (char *)malloc(110);
	  			strcpy(jobnames[backind - 1], dupcomm);
	  		}
	  		else
	  		{
	  			backind = 0;
	  			jobids[backind] = pid;
	  			jobnames[backind] = (char *)malloc(110);
	  			backind++;
	  			strcpy(jobnames[backind - 1], dupcomm);
	  		}

	  	}
	  	
	}
	return EXIT_SUCCESS;
}

