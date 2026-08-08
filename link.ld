ENTRY(loader)                /* the name of the entry label */

SECTIONS {
    /* ------------------------------------------------------------------
     * Estagio de boot: carregado E linkado em 1 MB (sem relocacao).
     * Aqui vive so o cabecalho multiboot, o entry point e o page
     * directory temporario (ver loader.s) - tudo que precisa rodar
     * ANTES de a paginacao estar ligada, entao endereco fisico tem que
     * ser igual ao endereco que o linker resolve.
     * ------------------------------------------------------------------ */
    . = 0x00100000;          /* the code should be loaded at 1 MB */

    .text.boot ALIGN (0x1000) :
    {
        *(.text.boot)
    }

    /* ------------------------------------------------------------------
     * Daqui pra frente o kernel roda na "higher half" (0xC0000000 = 3GB).
     * O endereco fisico de carga continua logo em seguida (por isso o
     * AT()), so o endereco virtual/linkado que sobe 3GB.
     * ------------------------------------------------------------------ */
    . += 0xC0000000;

    /* Marca o inicio do kernel "de verdade" (cap. 10): o page frame
     * allocator usa esses simbolos pra saber quais frames fisicos NAO
     * pode entregar pra ninguem, porque ja estao ocupados pelo proprio
     * kernel. */
    kernel_virtual_start  = .;
    kernel_physical_start = . - 0xC0000000;

    .text ALIGN (0x1000) : AT(ADDR(.text) - 0xC0000000)   /* align at 4 KB */
    {
        *(.text)             /* all text sections from all files */
    }

    .rodata ALIGN (0x1000) : AT(ADDR(.rodata) - 0xC0000000) /* align at 4 KB */
    {
        *(.rodata*)          /* all read-only data sections from all files */
    }

    .data ALIGN (0x1000) : AT(ADDR(.data) - 0xC0000000)   /* align at 4 KB */
    {
        *(.data)             /* all data sections from all files */
    }

    .bss ALIGN (0x1000) : AT(ADDR(.bss) - 0xC0000000)     /* align at 4 KB */
    {
        *(COMMON)            /* all COMMON sections from all files */
        *(.bss)              /* all bss sections from all files */
    }

    kernel_virtual_end  = .;
    kernel_physical_end = . - 0xC0000000;
}