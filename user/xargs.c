#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/param.h"
#include "user/user.h"

#define NULL (int *)0

int main(int argc, char *argv[])
{
    char *new_argv[MAXARG];
    int cur_argv;
    for (cur_argv = 1; cur_argv <= argc - 1; cur_argv++)
    {
        // the first para in argv is the executable file
        new_argv[cur_argv - 1] = argv[cur_argv];
    }
    char ch;
    char buf[128];
    char *cur_buf = buf;
    new_argv[argc - 1] = buf;
    cur_argv = argc;
    while (read(0, &ch, sizeof(char)) == sizeof(char))
    {
        if (ch == ' ')
        {
            *cur_buf = '\0';
            cur_buf++;
            new_argv[cur_argv] = cur_buf;
            cur_argv++;
        }
        else if (ch == '\n')
        {
            *cur_buf = '\0';
            new_argv[cur_argv] = 0;
            int pid = fork();
            if (pid == 0)
            {
                // exec(cmd, para)
                exec(new_argv[0], new_argv);
                exit(0);
            }
            else
            {
                wait(NULL);
                cur_buf = buf;
                cur_argv = argc;
            }
        }
        else
        {
            *cur_buf = ch;
            cur_buf++;
        }
    }
    exit(0);
}