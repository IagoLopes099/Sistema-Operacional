#include "timer.h"
#include "io.h"

#define PIT_COMMAND_PORT 0x43
#define PIT_CHANNEL0_PORT 0x40
#define PIT_BASE_FREQUENCY 1193180u
#define PIT_FREQUENCY 100u

static volatile u32int ticks = 0;

void timer_init(void)
{
    u16int divisor = (u16int)(PIT_BASE_FREQUENCY / PIT_FREQUENCY);

    /* Canal 0, acesso low+high byte, modo 2 (rate generator), binário. */
    outb(PIT_COMMAND_PORT, 0x34);
    outb(PIT_CHANNEL0_PORT, (u8int)(divisor & 0xFF));
    outb(PIT_CHANNEL0_PORT, (u8int)((divisor >> 8) & 0xFF));
}

void timer_handler(void)
{
    ticks++;
}

u32int timer_get_ticks(void)
{
    return ticks;
}
