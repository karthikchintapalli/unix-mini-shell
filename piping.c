#include "piping.h"
#include "parsing.h"
int piping(char *comm, int commind)
{
	char *args[100];
    int in, fd [2];
	in = 0;
	char *dup = comm;
	int pipcount = 0, i;
	int startpoints[100];
	startpoints[0] = commind;
	for(i = commind; comm[i] != '\0'; i++)
    {
    	if(comm[i] == '|')
    	{	
			comm[i] = 0;
			startpoints[++pipcount] = i + 1;
		}
    }
    for(i = 0; i < pipcount; i++)
    {
    	pipe (fd);
		parsecomm (comm + startpoints[i], in, fd [1]) ;
		close (fd [1]);
		in = fd [0];
    }
	parsecomm (comm + startpoints[i], in, 1) ;   
	return 0;
}