#ifndef _INCLUDE_INTERRUPT_H_
#define _INCLUDE_INTERRUPT_H_

void interrupt_handler(unsigned int interrupt);
void enable_interrupts(void);   /* definida em isr.s */

#endif