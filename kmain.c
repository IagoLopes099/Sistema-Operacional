#include "fb.h"
#include "serial.h"
#include "gdt.h"
#include "idt.h"
#include "pic.h"
#include "interrupt.h"
#include "multiboot.h"

#define SERIAL_COM1_BASE 0x3F8

typedef void (*call_module_t)(void);

void kmain(unsigned int ebx)
{
    gdt_install();
    idt_install();
    pic_remap();

    serial_configure_baud_rate(SERIAL_COM1_BASE, 2);
    serial_configure_line(SERIAL_COM1_BASE);
    serial_configure_buffer(SERIAL_COM1_BASE);
    serial_configure_modem(SERIAL_COM1_BASE);

    fb_write("Hello world! Digite algo:\n", 27);

    multiboot_info_t *mbinfo = (multiboot_info_t *) ebx;

    if ((mbinfo->flags & MULTIBOOT_FLAG_MODS) && mbinfo->mods_count == 1) {
        multiboot_module_t *module = (multiboot_module_t *) mbinfo->mods_addr;
        unsigned int address_of_module = module->mod_start;

        serial_write("Modulo encontrado, executando...\n", 34);

        call_module_t start_program = (call_module_t) address_of_module;
        start_program();
        /* nunca deveria chegar aqui, a nao ser que o programa retorne */
    } else {
        serial_write("Nenhum modulo carregado pelo GRUB.\n", 36);
    }

    enable_interrupts();

    while (1) {
        ;
    }
}