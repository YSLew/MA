#include <inttypes.h>
#include <stdio.h>
#include "xtimer.h"

#include "msg.h"
#include "thread.h"


static kernel_pid_t rcv_pid;
static char rcv_stack[THREAD_STACKSIZE_DEFAULT], test_stack[THREAD_STACKSIZE_DEFAULT];

static void *rcv(void *arg)
{
//    msg_t msg_req, msg_resp;
    (void)arg;
    while (1) {
//        msg_receive(&msg_req);
//        msg_resp.content.value = msg_req.content.value + 1;
//        msg_reply(&msg_req, &msg_resp);
	xtimer_sleep(1);
	puts("in rec thread.");
    }
    return NULL;
}

static void *test(void *arg)
{
	while(1) {
		xtimer_sleep(1);
		puts("in test thread");
		for(int x = 0; x < 10; x++)
			printf("test=%d \n",x);
		
	}
	return NULL;
}

int main(void)
{
//    msg_t msg_req, msg_resp;
//    msg_resp.content.value = 0;
    rcv_pid = thread_create(rcv_stack, sizeof(rcv_stack),
                            THREAD_PRIORITY_MAIN - 1, 0, rcv, NULL, "rcv");

	thread_create(test_stack, sizeof(rcv_stack),
                            THREAD_PRIORITY_MAIN - 1, 0, test, NULL, "test");

    while (1) {
//        msg_req.content.value = msg_resp.content.value;
//        msg_send_receive(&msg_req, &msg_resp, rcv_pid);
//        printf("Result: %" PRIu32 "\n", msg_resp.content.value);
	xtimer_usleep(1000000);
	puts("in main");
    }
    return 0;
}
