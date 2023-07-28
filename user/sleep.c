#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[])
{
    int sleepTime;
    if (argc < 2) {
        fprintf(2, "usage: sleep <number>\n");
        exit(1);
    }
    sleepTime = atoi(argv[1]);
    sleep(sleepTime);
    exit(0);

}