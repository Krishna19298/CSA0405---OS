#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdatomic.h>
#include <time.h>
#define TOTAL 1600000
static atomic_long ac; static long mc; static long sc; static atomic_flag sp = ATOMIC_FLAG_INIT; static pthread_mutex_t m=PTHREAD_MUTEX_INITIALIZER;
static int mode, T;
static double now(void){struct timespec t;clock_gettime(CLOCK_MONOTONIC,&t);return t.tv_sec+t.tv_nsec*1e-9;}
static double cpu(void){struct timespec t;clock_gettime(CLOCK_PROCESS_CPUTIME_ID,&t);return t.tv_sec+t.tv_nsec*1e-9;}
static void *w(void*a){(void)a;for(int i=0;i<TOTAL/T;i++){
 if(mode==0)atomic_fetch_add(&ac,1);
 else if(mode==1){pthread_mutex_lock(&m);mc++;pthread_mutex_unlock(&m);}
 else{while(atomic_flag_test_and_set_explicit(&sp,memory_order_acquire));sc++;atomic_flag_clear_explicit(&sp,memory_order_release);}}
 return NULL;}
int main(int c,char**v){mode=atoi(v[1]);T=atoi(v[2]);pthread_t th[32];double t=now(),c0=cpu();
 for(int i=0;i<T;i++)pthread_create(&th[i],NULL,w,NULL);for(int i=0;i<T;i++)pthread_join(th[i],NULL);
 double d=now()-t,cp=cpu()-c0;long r=mode==0?ac:mode==1?mc:sc;
 printf("%-7s T=%2d result=%ld wall=%.4f s  ns/update=%.1f  cpu=%.4f s\n",mode==0?"atomic":mode==1?"mutex":"spin",T,r,d,d/TOTAL*1e9,cp);return 0;}
