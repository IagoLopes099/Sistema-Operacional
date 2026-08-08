#include "user.h"
#include "gdt.h"
#include "paging.h"
#include "pfa.h"
#include "serial.h"

/* Seletores de segmento de modo usuario (cap. 11.1): offset na GDT (ver
 * gdt.c, indices 3 e 4) com RPL = 3 marcado nos 2 bits mais baixos, como
 * exigido pra usar iret entrando em PL3 (cap. 11.3). */
#define USER_CODE_SELECTOR (0x18 | 0x3)
#define USER_DATA_SELECTOR (0x20 | 0x3)

/* Regiao de 4MB dedicada ao processo de usuario. O cap. 9 deste projeto
 * usa paginas de 4MB (PSE) pro identity paging, entao nao existem page
 * tables de 4KB - a menor granularidade de protecao U/S disponivel e uma
 * entrada inteira do page directory. Usamos a entrada de indice 1
 * (0x00400000 - 0x007FFFFF), livre tanto do mapeamento baixo do kernel
 * (entrada 0) quanto da higher half (entrada 768 = 0xC0000000). Codigo e
 * pilha do processo de usuario moram os dois dentro dela. */
#define USER_REGION_VIRT 0x00400000u
#define USER_REGION_SIZE 0x00400000u
#define USER_STACK_TOP   (USER_REGION_VIRT + USER_REGION_SIZE - 4u)

/* enter_user_mode - definida em user_asm.s. Monta a pilha como se uma
 * interrupcao inter-privilegio tivesse ocorrido e executa iret (cap. 11.3).
 * Nunca retorna se tudo correr bem. */
extern void enter_user_mode(unsigned int data_sel, unsigned int code_sel,
                             unsigned int user_esp, unsigned int user_eip);

static void serial_write_string(const char *s)
{
    unsigned int len = 0;
    while (s[len] != '\0') {
        len++;
    }
    serial_write((char *) s, len);
}

int user_mode_start(multiboot_info_t *mbinfo)
{
    multiboot_module_t *mods;
    unsigned int program_addr, program_size, i;
    unsigned char *src, *dst;

    if (!(mbinfo->flags & MULTIBOOT_FLAG_MODS) || mbinfo->mods_count < 1) {
        serial_write_string("cap11: nenhum modulo do GRUB encontrado (programa de usuario)\n");
        return -1;
    }

    mods = (multiboot_module_t *) mbinfo->mods_addr;
    program_addr = mods[0].mod_start;
    program_size = mods[0].mod_end - mods[0].mod_start;

    if (program_size == 0 || program_size > USER_REGION_SIZE) {
        serial_write_string("cap11: programa de usuario invalido ou grande demais\n");
        return -1;
    }

    /* reserva a regiao no page frame allocator (cap. 10) pra ninguem mais
     * usa-la, ja que estamos escrevendo nela por fora do fluxo normal de
     * pfa_alloc_frame() */
    pfa_reserve_region(USER_REGION_VIRT, USER_REGION_SIZE);

    /* libera acesso PL3 pra essa regiao inteira de 4MB (cap. 11.2) */
    paging_allow_user_access(USER_REGION_VIRT);

    /* copia o binario do modulo (carregado pelo GRUB em algum lugar da
     * memoria fisica, ver cap. 7) pra dentro da regiao de usuario. Como o
     * kernel usa identity paging, os dois enderecos (origem e destino) ja
     * sao diretamente acessiveis como ponteiros comuns. */
    src = (unsigned char *) program_addr;
    dst = (unsigned char *) USER_REGION_VIRT;
    for (i = 0; i < program_size; i++) {
        dst[i] = src[i];
    }

    serial_write_string("cap11: saltando para o modo usuario (anel 3)...\n");

    /* cap. 11.3: entra em modo usuario. Se tudo estiver certo, o
     * processador passa a executar o programa copiado acima em PL3 e essa
     * chamada nunca retorna. */
    enter_user_mode(USER_DATA_SELECTOR, USER_CODE_SELECTOR,
                     USER_STACK_TOP, USER_REGION_VIRT);

    /* nunca deveria chegar aqui */
    return -1;
}
