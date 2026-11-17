#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/wait.h>
#include <time.h>
static double now(void){struct timespec t;clock_gettime(CLOCK_MONOTONIC,&t);return t.tv_sec+t.tv_nsec*1e-9;}
#define N 2000
static void *noop(void*a){(void)a;return NULL;}
static int a[2],b[2]; static int rounds=100000;
static void *pong(void*x){(void)x;char c;for(int i=0;i<rounds;i++){read(a[0],&c,1);write(b[1],&c,1);}return NULL;}
int main(void){
  double t=now();
  for(int i=0;i<N;i++){pid_t p=fork(); if(p==0)_exit(0); waitpid(p,NULL,0);}
  double f=(now()-t)/N*1e6;
  t=now();
  for(int i=0;i<N;i++){pthread_t th;pthread_create(&th,NULL,noop,NULL);pthread_join(th,NULL);}
  double th=(now()-t)/N*1e6;
  printf("create+finish+wait of %d tasks: process %.1f us each, thread %.1f us each\n",N,f,th);
  /* context switch by pipe ping-pong between two processes */
  pipe(a);pipe(b);
  pid_t p=fork();
  if(p==0){char c;for(int i=0;i<rounds;i++){read(a[0],&c,1);write(b[1],&c,1);}_exit(0);}
  char c='x'; t=now();
  for(int i=0;i<rounds;i++){write(a[1],&c,1);read(b[0],&c,1);}
  double pp=(now()-t)/(rounds*2)*1e6; waitpid(p,NULL,0);
  printf("process switch (pipe ping-pong): %.2f us per switch\n",pp);
  pthread_t tt; t=now(); pthread_create(&tt,NULL,pong,NULL);
  for(int i=0;i<rounds;i++){write(a[1],&c,1);read(b[0],&c,1);}
  double tp=(now()-t)/(rounds*2)*1e6; pthread_join(tt,NULL);
  printf("thread switch  (pipe ping-pong): %.2f us per switch\n",tp);
  return 0;}
