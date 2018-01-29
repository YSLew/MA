#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>

#include "shell.h"
#include "thread.h"
#include "xtimer.h"
#include "periph/i2c.h"
//#include "net/gnrc/rpl.h"
//#include "net/af.h"
//#include "net/protnum.h"
//#include "net/sock/udp.h"
//#include "net/sock/util.h"

#include "net/emcute.h"
#include "net/ipv6/addr.h"
#include "msg.h"

#define buffer_size         18
#define sensor_id           8
#define pixel_size          10                               //pixel_size =sensor id +  sensor pixel size + end bit
#define c_diff              25

#define EMCUTE_PORT         (1883U)                          //UDP port used for listening
#define EMCUTE_ID           ("gertrud")
#define EMCUTE_PRIO         (THREAD_PRIORITY_MAIN - 1)
#define RCV_QUEUE_SIZE      (8)

//#define NUMOFSUBS           (16U)
//#define TOPIC_MAXLEN        (64U)

static char stack[THREAD_STACKSIZE_DEFAULT];
//static emcute_sub_t subscriptions[NUMOFSUBS];
//static char topics[NUMOFSUBS][TOPIC_MAXLEN];
static kernel_pid_t emcute_pid;

//char server_addr[50] = "[fe80::7b78:3f01:b6a3:c2e]:8791";                  //server address and port
char server_addr[25]        = "fe80::7b78:3f01:b6a3:c2e";
char server_port_num[5]     = "1886";
char * sensor_data_topic = "SD/D6t_8";
char * last_message = "Disconnected";
char * message = "0";
//uint8_t last_message = 0x32;
//uint8_t message = 0x30;

i2c_t i2c;
i2c_speed_t speed = I2C_SPEED_NORMAL;		//100 kbit/sec
volatile int i;
uint8_t buffer[buffer_size];
int ptat, status;
int p[pixel_size], pd[pixel_size], p_old[pixel_size];
int written, read;
uint8_t pkt[pixel_size];

//static msg_t rcv_queue[RCV_QUEUE_SIZE];

static void *emcute_thread(void *arg)
{
    (void)arg;
    emcute_run(EMCUTE_PORT, EMCUTE_ID);
    return NULL;    /* should never be reached */
}

int data_pub(char *name, const char *data)
{
    emcute_topic_t t;
    unsigned flags = EMCUTE_QOS_0;
    /*get topic id*/
    t.name = name;
    if (emcute_reg(&t) != EMCUTE_OK)
    {
        puts("error: unable to obtain topic ID");
        return 1;
    }
    /*publish data*/
    if (emcute_pub(&t, message, strlen(message), flags) != EMCUTE_OK)
    {
        puts("unable to publish data");
        return 1;
    }
    puts("data published");
    return 0;

}

int main(void)
{
    puts("This is D6t_8");
    status = i2c_init_master(i2c, speed);
    printf("init_master: %d\n", status); 
    printf("speed: %d\n", speed);
    status =  i2c_acquire(i2c);
    printf("bus acquired: %d\n", status);

    /* initialize our subscription buffers */
//    memset(subscriptions, 0, (NUMOFSUBS * sizeof(emcute_sub_t)));

    /* start the emcute thread */
    emcute_pid = thread_create(stack, sizeof(stack), EMCUTE_PRIO, 0, emcute_thread, NULL, "emcute");
    /* connect to gateway */
    sock_udp_ep_t gw = { .family = AF_INET6, .port = EMCUTE_PORT};
    if (ipv6_addr_from_str((ipv6_addr_t *)&gw.addr.ipv6, server_addr) == NULL)
        {
            puts("error parsing IPv6 address");
            return 1;
        }
    if (emcute_con(&gw, true, sensor_data_topic, (char*)last_message, sizeof(last_message), 0) != EMCUTE_OK)
    {
        puts("error: unable to connect to gateway!!");
        return 1;
    }

    /* interact with sensor and publish data */
    while(1)
    {
    	written = i2c_write_byte(i2c, 0x0a, 0x4c);
    	printf("written: %d\n", written);
    	read = i2c_read_bytes(i2c, 0x0a, buffer, buffer_size);
    	printf("read: %d\n", read);

        ptat = 256*(int)buffer[1] + (int)buffer[0];
        for(i = 0; i < pixel_size; i++)
            {p[i] = 256*(int)buffer[(i*2)+3] + (int)buffer[(i*2)+2];}

        printf ("ptat = %d\n\r", ptat);
        printf ("%d %4d %4d %4d %4d %4d %4d %4d\n\r",  p[0],  p[1],  p[2],  p[3],  p[4],  p[5],  p[6],  p[7]);

      
        for(i = 0; i < pixel_size; i++)
            {
                if(p[i] - ptat > c_diff)
                    {
                        pkt[i+1] = 1;
                        message = "1";
                        data_pub(sensor_data_topic, message);
                        break;
                    }
                else
                {
                    pkt[i+1] = 0;
                    if(i==7)
                    {
                        message = "0";
                        data_pub(sensor_data_topic, message);
                    }

                }
            }

        pkt[0] = sensor_id;
        pkt[9] = 99;

        printf ("%d %4d %4d %4d %4d %4d %4d %4d\n\r",  pkt[1],  pkt[2],  pkt[3],  pkt[4],  pkt[5],  pkt[6],  pkt[7], pkt[8]);
        puts("------------\n");

//        if (msg_try_send(&msg, emcute_pid) == 1)
//            {puts("msg sent from main thread");}
//        else
//            puts("msg not sent");

        xtimer_sleep(5);
    }
    return 0;
}