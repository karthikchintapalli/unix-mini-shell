#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <signal.h>
#include "parsing.h"
extern char pseudohome[102];
extern int backproc;
extern char *jobnames[100];
extern int jobids[100];
extern int numback;
extern int backind;
extern int curproc;
extern char curname[100];
char *uname;
char hostname[32];
void sig_handler(int signo)
{
	if (signo == SIGINT)
	{
		printf("\n");
    	dispprompt(uname, hostname);
    }
    else if(signo == SIGTSTP)
    {
    	if(curproc != -1000)
    	{
    		kill(curproc, SIGTSTP);
    		printf("Process with id: [%d] stopped\n", curproc);
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
	  			jobids[backind++] = curproc;
	  			jobnames[backind - 1] = (char *)malloc(110);
	  			strcpy(jobnames[backind - 1], curname);
	  		}
	  		else
	  		{
	  			backind = 0;
	  			jobids[backind] = curproc;
	  			jobnames[backind] = (char *)malloc(110);
	  			backind++;
	  			strcpy(jobnames[backind - 1], curname);
	  		}
    	}
    }
}
int main(int argc, char const *argv[])
{
	
	uid_t uid = geteuid();
	struct passwd *pw = getpwuid(uid);
	if (pw) 
		uname = pw->pw_name;
	gethostname(hostname, 32);
	getcwd(pseudohome, 100);
	backproc = 0;
	numback = 0;
	backind = 0;
	int i;
	for(i = 0; i < 100; i++)
	jobids[i] = -1000;
	signal(SIGINT, sig_handler);
	signal(SIGTSTP, sig_handler);
	while (1)
	{
		curproc = -1000;
		dispprompt(uname, hostname);
	}
	return 0;
}