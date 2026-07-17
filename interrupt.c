#include "interrupt.h"
#include "pic.h"
#include "keyboard.h"

#define KEYBOARD_INTERRUPT 33   /* IRQ1 remapeada = 0x21 */

/** interrupt_handler:
 *  Chamada por todo handler gerado em isr.s. Decide o que fazer
 *  com base no número da interrupção recebida.
 */
void interrupt_handler(unsigned int interrupt)
{
    if (interrupt == KEYBOARD_INTERRUPT) {
        keyboard_handler();
    }

    /* outras interrupções (exceções, timer, etc.) podem ser tratadas aqui
     * conforme você for implementando mais coisas */

    pic_acknowledge(interrupt);
}