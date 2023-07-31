#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define READEND 0
#define WRITEEND 1
// TODO: Not understanding the method of this program
int main(int argc, char* argv[]) {
    if (argc > 1) {
        fprintf(2, "Usage: primes\n");
        exit(1);
    }
    int numbers[36];
    int pfd[2];
    int idx = 0;
    int pid;
    for (int i = 2; i <= 35; i++) {
        numbers[idx++] = i;
    }
    while (idx > 0) {
        pipe(pfd);
        if ((pid = fork()) < 0) {
            fprintf(2, "fork error\n");
            exit(0);
        }
        else if(pid > 0) {
            // father
            close(pfd[READEND]);
            for (int i = 0; i < idx; ++i) {
                write(pfd[WRITEEND], &numbers[i], sizeof(int));
            }
            close(pfd[WRITEEND]);
            wait((int *) 0);
            exit(0);
        }
        else {
            // child
            close(pfd[WRITEEND]);
            int prime = 0;
            int temp = 0;
            idx = -1;
            while (read(pfd[READEND], &temp, sizeof(int)) != 0) {
                if (idx < 0) {
                    prime = temp;
                    idx++;
                }else {
                    if (temp % prime != 0) {
                        numbers[idx++] = temp;
                    }
                }
            }
            printf("prime %d\n", prime);
            close(pfd[READEND]);
        }
    }
    exit(0);
}