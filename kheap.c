#include "kheap.h"
#include "pfa.h"

/* Cabecalho de cada bloco (livre ou alocado), formando uma lista circular
 * de blocos livres. "size" e medido em unidades de "Header" (incluindo o
 * proprio cabecalho), igual ao malloc classico do K&R. O union com Align
 * garante que o inicio de cada bloco fique alinhado do jeito que o pior
 * caso (ex: double) precisa. */
typedef long Align;

typedef union header {
    struct {
        union header *next;
        unsigned int  size;
    } s;
    Align x;
} Header;

static Header  base;          /* bloco vazio, so pra iniciar a lista */
static Header *freep = 0;     /* ultimo bloco liberado (comeco da busca) */

/* quantas unidades de Header cabem em um frame inteiro de 4KB */
#define NALLOC (PAGE_FRAME_SIZE / sizeof(Header))

/** morecore:
 *  Pede mais memoria pro page frame allocator (cap. 10.1/10.2) e devolve
 *  pro pool do kfree(). Como o kernel usa identity paging completa (ver
 *  paging.c, cap. 9), o endereco fisico do frame ja e diretamente
 *  utilizavel como ponteiro - nao precisamos mapear nada extra aqui,
 *  ao contrario do que a secao 10.2 do livro descreve pra um kernel que
 *  so mapeia paginas sob demanda.
 */
static Header *morecore(unsigned int nunits)
{
    unsigned int frame;
    Header *up;

    if (nunits > NALLOC - 1) {
        /* um unico frame nao caberia esse pedido - ver limitacao
         * documentada em kheap.h */
        return 0;
    }

    frame = pfa_alloc_frame();
    if (frame == 0) {
        return 0; /* sem memoria fisica disponivel */
    }

    up = (Header *) frame;
    up->s.size = NALLOC;
    kfree((void *) (up + 1));

    return freep;
}

void *kmalloc(unsigned int nbytes)
{
    Header *p, *prevp;
    unsigned int nunits;

    /* +1 unidade pro proprio cabecalho, arredondando pra cima */
    nunits = (nbytes + sizeof(Header) - 1) / sizeof(Header) + 1;

    if ((prevp = freep) == 0) {
        /* primeira chamada: lista vazia, "base" aponta pra si mesma */
        base.s.next = freep = prevp = &base;
        base.s.size = 0;
    }

    for (p = prevp->s.next; ; prevp = p, p = p->s.next) {
        if (p->s.size >= nunits) {
            if (p->s.size == nunits) {
                /* bloco do tamanho exato: tira ele da lista */
                prevp->s.next = p->s.next;
            } else {
                /* bloco maior: corta um pedaco do FIM dele */
                p->s.size -= nunits;
                p += p->s.size;
                p->s.size = nunits;
            }
            freep = prevp;
            return (void *) (p + 1);
        }

        if (p == freep) {
            /* deu a volta na lista inteira sem achar espaco - pede mais */
            if ((p = morecore(nunits)) == 0) {
                return 0; /* sem memoria */
            }
        }
    }
}

void kfree(void *ap)
{
    Header *bp, *p;

    if (ap == 0) {
        return;
    }

    bp = (Header *) ap - 1; /* volta pro cabecalho do bloco */

    /* acha o lugar certo na lista circular ordenada por endereco, pra
     * poder fundir blocos vizinhos e reduzir fragmentacao */
    for (p = freep; !(bp > p && bp < p->s.next); p = p->s.next) {
        if (p >= p->s.next && (bp > p || bp < p->s.next)) {
            break; /* bp fica no "fim"/"comeco" da lista (maior ou menor endereco de todos) */
        }
    }

    if (bp + bp->s.size == p->s.next) {
        /* funde com o bloco livre seguinte */
        bp->s.size += p->s.next->s.size;
        bp->s.next = p->s.next->s.next;
    } else {
        bp->s.next = p->s.next;
    }

    if (p + p->s.size == bp) {
        /* funde com o bloco livre anterior */
        p->s.size += bp->s.size;
        p->s.next = bp->s.next;
    } else {
        p->s.next = bp;
    }

    freep = p;
}
