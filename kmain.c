#include "fb.h"
#include "serial.h"
#include "gdt.h"
#include "idt.h"
#include "pic.h"
#include "interrupt.h"


#define SERIAL_COM1_BASE 0x3F8

void kmain()
{
    gdt_install();
    idt_install();
    pic_remap();

    serial_configure_baud_rate(SERIAL_COM1_BASE, 2);
    serial_configure_line(SERIAL_COM1_BASE);
    serial_configure_buffer(SERIAL_COM1_BASE);
    serial_configure_modem(SERIAL_COM1_BASE);

    
    fb_write("Hello world! Digite algo: ", 27);

    enable_interrupts();   /* só depois da IDT e do PIC estarem prontos! */

    while (1) {
        ; /* o kernel agora fica esperando interrupcoes */
    }
}