#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pwd.h>
#include <signal.h>

void callcd(char *comm, int commind);
void callpwd(char *comm, int commind);
void callecho(char *comm, int commind);
int callbuiltin(char *comm, int commind);
void callpinfo(char *comm, int commind);
void parsecomm(char *comm);
char* promptdir();
void dispprompt(char *uname, char *hostname);
void handleterm(int sig);

