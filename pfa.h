#ifndef _INCLUDE_PFA_H
#define _INCLUDE_PFA_H

#include "multiboot.h"

#define PAGE_FRAME_SIZE 4096

/** pfa_init:
 *  Inicializa o page frame allocator a partir das informacoes que o GRUB
 *  passou na struct multiboot_info_t (quanta memoria existe, onde estao
 *  os modulos carregados etc). Depois disso, pfa_alloc_frame() ja pode
 *  ser chamada.
 *
 *  @param mbinfo A struct multiboot recebida em kmain (vinda do ebx)
 */
void pfa_init(multiboot_info_t *mbinfo);

/** pfa_alloc_frame:
 *  Reserva um frame fisico de 4KB livre e o marca como usado.
 *
 *  @return O endereco fisico do frame reservado, ou 0 se nao houver
 *          memoria livre (0 nunca e um frame valido de verdade, pois e
 *          sempre reservado pelo kernel/BIOS).
 */
unsigned int pfa_alloc_frame(void);

/** pfa_free_frame:
 *  Devolve um frame fisico previamente reservado com pfa_alloc_frame().
 *
 *  @param frame_addr O endereco fisico do frame (deve ser alinhado em 4KB)
 */
void pfa_free_frame(unsigned int frame_addr);

/** pfa_free_frame_count:
 *  @return Quantos frames ainda estao livres.
 */
unsigned int pfa_free_frame_count(void);

/** pfa_total_frame_count:
 *  @return O total de frames que o allocator conhece (livres + usados).
 */
unsigned int pfa_total_frame_count(void);

#endif
