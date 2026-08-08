#ifndef _INCLUDE_KHEAP_H
#define _INCLUDE_KHEAP_H

/** kmalloc:
 *  Aloca um bloco de memoria do heap do kernel.
 *
 *  Limitacao atual: como o allocator de frames (pfa.c) ainda nao garante
 *  frames fisicos contiguos entre chamadas, cada bloco alocado por aqui
 *  cabe dentro de um unico frame de 4KB (menos o cabecalho). Pedidos
 *  maiores que isso retornam 0. Dividir um heap grande entre varios
 *  frames contiguos fica pra uma proxima iteracao.
 *
 *  @param  nbytes Quantidade de bytes desejada
 *  @return Ponteiro pro bloco alocado, ou 0 se nao foi possivel alocar
 */
void *kmalloc(unsigned int nbytes);

/** kfree:
 *  Devolve um bloco previamente alocado com kmalloc() pro heap, pra ser
 *  reaproveitado em alocacoes futuras.
 *
 *  @param ap Ponteiro retornado por kmalloc(). kfree(0) nao faz nada.
 */
void kfree(void *ap);

#endif
