#include <stdio.h>
#include <string.h>
#include "xtimer.h"

int main(void)
{
    while(1){
		xtimer_sleep(2);	
		puts("Hello World!");
	}
    return 0;
}
