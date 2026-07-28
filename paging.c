#include "paging.h"

#define PAGE_DIR_ENTRIES 1024

#define PAGE_PRESENT  0x1   /* bit 0: pagina presente */
#define PAGE_RW       0x2   /* bit 1: leitura e escrita */
#define PAGE_SIZE_4MB 0x80  /* bit 7: esta entrada aponta pra uma pagina de 4MB */

extern void load_page_directory(unsigned int pd_address); /* em paging_asm.s */
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
 */
void paging_install(void)
{
    unsigned int i;

    for (i = 0; i < PAGE_DIR_ENTRIES; i++) {
        /* cada entrada cobre 4MB: entrada i mapeia o endereco fisico i*4MB */
        page_directory[i] = (i * 0x400000) | PAGE_PRESENT | PAGE_RW | PAGE_SIZE_4MB;
    }

    load_page_directory((unsigned int) page_directory);
    enable_paging();
}