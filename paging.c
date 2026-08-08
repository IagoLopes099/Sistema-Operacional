#include "paging.h"

#define PAGE_DIR_ENTRIES 1024

#define PAGE_PRESENT  0x1   /* bit 0: pagina presente */
#define PAGE_RW       0x2   /* bit 1: leitura e escrita */
#define PAGE_SIZE_4MB 0x80  /* bit 7: esta entrada aponta pra uma pagina de 4MB */

/* Onde o kernel enxerga a si mesmo na memoria virtual (higher half).
 * Precisa bater com o valor usado em loader.s e link.ld. */
#define KERNEL_VIRTUAL_BASE 0xC0000000
#define KERNEL_PDE_INDEX    (KERNEL_VIRTUAL_BASE >> 22)  /* = 768 */

extern void load_page_directory(unsigned int pd_physical_address); /* em paging_asm.s */
extern void enable_paging(void);                          /* em paging_asm.s */

/* O page directory PRECISA estar alinhado em 4KB (os 12 bits baixos do
 * endereco sao usados pelas flags, nao pelo endereco em si). O atributo
 * "aligned" garante isso — sem ele, o cr3 apontaria pra um lugar errado. */
static unsigned int page_directory[PAGE_DIR_ENTRIES] __attribute__((aligned(4096)));

/** paging_install:
 *  Configura identity paging usando paginas de 4MB: o endereco virtual X
 *  é mapeado exatamente para o endereco fisico X, para todo o espaco de
 *  enderecamento de 4GB. Isso nao muda nada visivelmente, mas ativa
 *  a MMU de verdade — é a base para paginacao "de verdade" depois.
 *
 *  Alem disso (cap. 9.3), o kernel roda na "higher half": o loader.s ja
 *  pulou pra rodar em enderecos virtuais >= 0xC0000000 antes mesmo dessa
 *  funcao ser chamada, usando um page directory temporario. Esse page
 *  directory DEFINITIVO precisa continuar honrando esse mapeamento, senao
 *  a troca de cr3 quebraria o kernel no meio da execucao.
 */
void paging_install(void)
{
    unsigned int i;
    unsigned int pd_physical_address;

    for (i = 0; i < PAGE_DIR_ENTRIES; i++) {
        /* cada entrada cobre 4MB: entrada i mapeia o endereco fisico i*4MB */
        page_directory[i] = (i * 0x400000) | PAGE_PRESENT | PAGE_RW | PAGE_SIZE_4MB;
    }

    /* excecao: a entrada que cobre 0xC0000000 (onde o kernel foi linkado)
     * nao pode apontar pro fisico 0xC0000000 (que provavelmente nem existe
     * na maquina) - ela tem que apontar pro MESMO frame fisico da entrada
     * 0, que e onde o GRUB realmente carregou o kernel. */
    page_directory[KERNEL_PDE_INDEX] = 0x00000000 | PAGE_PRESENT | PAGE_RW | PAGE_SIZE_4MB;

    /* page_directory e um simbolo do kernel: seu endereco, do ponto de
     * vista do C, ja e o endereco VIRTUAL (higher half). O registrador
     * cr3 exige o endereco FISICO, entao subtraimos a base virtual. */
    pd_physical_address = (unsigned int) page_directory - KERNEL_VIRTUAL_BASE;

    load_page_directory(pd_physical_address);
    enable_paging();
}