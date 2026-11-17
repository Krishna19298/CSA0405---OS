#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#define TR 10
#define CO 10
#define SE 60
#define OPS 200000
static int seat[TR][CO][SE]; static int sold[TR][CO];
static pthread_mutex_t g = PTHREAD_MUTEX_INITIALIZER, lt[TR], lc[TR][CO];
static int mode, T;
static double now(void){struct timespec t;clock_gettime(CLOCK_MONOTONIC,&t);return t.tv_sec+t.tv_nsec*1e-9;}
static void *agent(void *a){unsigned s=(unsigned)(long)a*7919u+1;
 for(int i=0;i<OPS;i++){s=s*1103515245u+12345u;int t=(s>>8)%TR,c=(s>>16)%CO;
  pthread_mutex_t *m = mode==0?&g: mode==1?&lt[t]:&lc[t][c];
  pthread_mutex_lock(m);
  for(int k=0;k<SE;k++) if(!seat[t][c][k]){seat[t][c][k]=1;sold[t][c]++;break;}   /* find and take a free seat */
  if(sold[t][c]==SE){ for(int k=0;k<SE;k++)seat[t][c][k]=0; sold[t][c]=0; }       /* coach full: new day */
  pthread_mutex_unlock(m);}
 return NULL;}
int main(int c,char**v){mode=atoi(v[1]);T=atoi(v[2]);
 for(int i=0;i<TR;i++){pthread_mutex_init(&lt[i],NULL);for(int j=0;j<CO;j++)pthread_mutex_init(&lc[i][j],NULL);}
 pthread_t th[64];double t0=now();
 for(long i=0;i<T;i++)pthread_create(&th[i],NULL,agent,(void*)i);
 for(int i=0;i<T;i++)pthread_join(th[i],NULL);double d=now()-t0;
 long ops=(long)T*OPS; printf("%-12s threads=%2d time=%.3f s  throughput=%.2f M bookings/s\n",mode==0?"global":mode==1?"per-train":"per-coach",T,d,ops/d/1e6);return 0;}
