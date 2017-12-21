#include <stdio.h>
#include <string.h>

#include "shell.h"
#include "thread.h"
#include "xtimer.h"

char stack[THREAD_STACKSIZE_MAIN];
int i;
xtimer_ticks32_t now;

void *thread_handler(void *arg)
{
    while(1)
    /* ... */
    {
        now = xtimer_now();
//        printf("Time: %fn",now);
        xtimer_sleep(2);
        puts("In\n");   

    }
    return NULL;
}

int main(void)
{
    puts("This is Task-04");

    kernel_pid_t pid = thread_create(stack, sizeof(stack),
                                     THREAD_PRIORITY_MAIN - 1,
                                     THREAD_CREATE_STACKTEST,
                                     thread_handler, NULL,
                                     "thread");
    i = thread_getstatus(pid);
    printf("thread is %d\n", i);
    printf("pid is %d\n", pid);
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(NULL, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}

