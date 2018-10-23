/* This lib manages the uart communication
 * Big parts of the code are copied from:
 * http://www.tldp.org/HOWTO/Serial-Programming-HOWTO/x115.html *  * The vesc uart part is made followed by the instructions on: * http://vedder.se/2015/10/communicating-with-the-vesc-using-uart/ */

#ifndef UART_h
#define UART_h

#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/signal.h>
#include <sys/types.h>
#include <pthread.h>        // for multithreading 

// for vesc
#include "libs_vesc_uart/bldc_interface_uart.h"
#include "libs_vesc_uart/bldc_interface.h"
#include "libs_vesc_uart/datatypes.h"

#define BAUDRATE B115200
#define MODEMDEVICE "/dev/serial0"

#define MOTOR_POLES 12
#define VESC_UART_TIMEOUT 1

#ifdef __cplusplus
extern "C"
{
#endif

    /* private variables: */
    int fd;
    volatile unsigned char rx_buffer[256];
    volatile unsigned int rx_buffer_length;
    volatile int erpm;
    struct termios oldtio, newtio;
    pthread_t recieve_thread;
    pthread_t get_values_thread;
    pthread_t set_erpm_thread;

    /* general uart functions */
    void uart_read();
    void *uart_recieve_handler(void *val);
    void uart_init(); // sets up the uart interfaces
    void uart_transmit(unsigned char *data, unsigned int size);
    void uart_close();

    /* vesc specific functions */
    void comm_uart_init();
    void bldc_val_received();
    void *get_values(void *val);
    void vesc_set_rpm(int rpm);

#ifdef __cplusplus
    extern "C"
    {
#endif

#endif
