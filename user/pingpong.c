#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"


#define READEND 0
#define WRITEEND 1


int main(int argc, char *argv[]) {
    int p[2];
    int pid;
    char content;
    if (argc > 1) {
        fprintf(2, "Usage: pingpong\n");
        exit(1);
    }
    pipe(p);
    if (fork() == 0) {
        // child
        pid = getpid();
        read(p[READEND], &content, 1);
        close(p[READEND]);
        printf("%d: received ping\n", pid);
        write(p[WRITEEND], "0", 1);
        close(p[WRITEEND]);
    }else {
        //parent
        pid = getpid();
        write(p[WRITEEND], "0", 1);
        close(p[WRITEEND]);
        wait(0);
        read(p[READEND], &content, 1);
        close(p[READEND]);
        printf("%d: received pong\n", pid);
    }
    exit(0);

}


