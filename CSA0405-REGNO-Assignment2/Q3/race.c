#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <sched.h>
#define LOOPS 100000
long unsafe = 0, locked = 0; pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
void *w(void *a){(void)a;
 for(int i=0;i<LOOPS;i++){
  long tmp = unsafe;            /* read   */
  if (i % 50 == 0) sched_yield(); /* lets another thread run in the gap (as the OS may at any time) */
  unsafe = tmp + 1;             /* write  */
  pthread_mutex_lock(&m); locked++; pthread_mutex_unlock(&m);}
 return NULL;}
int main(void){pthread_t t[4];for(int i=0;i<4;i++)pthread_create(&t[i],NULL,w,NULL);for(int i=0;i<4;i++)pthread_join(t[i],NULL);
 printf("expected    : %d\nwithout lock: %ld\nwith mutex  : %ld\n",4*LOOPS,unsafe,locked);return 0;}
