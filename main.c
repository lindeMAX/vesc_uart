#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "uart/uart.h"

int main(int argc, char *argv[])
{
    int rpm = -1;

    comm_uart_init();
    printf("Type in the desired bldc rpm (not erpm!).\n ");
    printf("You can type in '0' to quit the programm.\n");

    while (rpm != 0)
    {
        scanf("%i", &rpm);
        vesc_set_rpm(rpm);
    }

    // stop bldc motor
    vesc_set_rpm(0);

    // clean up uart an it's threads
    uart_close();

    exit(0);

    return 0;
}
