#include "fb.h"
#include "serial.h"
#include "gdt.h"

#define SERIAL_COM1_BASE 0x3F8

void kmain()
{
    gdt_install();

    serial_configure_baud_rate(SERIAL_COM1_BASE, 2);   // 115200/2 = 57600 bps
    serial_configure_line(SERIAL_COM1_BASE);           // 8N1
    serial_configure_buffer(SERIAL_COM1_BASE);         // FIFO
    serial_configure_modem(SERIAL_COM1_BASE);          // RTS/DTR

    serial_write("Log iniciado via serial!\n", 26);
    fb_write("Hello world!", 12);                         // continua indo pra tela
}