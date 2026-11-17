#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <time.h>
#define CH 65536
#define SLOTS 8
static double now(void){struct timespec t;clock_gettime(CLOCK_MONOTONIC,&t);return t.tv_sec+t.tv_nsec*1e-9;}
struct shm { sem_t empty, full; size_t len[SLOTS]; char buf[SLOTS][CH]; };
static double by_pipe(size_t total)
{
 int fd[2]; pipe(fd); static char buf[CH]; memset(buf, 7, CH);
 double t = now(); pid_t p = fork();
 if (p == 0) { close(fd[1]); static char rb[CH]; size_t got = 0; ssize_t r;
   while ((r = read(fd[0], rb, CH)) > 0) got += r; _exit(got == total ? 0 : 1); }
 close(fd[0]);
 for (size_t s = 0; s < total; s += CH) write(fd[1], buf, CH);
 close(fd[1]); int st; waitpid(p, &st, 0);
 if (WEXITSTATUS(st)) puts("pipe: size mismatch");
 return now() - t;
}
static double by_shm(size_t total)
{
 struct shm *m = mmap(NULL, sizeof *m, PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, -1, 0);
 sem_init(&m->empty, 1, SLOTS); sem_init(&m->full, 1, 0);
 double t = now(); pid_t p = fork();
 if (p == 0) { size_t got = 0; int out = 0;
   while (got < total) { sem_wait(&m->full); got += m->len[out]; out = (out+1)%SLOTS; sem_post(&m->empty); }
   _exit(got == total ? 0 : 1); }
 int in = 0;
 for (size_t s = 0; s < total; s += CH) { sem_wait(&m->empty); memset(m->buf[in], 7, CH); m->len[in] = CH; in = (in+1)%SLOTS; sem_post(&m->full); }
 int st; waitpid(p, &st, 0);
 if (WEXITSTATUS(st)) puts("shm: size mismatch");
 double d = now() - t; munmap(m, sizeof *m); return d;
}
int main(void)
{
 size_t mb[] = {1, 10, 100};
 printf("%-8s %-12s %-12s\n", "size", "pipe (s)", "shared mem (s)");
 for (int i = 0; i < 3; i++) {
  double a = 1e9, b = 1e9;
  for (int k = 0; k < 5; k++) { double x = by_pipe(mb[i]<<20); if (x<a) a=x; x = by_shm(mb[i]<<20); if (x<b) b=x; }
  printf("%-5zuMB  %-12.4f %-12.4f\n", mb[i], a, b);
 }
 return 0;
}
