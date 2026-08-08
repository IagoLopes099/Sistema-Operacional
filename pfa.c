#include "pfa.h"

/* Suporta ate 1GB de RAM (256k frames de 4KB = 32KB de bitmap). E mais do
 * que suficiente pra rodar em QEMU/Bochs; se a maquina tiver menos RAM
 * (o normal), so uma parte do bitmap e realmente usada. */
#define MAX_FRAMES        (1024u * 1024u * 1024u / PAGE_FRAME_SIZE)
#define BITMAP_SIZE_WORDS (MAX_FRAMES / 32)

/* 1 = frame ocupado (ou fora dos limites da RAM real), 0 = frame livre.
 * Comeca tudo em "ocupado": so liberamos explicitamente o que sabemos
 * que e RAM de verdade em pfa_init(). */
static unsigned int bitmap[BITMAP_SIZE_WORDS];

static unsigned int total_frames = 0;
static unsigned int free_frames  = 0;

/* onde comecar a procurar o proximo frame livre. So um "chute" pra nao
 * precisar varrer o bitmap inteiro toda vez a partir do zero - ajuda a
 * manter frames alocados em sequencia tambem, o que o kernel heap
 * (kheap.c) aproveita pra pedir varios frames "quase contiguos". */
static unsigned int next_free_hint = 0;

/* Simbolos exportados pelo link.ld - marcam onde o kernel (codigo, dados,
 * bss) realmente esta na memoria fisica. */
extern unsigned int kernel_physical_start;
extern unsigned int kernel_physical_end;

static void set_frame(unsigned int frame_idx)
{
    bitmap[frame_idx / 32] |= (1u << (frame_idx % 32));
}

static void clear_frame(unsigned int frame_idx)
{
    bitmap[frame_idx / 32] &= ~(1u << (frame_idx % 32));
}

static int frame_is_set(unsigned int frame_idx)
{
    return (bitmap[frame_idx / 32] & (1u << (frame_idx % 32))) != 0;
}

/* Marca como OCUPADA (bit = 1) a faixa de memoria fisica [addr, addr+length),
 * arredondando pra cima/baixo o suficiente pra cobrir frames parcialmente
 * dentro da faixa (mais seguro reservar de mais do que de menos). */
static void mark_region_used(unsigned int addr, unsigned int length)
{
    unsigned int start_frame = addr / PAGE_FRAME_SIZE;
    unsigned int end_frame   = (addr + length + PAGE_FRAME_SIZE - 1) / PAGE_FRAME_SIZE;
    unsigned int i;

    if (end_frame > MAX_FRAMES) {
        end_frame = MAX_FRAMES;
    }

    for (i = start_frame; i < end_frame; i++) {
        if (!frame_is_set(i)) {
            set_frame(i);
            free_frames--;
        }
    }
}

void pfa_init(multiboot_info_t *mbinfo)
{
    unsigned int i;
    unsigned int mem_upper_kb;
    unsigned int total_bytes;
    unsigned int kstart, kend;

    /* comeca marcando TUDO como ocupado - so libera depois o que sabemos
     * que e RAM utilizavel de verdade */
    for (i = 0; i < BITMAP_SIZE_WORDS; i++) {
        bitmap[i] = 0xFFFFFFFF;
    }
    total_frames = 0;
    free_frames  = 0;

    if (!(mbinfo->flags & MULTIBOOT_FLAG_MEMINFO)) {
        /* o GRUB nao nos deu informacao de memoria - ficamos sem nenhum
         * frame livre mesmo, mais seguro do que inventar um numero */
        return;
    }

    /* mem_upper = KB de memoria continua a partir de 1MB (o multiboot nao
     * garante nada sobre o espaco entre 640KB e 1MB, que e cheio de
     * memoria mapeada de video/BIOS, entao nem tentamos usar essa parte) */
    mem_upper_kb = mbinfo->mem_upper;
    total_bytes  = (1024u * 1024u) + mem_upper_kb * 1024u;

    total_frames = total_bytes / PAGE_FRAME_SIZE;
    if (total_frames > MAX_FRAMES) {
        total_frames = MAX_FRAMES;
    }

    /* libera a regiao usavel: de 1MB ate onde a RAM realmente vai */
    for (i = (1024u * 1024u) / PAGE_FRAME_SIZE; i < total_frames; i++) {
        clear_frame(i);
        free_frames++;
    }

    /* reserva de volta a area onde o kernel foi carregado (cap. 9) */
    kstart = (unsigned int) &kernel_physical_start;
    kend   = (unsigned int) &kernel_physical_end;
    mark_region_used(kstart, kend - kstart);

    /* reserva os modulos carregados pelo GRUB (ex: o "program" do cap. 7),
     * senao o allocator poderia entregar essa memoria pra outra coisa e
     * sobrescrever o modulo */
    if (mbinfo->flags & MULTIBOOT_FLAG_MODS) {
        multiboot_module_t *mods = (multiboot_module_t *) mbinfo->mods_addr;
        unsigned int m;
        for (m = 0; m < mbinfo->mods_count; m++) {
            mark_region_used(mods[m].mod_start, mods[m].mod_end - mods[m].mod_start);
        }
        /* a propria lista de modulos tambem ocupa memoria fisica */
        mark_region_used(mbinfo->mods_addr,
                          mbinfo->mods_count * sizeof(multiboot_module_t));
    }

    /* o primeiro 1MB (real mode area, BIOS, video etc) nunca foi liberado
     * acima, entao ja fica reservado por padrao - nao precisa fazer nada */

    next_free_hint = (1024u * 1024u) / PAGE_FRAME_SIZE;
}

unsigned int pfa_alloc_frame(void)
{
    unsigned int i;

    /* primeiro tenta a partir do "chute" (normalmente acha rapido e em
     * sequencia com o ultimo frame alocado) */
    for (i = next_free_hint; i < total_frames; i++) {
        if (!frame_is_set(i)) {
            set_frame(i);
            free_frames--;
            next_free_hint = i + 1;
            return i * PAGE_FRAME_SIZE;
        }
    }

    /* nao achou depois do hint - varre desde o comeco (pode haver frames
     * livres que foram devolvidos com pfa_free_frame antes do hint) */
    for (i = 0; i < next_free_hint && i < total_frames; i++) {
        if (!frame_is_set(i)) {
            set_frame(i);
            free_frames--;
            next_free_hint = i + 1;
            return i * PAGE_FRAME_SIZE;
        }
    }

    return 0; /* sem memoria */
}

void pfa_free_frame(unsigned int frame_addr)
{
    unsigned int frame_idx = frame_addr / PAGE_FRAME_SIZE;

    if (frame_idx >= total_frames) {
        return; /* endereco invalido, ignora */
    }

    if (frame_is_set(frame_idx)) {
        clear_frame(frame_idx);
        free_frames++;
        if (frame_idx < next_free_hint) {
            next_free_hint = frame_idx;
        }
    }
}

unsigned int pfa_free_frame_count(void)
{
    return free_frames;
}

unsigned int pfa_total_frame_count(void)
{
    return total_frames;
}
