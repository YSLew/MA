#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>

#include "thread.h"
#include "xtimer.h"
#include "periph/i2c.h"


#define buffer_size         19                              //(sensor pixel size *2) + 3
#define sensor_id           8
#define pixel_size          10                               //pixel_size =sensor id +  sensor pixel size + end bit



i2c_t i2c;
i2c_speed_t speed = I2C_SPEED_NORMAL;		//100 kbit/sec
volatile int i;
uint8_t buffer[buffer_size];
int ptat, status;
int p[pixel_size], pd[pixel_size], p_old[pixel_size];
int written, read;
uint8_t pkt[pixel_size];



int main(void)
{
    puts("This is D6t_8");
    status = i2c_init_master(i2c, speed);
//    printf("init_master: %d\n", status); 
//    printf("speed: %d\n", speed);
    status =  i2c_acquire(i2c);
//    printf("bus acquired: %d\n", status);

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
    }
    return 0;
}
