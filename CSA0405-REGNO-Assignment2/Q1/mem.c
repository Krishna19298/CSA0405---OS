#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/wait.h>
static void *sl(void*a){(void)a;sleep(2);return NULL;}
int main(int c,char**v){int n=200; int mode=v[1][0]=='p';
 if(mode){for(int i=0;i<n;i++) if(fork()==0){sleep(2);_exit(0);}}
 else {pthread_t t;for(int i=0;i<n;i++)pthread_create(&t,NULL,sl,NULL);}
 usleep(500000); char cmd[100]; sprintf(cmd,"grep -E 'VmRSS|VmSize|Threads' /proc/%d/status",getpid()); system(cmd);
 if(mode)while(wait(NULL)>0);else sleep(2); return 0;}
