#include <stdio.h>
#include <string.h>
#include <math.h>

#include "shell.h"
#include "thread.h"
#include "xtimer.h"
#include "periph/i2c.h"

#define buffer_size 35
#define pixel_size 16

i2c_t i2c;
i2c_speed_t speed = I2C_SPEED_NORMAL;		//100 kbit/sec
int i, status;
uint8_t buffer[buffer_size];
int ptat;
int p[pixel_size], pd[pixel_size], p_old[pixel_size];
int written, read;
uint8_t x;

int sendbyte0a(int argc, char **argv)
{
    written = i2c_write_byte(i2c, 0x0a, 0x4c);
    printf("written: %d\n", written);
    return 0;
}

int sendbyte00(int argc, char **argv)
{
    written = i2c_write_byte(i2c, 0x00, 0x4c);
    printf("written: %d\n", written);
    return 0;
}

int sendbyteff(int argc, char **argv)
{
    written = i2c_write_byte(i2c, 0xff, 0x4c);
    printf("written: %d\n", written);
    return 0;
}

int readbytes(int argc, char **argv)
{
    read = i2c_read_bytes(i2c, 0x0a, buffer, buffer_size);
    printf("read: %d\n", read);
    ptat = 256*(int)buffer[1] + (int)buffer[0];
    for(i = 0; i < pixel_size; i++)
    p[i] = 256*(int)buffer[(i*2)+3] + (int)buffer[(i*2)+2];

    printf ("ptat = %d\n\r", ptat);
    printf ("%4d %4d %4d %4d\n\r",  p[0],  p[1],  p[2],  p[3]);
    printf ("%4d %4d %4d %4d\n\r",  p[4],  p[5],  p[6],  p[7]);
    printf ("%4d %4d %4d %4d\n\r",  p[8],  p[9], p[10], p[11]);
    printf ("%4d %4d %4d %4d\n\r",  p[12], p[13], p[14], p[15]);

    return 0;
}

int acq(int argc, char **argv)
{
    int acq = i2c_acquire(i2c);
    printf("acq: %d\n", acq);
    return 0;
}

int rele(int argc, char **argv)
{
    int rele = i2c_release(i2c);
    printf("released: %d\n", rele);
    return 0;
}

static const shell_command_t commands[] = {
    {"sb0a", "send 1 byte", sendbyte0a},
//    {"sb00", "send 1 byte", sendbyte00},
//    {"sbff", "send 1 byte", sendbyteff},
    {"readbytes", "read 35 bytes", readbytes},
    {"acq", "acquire bus", acq},
    {"rele", "release bus", rele},
    { NULL, NULL, NULL }
    
};

int main(void)
{
    puts("This is D6t_test");
    status = i2c_init_master(i2c, speed);
    printf("init_master: %d\n", status); 
    status =  i2c_acquire(i2c);
    printf("bus acquired: %d\n", status);
    printf("speed: %d\n", speed);

//    for(i = 0; i < 8; i++)
//   	tp_old[i] = 0;


    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}