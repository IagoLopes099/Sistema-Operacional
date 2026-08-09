#include "interrupt.h"
#include "pic.h"
#include "keyboard.h"
#include "timer.h"
#include "process.h"

#define TIMER_INTERRUPT    32   /* IRQ0 remapeada = 0x20 */
#define KEYBOARD_INTERRUPT 33   /* IRQ1 remapeada = 0x21 */

/** interrupt_handler:
 *  Chamada por todo handler gerado em isr.s. Decide o que fazer
 *  com base no número da interrupção recebida.
 */
void interrupt_handler(unsigned int interrupt)
{
    if (interrupt == TIMER_INTERRUPT) {
        timer_handler();
        process_account_tick();
    } else if (interrupt == KEYBOARD_INTERRUPT) {
        keyboard_handler();
    }

    pic_acknowledge(interrupt);
}