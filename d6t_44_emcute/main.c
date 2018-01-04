#include <stdio.h>
#include <string.h>
#include <math.h>

#include "shell.h"
#include "thread.h"
#include "xtimer.h"
#include "periph/i2c.h"
#include "net/gnrc/rpl.h"
#include "net/af.h"
#include "net/protnum.h"
#include "net/ipv6/addr.h"
#include "net/sock/udp.h"
#include "net/sock/util.h"

#define buffer_size 34
#define sensor_id 44
#define pixel_size 18                               //pixel_size =sensor id +  sensor pixel size + end bit
#define c_diff 25

char server_addr[50] = "[fe80::7b78:3f01:b6a3:c2e]:8791";                  //server address and port

i2c_t i2c;
i2c_speed_t speed = I2C_SPEED_NORMAL;		//100 kbit/sec
int i, status;
uint8_t buffer[buffer_size];
int ptat;
int p[pixel_size], pd[pixel_size], p_old[pixel_size];
int written, read;

uint8_t pkt[pixel_size];
//uint8_t test[10] = {44,1,0,1,0,1,0,1,0,99};

int main(void)
{
    puts("This is D6t_44");
    status = i2c_init_master(i2c, speed);
    printf("init_master: %d\n", status); 
    printf("speed: %d\n", speed);
    status =  i2c_acquire(i2c);
    printf("bus acquired: %d\n", status);

    if(gnrc_rpl_init(7)<0)
    {
        gnrc_rpl_init(6);
        puts("initialised rpl on iface 6");
    }
    else
        puts("initialised rpl on iface 7");

    sock_udp_ep_t local = SOCK_IPV6_EP_ANY;
    sock_udp_t sock;
    local.port = 0xabcd;
    if (sock_udp_create(&sock, &local, NULL, 0) < 0)
    {
        puts("Error creating UDP sock");
        return 1;
    }

    sock_udp_ep_t remote = { .family = AF_INET6 };
    sock_udp_str2ep(&remote, server_addr);                      //convert string into UDP endpoint structure


    while(1)
    {
    	written = i2c_write_byte(i2c, 0x0a, 0x4c);
    	printf("written: %d\n", written);
    	read = i2c_read_bytes(i2c, 0x0a, buffer, buffer_size);
    	printf("read: %d\n", read);

        ptat = 256*(int)buffer[1] + (int)buffer[0];
        for(i = 0; i < pixel_size; i++)
        p[i] = 256*(int)buffer[(i*2)+3] + (int)buffer[(i*2)+2];

        for(i = 0; i < pixel_size-1; i++)
            {
                if(p[i] - ptat > c_diff)
                    pkt[i+1] = 1;
                else
                    pkt[i+1] = 0;
            }
        pkt[0] = sensor_id;
        pkt[17] = 99;

        printf ("ptat = %d\n\r", ptat);
        printf ("%4d %4d %4d %4d\n\r",  pkt[1],  pkt[2],  pkt[3],  pkt[4]);
        printf ("%4d %4d %4d %4d\n\r",  pkt[5],  pkt[6],  pkt[7],  pkt[8]);
        printf ("%4d %4d %4d %4d\n\r",  pkt[9],  pkt[10], pkt[11], pkt[12]);
        printf ("%4d %4d %4d %4d\n\r",  pkt[13], pkt[14], pkt[15], pkt[16]);

    	puts("------------\n");

        if (sock_udp_send(&sock, pkt, sizeof(pkt), &remote) < 0)
        {
            puts("Error sending message");
            sock_udp_close(&sock);
            return 1;
        }

        xtimer_sleep(2);
    }
/*
        static const shell_command_t shell_commands[] = {
            { NULL, NULL, NULL }
        };
        puts("All up, running the shell now");
        char line_buf[SHELL_DEFAULT_BUFSIZE];
        shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

        return -1;
*/

    return 0;
}
