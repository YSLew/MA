#include <stdio.h>
#include <string.h>

#include "shell.h"
#include "thread.h"

char stack[THREAD_STACKSIZE_MAIN];
int i = 0;

void *thread_handler(void *arg)
{
 	puts("I'm in \"thread\" now");
   	return NULL;
}

int main(void)
{
    puts("This is Task-03");

    /* ... */
    kernel_pid_t pid = thread_create(stack, sizeof(stack),THREAD_PRIORITY_MAIN - 1, THREAD_CREATE_STACKTEST, thread_handler, NULL, "thread_handler" );
    i = thread_getstatus(pid);
    printf("thread is %d\n", i);
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(NULL, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}

