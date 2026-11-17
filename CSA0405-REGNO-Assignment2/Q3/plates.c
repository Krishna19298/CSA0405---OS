#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#define CAMERAS 64
static double now(void){struct timespec t;clock_gettime(CLOCK_MONOTONIC,&t);return t.tv_sec+t.tv_nsec*1e-9;}
static int io_ms = 0;            /* simulated camera read wait per frame */
static volatile unsigned sink;
static void process_frame(int cam)
{
    if (io_ms) usleep(io_ms * 1000);                 /* wait for camera frame (I/O) */
    unsigned x = cam + 1;
    for (long i = 0; i < 3000000; i++) x = x * 1664525u + 1013904223u;  /* "read plate" CPU work */
    sink ^= x;
}
static int next_cam = 0, nthreads;
static pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
static void *pool_worker(void *a)
{
    (void)a;
    for (;;) {
        pthread_mutex_lock(&m); int c = next_cam < CAMERAS ? next_cam++ : -1; pthread_mutex_unlock(&m);
        if (c < 0) break;
        process_frame(c);
    }
    return NULL;
}
static void *one_cam(void *a) { process_frame((int)(long)a); return NULL; }
int main(int argc, char **argv)
{
    io_ms = argc > 1 ? atoi(argv[1]) : 0;
    double t = now();
    for (int c = 0; c < CAMERAS; c++) process_frame(c);
    double serial = now() - t;
    printf("io_ms=%d serial: %.3f s\n", io_ms, serial);
    pthread_t th[CAMERAS];
    t = now();
    for (long c = 0; c < CAMERAS; c++) pthread_create(&th[c], NULL, one_cam, (void *)c);
    for (int c = 0; c < CAMERAS; c++) pthread_join(th[c], NULL);
    double d = now() - t;
    printf("64 threads (1 per camera): %.3f s speed-up %.2f\n", d, serial / d);
    int sizes[] = {4, 8, 16};
    for (int s = 0; s < 3; s++) {
        next_cam = 0; nthreads = sizes[s];
        t = now();
        for (int i = 0; i < nthreads; i++) pthread_create(&th[i], NULL, pool_worker, NULL);
        for (int i = 0; i < nthreads; i++) pthread_join(th[i], NULL);
        d = now() - t;
        printf("pool of %2d threads: %.3f s speed-up %.2f\n", nthreads, d, serial / d);
    }
    return 0;
}
