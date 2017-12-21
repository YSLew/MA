#include <stdio.h>
#include <string.h>

#include "shell.h"
//#include "board.h"	already included in led.h
#include "led.h" // 	only 1 led available
#include "periph/gpio.h"		//button sw0 : pa28

#define led GPIO_PIN(0, 19)
#define sw0 GPIO_PIN(0, 28)
#define sda GPIO_PIN(0, 16)
#define scl GPIO_PIN(0, 17)
#define twotwo GPIO_PIN(0, 22)
#define onesix GPIO_PIN(0, 16)


int i;

int echo(int argc, char **argv)
{
    printf("The first argument of the array is %s.\n", argv[0]);
    for (i = 1; i < argc; i++)
    {
    	printf("%d : %s\n", i , argv[i]);
    }
    /* ... */
    return 0;
}

int ffunction(int argc, char **argv)
{
	puts("Yippie");
	return 0;
}

int led_0_on(int argc, char **argv)
{
	LED0_ON;
	puts("LED0 is on");
	return 0;
}

int led_0_off(int argc, char **argv)
{
	LED0_OFF;
	puts("LED0 is off");
	return 0;
}

int toggle(int argc, char **argv)
{
	LED0_TOGGLE;
	puts("LED0 toggled");
	return 0;
}

int led_toggle(int argc, char **argv)
{
	gpio_toggle(led);
	puts("led toggled");
	return 0;
}

int sda_t(int argc, char **argv)
{
	gpio_toggle(sda);
	puts("sda toggled");
	return 0;
}

int tt_t(int argc, char **argv)
{
	gpio_toggle(twotwo);
	puts("tt toggled");
	return 0;
}

int tt_s(int argc, char **argv)
{
	gpio_set(twotwo);
	puts("tt toggled");
	return 0;
}

int os_s(int argc, char **argv)
{
	gpio_set(onesix);
	puts("16 set");
	return 0;
}

int os_t(int argc, char **argv)
{
	gpio_toggle(onesix);
	puts("16 toggled");
	return 0;
}

void button_pressed(int argc, char **argv)
{
	LED0_TOGGLE;	
}


static const shell_command_t commands[] = {
	{"first", "Hello there", ffunction},
	{"echo", "echo first argument", echo},
	{"LED0_on", "on", led_0_on},
	{"LED0_off", "off", led_0_off},
	{"Toggle","toggle", toggle},
	{"led_toggle", "led toggle", led_toggle},
	{"sda_t","sda toggle", sda_t},
	{"tt_t", "tt_t", tt_t},
	{"tt_s", "tt_s", tt_s},
	{"os_s", "os_s", os_s},
	{"os_t", "os_t", os_t},
    { NULL, NULL, NULL }
	
};

int main(void)
{
    puts("This is Task-02");
	int x = gpio_init(led, GPIO_OUT);
	printf("init: %d\n", x);
	gpio_init(sda, GPIO_OUT);
	gpio_init(scl, GPIO_OUT);
	gpio_init(twotwo, GPIO_OUT);

//	gpio_irq_enable(sw0);
//	gpio_cb_t bp = button_pressed;
//	x = gpio_init_int(sw0, GPIO_IN, GPIO_FALLING, (gpio_cb_t)bp, NULL);

    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
