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

#define buffer_size 18
#define sensor_id 8
#define pixel_size 10                               //pixel_size =sensor id +  sensor pixel size + end bit
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


int main(void)
{
    puts("This is D6t_8");
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

        printf ("ptat = %d\n\r", ptat);
        printf ("%d %4d %4d %4d %4d %4d %4d %4d\n\r",  p[0],  p[1],  p[2],  p[3],  p[4],  p[5],  p[6],  p[7]);

        for(i = 0; i < pixel_size; i++)
            {
                if(p[i] - ptat > c_diff)
                    pkt[i+1] = 1;
                else
                    pkt[i+1] = 0;
            }
        pkt[0] = sensor_id;
        pkt[9] = 99;

        printf ("%d %4d %4d %4d %4d %4d %4d %4d\n\r",  pkt[1],  pkt[2],  pkt[3],  pkt[4],  pkt[5],  pkt[6],  pkt[7], pkt[8]);
        puts("------------\n");

        if (sock_udp_send(&sock, pkt, sizeof(pkt), &remote) < 0)
        {
            puts("Error sending message");
            sock_udp_close(&sock);
            return 1;
        }

        xtimer_sleep(2);
    }
    return 0;
}