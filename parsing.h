#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
void parsecomm(char *comm, int indesc, int outdesc);
char* promptdir();
void dispprompt(char *uname, char *hostname);
void handleterm(int sig);