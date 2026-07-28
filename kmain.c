#include "fb.h"
#include "serial.h"
#include "gdt.h"
#include "idt.h"
#include "pic.h"
#include "interrupt.h"
#include "paging.h"

#define SERIAL_COM1_BASE 0x3F8

void kmain(unsigned int ebx)
{
    (void) ebx; /* nao usado por enquanto — sera reaproveitado no cap. 10
                 * (page frame allocation), quando precisarmos do mapa de
                 * memoria que o GRUB entrega na struct multiboot_info_t */

    gdt_install();
    idt_install();
    paging_install();
    pic_remap();

    serial_configure_baud_rate(SERIAL_COM1_BASE, 2);
    serial_configure_line(SERIAL_COM1_BASE);
    serial_configure_buffer(SERIAL_COM1_BASE);
    serial_configure_modem(SERIAL_COM1_BASE);

    serial_write("Kernel inicializado (GDT, IDT, paginacao, PIC ok)\n", 51);

    fb_write("Hello world! Digite algo:\n", 27);

    enable_interrupts();

    while (1) {
        __asm__ volatile ("hlt"); /* dorme ate a proxima interrupcao chegar */
    }
}