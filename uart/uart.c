#include "uart.h"

/* general uart stuff */

void uart_read()
{
    //----- CHECK FOR ANY RX BYTES -----
    if (fd != -1)
    {
        rx_buffer_length = read(fd, (void *)rx_buffer, 255); //Filestream, buffer to store in, number of bytes to read (max)
        if (rx_buffer_length < 0)
        {
            //An error occured (will occur if there are no bytes)
        }
        else if (rx_buffer_length == 0)
        {
            //No data waiting
        }
        else
        {
            // Bytes received
            // rx_buffer[rx_buffer_length] = '\0';
            // printf("%i bytes read : %s\n", rx_buffer_length, rx_buffer);

            for (uint8_t i = 0; i < rx_buffer_length; i++) {
                // for the vesc:
                /* Call this function every time a byte is received on the UART with the received byte.
                 * It will run the state machine in the packet assembler and the callbacks i
                 * n bldc interface will be called when the packets are ready
                */
                bldc_interface_uart_process_byte(rx_buffer[i]);
            }
        }
    }
}

void *uart_recieve_handler(void *val) {
    while (1) {
        uart_read();
    }
}

void uart_init()
{

    fd = open(MODEMDEVICE, O_RDWR | O_NOCTTY );
    if (fd <0) { perror(MODEMDEVICE); exit(-1); }

    tcgetattr(fd, &oldtio); /* save current serial port settings */
    bzero(&newtio, sizeof(newtio)); /* clear struct for new port settings */
    /* 
        BAUDRATE: Set bps rate. You could also use cfsetispeed and cfsetospeed.
        CRTSCTS : output hardware flow control (only used if the cable has
                  all necessary lines. See sect. 7 of Serial-HOWTO)
        CS8     : 8n1 (8bit,no parity,1 stopbit)
        CLOCAL  : local connection, no modem contol
        CREAD   : enable receiving characters
    */
    newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;

    /*
        IGNPAR  : ignore bytes with parity errors
        ICRNL   : map CR to NL (otherwise a CR input on the other computer
                  will not terminate input)
        otherwise make device raw (no other input processing)
    */
    newtio.c_iflag = IGNPAR;
    
    /*
        Raw output.
    */
    newtio.c_oflag = 0;

    /* set input mode (non-canonical, no echo,...) */
    newtio.c_lflag = 0;

    newtio.c_cc[VTIME]    = 0;   // inter-character timer unused 
    newtio.c_cc[VMIN]     = 1;   // blocking read until 1 chars received

    /* 
        now clean the modem line and activate the settings for the port
    */
    tcflush(fd, TCIFLUSH);
    tcsetattr(fd,TCSANOW,&newtio);

    // handle the input with a seperate thread
    int rc = pthread_create(&recieve_thread, NULL, &uart_recieve_handler, NULL);
    if (rc != 0)
    {
        printf("Failed creating recieve uart recieve thread!");
        exit(-1);
    }
}

void uart_transmit(unsigned char *data, unsigned int size)
{
    if (fd != -1)
    {
        int count = write(fd, data, size); //Filestream, bytes to write, number of bytes to write
        if (count < 0)
        {
            printf("UART TX error\n");
        }
    }
}

void uart_close()
{
    close(fd);
    pthread_cancel(recieve_thread);
    pthread_cancel(get_values_thread);
    pthread_cancel(set_erpm_thread);
}

// exec by a seperate thread to constantly get values
void *get_values(void *val) {
    while (1) {
        bldc_interface_get_values();
        sleep(VESC_UART_TIMEOUT);
    }
}

// Your init function
void comm_uart_init(void) {
	// Initialize your UART...
	uart_init();
	// Initialize the bldc interface and provide your send function
	bldc_interface_uart_init(uart_transmit);
	// Somewhere in your init code you can set your callback function(s).
	bldc_interface_set_rx_value_func(bldc_val_received);
	// start the sget_values_thread
	int rc = pthread_create(&get_values_thread, NULL, &get_values, NULL);
    	if (rc != 0)
    	{
        	printf("Failed creating get_values_thread!");
        	exit(-1);
    	}
}

void bldc_val_received(mc_values *val) {
	printf("Input voltage: %.2f V\r\n", val->v_in);
	printf("Temp_Mosf:     %.2f degC\r\n", val->temp_mos);
    	printf("Temp_Motor:    %.2f degC\r\n", val->temp_motor);
	printf("Current motor: %.2f A\r\n", val->current_motor);
	printf("Current in:    %.2f A\r\n", val->current_in);
	printf("eRPM:          %.1f eRPM\r\n", val->rpm);
    	printf("RPM:           %.1f RPM\r\n", val->rpm / (12 / 2));     // rpm = erpm / motor_pole_pairs
	printf("Duty cycle:    %.1f %%\r\n", val->duty_now * 100.0);
	printf("Ah Drawn:      %.4f Ah\r\n", val->amp_hours);
	printf("Ah Regen:      %.4f Ah\r\n", val->amp_hours_charged);
	printf("Wh Drawn:      %.4f Wh\r\n", val->watt_hours);
	printf("Wh Regen:      %.4f Wh\r\n", val->watt_hours_charged);
	printf("Tacho:         %i counts\r\n", val->tachometer);
	printf("Tacho ABS:     %i counts\r\n", val->tachometer_abs);
	printf("Fault Code:    %s\r\n", bldc_interface_fault_to_string(val->fault_code));
    	printf("\r\n");
}

// exec by set_erpm_thread
void *set_erpm(void *val) {
    while (1) {
        bldc_interface_set_rpm(erpm);
        sleep(VESC_UART_TIMEOUT);
    }
}

void vesc_set_rpm(int rpm) {
    // calc erpm from rpm
    erpm = rpm * (MOTOR_POLES / 2);
    int rc = pthread_create(&set_erpm_thread, NULL, &set_erpm, NULL);
    if (rc != 0)
    {
        printf("Failed creating set_erpm_thread!");
        exit(-1);
    }
}
