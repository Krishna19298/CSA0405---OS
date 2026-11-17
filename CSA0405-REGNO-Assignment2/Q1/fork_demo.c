#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
int main(int argc, char **argv)
{
    int n = argc > 1 ? atoi(argv[1]) : 3;
    printf("parent pid=%d\n", getpid());
    fflush(stdout);
    for (int i = 0; i < n; i++) {
        pid_t pid = fork();
        if (pid < 0) { perror("fork"); return 1; }
        if (pid == 0) {
            printf("child %d: pid=%d ppid=%d\n", i + 1, getpid(), getppid());
            fflush(stdout);
            sleep(3 + i);              /* stay alive so ps can see us */
            _exit(10 + i);
        }
    }
    fflush(stdout);
    for (int i = 0; i < n; i++) {
        int st; pid_t p = wait(&st);
        printf("parent: child %d finished, exit code %d\n", p, WEXITSTATUS(st));
    }
    return 0;
}
