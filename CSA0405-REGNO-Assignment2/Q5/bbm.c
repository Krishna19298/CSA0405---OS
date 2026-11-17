#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <sched.h>
#define SIZE 5
#define PER 2000
int buffer[SIZE], in = 0, out = 0, K;
sem_t empty, full; pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
int use_mutex; static int seen[100000]; long sum = 0; pthread_mutex_t sl = PTHREAD_MUTEX_INITIALIZER;
void *prod(void *a){int id=(int)(long)a;for(int i=1;i<=PER;i++){int item=id*PER+i;sem_wait(&empty);
 if(use_mutex)pthread_mutex_lock(&lock);
 int s=in; if(!use_mutex&&i%7==0)sched_yield(); buffer[s]=item; in=(s+1)%SIZE;
 if(use_mutex)pthread_mutex_unlock(&lock); sem_post(&full);} return NULL;}
void *cons(void *a){(void)a;for(int i=0;i<PER;i++){sem_wait(&full);
 if(use_mutex)pthread_mutex_lock(&lock);
 int s=out; if(!use_mutex&&i%7==0)sched_yield(); int item=buffer[s]; out=(s+1)%SIZE;
 if(use_mutex)pthread_mutex_unlock(&lock); sem_post(&empty);
 pthread_mutex_lock(&sl); seen[item]++; sum+=item; pthread_mutex_unlock(&sl);} return NULL;}
int main(int c,char**v){K=atoi(v[1]);use_mutex=atoi(v[2]);sem_init(&empty,0,SIZE);sem_init(&full,0,0);
 pthread_t p[8],q[8];for(int i=0;i<K;i++){pthread_create(&p[i],NULL,prod,(void*)(long)i);pthread_create(&q[i],NULL,cons,NULL);}
 for(int i=0;i<K;i++){pthread_join(p[i],NULL);pthread_join(q[i],NULL);}
 long exp=0;int lost=0,dup=0;for(int id=0;id<K;id++)for(int i=1;i<=PER;i++){int it=id*PER+i;exp+=it;if(seen[it]==0)lost++;if(seen[it]>1)dup++;}
 printf("K=%d mutex=%d consumed=%d expected_sum=%ld sum=%ld missing=%d duplicated=%d\n",K,use_mutex,K*PER,exp,sum,lost,dup);return 0;}
