# vesc_uart
communicating with vesc via uart

This is an example of how to get the motor contol values from the vesc and set the rpm
on a RaspberryPi Zero W via uart.

It's done by following the instructions at http://vedder.se/2015/10/communicating-with-the-vesc-using-uart/.
But the code provided in https://github.com/vedderb/bldc_uart_comm_stm32f4_discovery is a bit outdated.
So i made some changes to bldc_interface.c especially the bldc_interface_process_packet() function and datatypes.h.

Now (at least) the bldc_interface_get_values() function and bldc_interface_process_packet() (the "COMM_GET_VALUES" part)
are working properly at hardware version 4.10 with the 3.40 firmware installed.
This is what i have tested!

If you want to port this to another hardware than Pi Zero, you just have to reimplement uart.h and uart.c.
You will find the instructions for that, as mentioned before, at http://vedder.se/2015/10/communicating-with-the-vesc-using-uart/.

To compile it for the Pi Zero use the toolchains given in https://github.com/raspberrypi/tools with the following command:
"path to toolchain"/arm-linux-gnueabihf-gcc -pthread -mcpu=arm1176jzf-s -mfpu=vfp -std=c99 -g -Wall -o/media/max/RastermannPi/firmware/RastermannPi/bin/debug/RastermannPi uart/libs_vesc_uart/packet.c uart/libs_vesc_uart/crc.c uart/libs_vesc_uart/buffer.c uart/libs_vesc_uart/bldc_interface_uart.c uart/libs_vesc_uart/bldc_interface.c uart/uart.c main.c

If everything works, fine you should get some output like this:

Input voltage: 12.60 V
Temp_Mosf:     26.50 degC
Temp_Motor:    -74.90 degC
Current motor: 0.58 A
Current in:    0.46 A
eRPM:          15814.0 eRPM
RPM:           2635.7 RPM
Duty cycle:    94.9 %
Ah Drawn:      0.0826 Ah
Ah Regen:      0.0000 Ah
Wh Drawn:      1.0458 Wh
Wh Regen:      0.0016 Wh
Tacho:         609870 counts
Tacho ABS:     609924 counts
Fault Code:    FAULT_CODE_NONE

I hope this will be usefull to you.



