#include "parsing.h"
#include "commands.h"
#include "redirection.h"
#include "piping.h"
char pseudohome[102];
void parsecomm(char *comm, int indesc, int outdesc)
{
	char word[100];
	int commind = 0;
	int wordind = 0;
	while((comm[commind] != ';') && comm[commind] != 0)
		commind++;
	
	if(comm[commind] == ';')
	{
		comm[commind] = 0;
		parsecomm(comm, 0, 1);
		parsecomm(comm + commind + 1, 0, 1);
		return;
	}
	commind = 0;
	while((comm[commind] == ' ') || (comm[commind] == '\t'))
		commind++;
	int dupind = commind;
	int redircount = 0;
	while(comm[dupind] != 0)
	{
		if(comm[dupind] == '<' || comm[dupind] == '>')
			redircount++;
		else if(comm[dupind] == '|')
		{
			piping(comm, commind);
			return;
		}
 
		dupind++;
	}
	if(redircount)
	{
		redirec(comm, commind, indesc, outdesc);
		return;
	}
	while ((comm[commind] != ' ') && (comm[commind] != '\t') && (comm[commind] != ';') && (comm[commind] != '\0'))
		word[wordind++] = comm[commind++];
	word[wordind] = '\0';
	while((comm[commind] == ' ') || (comm[commind] == '\t'))
		commind++;
	if(strcmp(word, "quit") == 0)
		_exit(EXIT_SUCCESS);
  	else if (strcmp(word, "cd") == 0)
  		callcd(comm, commind);
  	/*else if (strcmp(word, "pwd") == 0)
  		callpwd(comm, commind);
  	else if (strcmp(word, "echo") == 0)
  		callecho(comm, commind);*/
	else if(strcmp(word, "fg") == 0)
		callfg(comm, commind);
	else if(strcmp(word, "killallbg") == 0)
		callkillallbg(comm, commind);
  	else if(strcmp(word, "pinfo") == 0)
  		callpinfo(comm, commind);
  	else if(strcmp(word, "listjobs") == 0)
  		calllistjobs(comm, commind);
  	else if(strcmp(word, "sendsig") == 0)
  		callsendsig(comm, commind);
	else
  		callbuiltin(comm, commind, indesc, outdesc);
	return;		
    
}
char* promptdir()
{
	char curdir1[100];
	char *retstr = malloc(100);
	
	getcwd(curdir1, 100);
	char *curdir = curdir1;
	if(strstr(curdir, pseudohome) != NULL)
	{
		int i = 0;
		while((curdir[0] != '\0') && (curdir[0] == pseudohome[i]))
		{
			i++;
			curdir++;
		}
		strcpy(retstr, "~");
		if(curdir[0] != '\0')
			strcat(retstr, curdir);
		
	}
	else
		strcpy(retstr, curdir);
	return retstr;
}

void dispprompt(char *uname, char *hostname)
{
	char comm[256];
	printf("<%s@%s: %s> ", uname, hostname, promptdir());
	int ind = 0;
	while(1)
	{
		char c = getchar();
		if(c == EOF)
		{
			printf("\n");
			exit(0);
		}
		else if(c == '\n')
		{
			if(ind == 0)
				return;
			else
			{
				comm[ind] = 0;
				parsecomm(comm, 0, 1);
				return;
			}
		}
		else
		{
			comm[ind++] = c;
		}
	}
}