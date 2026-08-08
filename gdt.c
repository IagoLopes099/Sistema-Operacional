#include "gdt.h"

#define GDT_ENTRIES 5

/* Um descritor de segmento (8 bytes), no formato que o hardware espera */
struct gdt_entry {
    unsigned short limit_low;
    unsigned short base_low;
    unsigned char  base_middle;
    unsigned char  access;
    unsigned char  granularity;
    unsigned char  base_high;
} __attribute__((packed));

/* A struct que o "lgdt" espera: tamanho da GDT + endereço dela */
struct gdt_ptr {
    unsigned short limit;
    unsigned int   base;
} __attribute__((packed));

extern void load_gdt(unsigned int gdt_ptr_address); /* definida em gdt.s */

static struct gdt_entry gdt[GDT_ENTRIES];
static struct gdt_ptr   gp;

/** gdt_set_entry:
 *  Preenche um descritor de segmento na GDT.
 *
 *  @param num    índice do descritor (0 = null, 1 = code, 2 = data)
 *  @param base   endereço base do segmento
 *  @param limit  tamanho do segmento
 *  @param access byte de acesso (presença, DPL, tipo)
 *  @param gran   byte de granularidade (tamanho de página, modo 32 bits)
 */
static void gdt_set_entry(int num, unsigned int base, unsigned int limit,
                           unsigned char access, unsigned char gran)
{
    gdt[num].base_low    = (base & 0xFFFF);
    gdt[num].base_middle = (base >> 16) & 0xFF;
    gdt[num].base_high   = (base >> 24) & 0xFF;

    gdt[num].limit_low   = (limit & 0xFFFF);
    gdt[num].granularity = (limit >> 16) & 0x0F;
    gdt[num].granularity |= (gran & 0xF0);

    gdt[num].access = access;
}

void gdt_install(void)
{
    gp.limit = (sizeof(struct gdt_entry) * GDT_ENTRIES) - 1;
    gp.base  = (unsigned int) &gdt;

    /* índice 0: descritor nulo, obrigatório e nunca usado */
    gdt_set_entry(0, 0, 0, 0, 0);

    /* índice 1 (offset 0x08): kernel code segment
     * base = 0, limit = 4GB, access = 0x9A (present, PL0, execute/read)
     * gran  = 0xCF (granularidade 4KB, modo 32 bits)
     */
    gdt_set_entry(1, 0, 0xFFFFFFFF, 0x9A, 0xCF);

    /* índice 2 (offset 0x10): kernel data segment
     * access = 0x92 (present, PL0, read/write)
     */
    gdt_set_entry(2, 0, 0xFFFFFFFF, 0x92, 0xCF);

    /* índice 3 (offset 0x18): user code segment (cap. 11.1)
     * access = 0xFA (present, PL3, execute/read) - igual ao kernel code
     * segment, so muda o DPL pra 3
     */
    gdt_set_entry(3, 0, 0xFFFFFFFF, 0xFA, 0xCF);

    /* índice 4 (offset 0x20): user data segment (cap. 11.1)
     * access = 0xF2 (present, PL3, read/write)
     */
    gdt_set_entry(4, 0, 0xFFFFFFFF, 0xF2, 0xCF);

    load_gdt((unsigned int) &gp);
}