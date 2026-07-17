#include "idt.h"

#define IDT_ENTRIES     256
#define IDT_USED_ENTRIES 48   /* 32 exceções da CPU + 16 IRQs remapeadas */

struct idt_entry {
    unsigned short base_low;
    unsigned short sel;
    unsigned char  always0;
    unsigned char  flags;
    unsigned short base_high;
} __attribute__((packed));

struct idt_ptr {
    unsigned short limit;
    unsigned int   base;
} __attribute__((packed));

extern void load_idt(unsigned int idt_ptr_address);      /* em isr.s */
extern unsigned int interrupt_handler_table[];            /* em isr.s */

static struct idt_entry idt[IDT_ENTRIES];
static struct idt_ptr   ip;

static void idt_set_gate(unsigned char num, unsigned int base,
                          unsigned short sel, unsigned char flags)
{
    idt[num].base_low  = base & 0xFFFF;
    idt[num].base_high = (base >> 16) & 0xFFFF;
    idt[num].sel       = sel;
    idt[num].always0   = 0;
    idt[num].flags     = flags;
}

void idt_install(void)
{
    unsigned int i;

    ip.limit = (sizeof(struct idt_entry) * IDT_ENTRIES) - 1;
    ip.base  = (unsigned int) &idt;

    /* zera tudo primeiro (entradas não usadas ficam "não presentes") */
    for (i = 0; i < IDT_ENTRIES; i++) {
        idt_set_gate(i, 0, 0, 0);
    }

    /* 0x08 = seletor do kernel code segment (definido no capítulo 5)
     * 0x8E = presente, DPL 0, interrupt gate de 32 bits
     */
    for (i = 0; i < IDT_USED_ENTRIES; i++) {
        idt_set_gate(i, interrupt_handler_table[i], 0x08, 0x8E);
    }

    load_idt((unsigned int) &ip);
}