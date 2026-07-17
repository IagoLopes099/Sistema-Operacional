#include "io.h"
#include "pic.h"

#define PIC1_COMMAND_PORT 0x20
#define PIC1_DATA_PORT    0x21
#define PIC2_COMMAND_PORT 0xA0
#define PIC2_DATA_PORT    0xA1

#define PIC1_START_INTERRUPT 0x20
#define PIC2_START_INTERRUPT 0x28
#define PIC2_END_INTERRUPT   (PIC2_START_INTERRUPT + 7)

#define PIC_ACK   0x20
#define ICW1_INIT 0x10
#define ICW1_ICW4 0x01
#define ICW4_8086 0x01

/** pic_remap:
 *  Reprograma os dois PICs para que IRQs 0-15 caiam nos vetores
 *  0x20-0x2F, evitando colisão com as exceções 0-31 da CPU.
 */
void pic_remap(void)
{
    outb(PIC1_COMMAND_PORT, ICW1_INIT | ICW1_ICW4);
    outb(PIC2_COMMAND_PORT, ICW1_INIT | ICW1_ICW4);

    outb(PIC1_DATA_PORT, PIC1_START_INTERRUPT); /* PIC1 -> 0x20 */
    outb(PIC2_DATA_PORT, PIC2_START_INTERRUPT); /* PIC2 -> 0x28 */

    outb(PIC1_DATA_PORT, 0x04); /* avisa o PIC1 que tem PIC2 na linha IRQ2 */
    outb(PIC2_DATA_PORT, 0x02); /* identidade do PIC2 em cascata */

    outb(PIC1_DATA_PORT, ICW4_8086);
    outb(PIC2_DATA_PORT, ICW4_8086);

    /* máscaras: por enquanto só o teclado (IRQ1) fica habilitado */
    outb(PIC1_DATA_PORT, 0xFD); /* 1111 1101 */
    outb(PIC2_DATA_PORT, 0xFF); /* PIC2 todo desabilitado */
}

/** pic_acknowledge:
 *  Confirma pro(s) PIC(s) que a interrupção foi tratada.
 */
void pic_acknowledge(unsigned int interrupt)
{
    if (interrupt < PIC1_START_INTERRUPT || interrupt > PIC2_END_INTERRUPT) {
        return;
    }

    if (interrupt < PIC2_START_INTERRUPT) {
        outb(PIC1_COMMAND_PORT, PIC_ACK);
    } else {
        outb(PIC2_COMMAND_PORT, PIC_ACK);
        outb(PIC1_COMMAND_PORT, PIC_ACK); /* PICs em cascata: ambos precisam de ack */
    }
}