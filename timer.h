#ifndef INCLUDE_TIMER_H
#define INCLUDE_TIMER_H

#include "types.h"

/* Inicializa o PIT em 100 Hz (um tick a cada 10 ms). */
void timer_init(void);

/* Retorna o número de ticks desde a inicialização do timer. */
u32int timer_get_ticks(void);

/* Chamado pelo handler da IRQ0. */
void timer_handler(void);

#endif
