#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <signal.h>
void callcd(char *comm, int commind);
void callpwd(char *comm, int commind);
void callecho(char *comm, int commind);
int callbuiltin(char *comm, int commind, int indesc, int outdesc);
void callpinfo(char *comm, int commind);
void calllistjobs(char *comm, int commind);
void callsendsig(char *comm, int commind);
void callkillallbg(char *comm, int commind);
void callfg(char *comm, int commind);