#include "fb.h"
#include "gdt.h"
#include "idt.h"
#include "pic.h"
#include "timer.h"
#include "keyboard.h"
#include "kheap.h"
#include "pfa.h"
#include "process.h"
#include "shell.h"
#include "multiboot.h"

void kmain(multiboot_info_t *mbinfo) {
    char *fb = (char *)0x000B8000;
    unsigned int i;
    int shell_pid;

    for (i = 0; i < 80 * 25 * 2; i += 2) {
        fb[i] = ' ';
        fb[i + 1] = 0x0F;
    }
    fb_move_cursor(0);

    gdt_install();
    idt_install();
    pic_remap();

    /* The process stacks are allocated by kmalloc(), which uses the PFA. */
    pfa_init(mbinfo);

    process_init();
    shell_init();

    shell_pid = process_create("shell", shell_task, 0);
    if (shell_pid < 0) {
        fb_write("Falha ao criar shell.\n", 22);
        for (;;) {}
    }

    /* Make the shell the initial running process and enter it through the
       same cooperative context-switch path used by every child. */
    process_start(shell_pid);

    /* PIT precisa estar configurado antes de habilitar as IRQs. */
    timer_init();
    __asm__ __volatile__("sti");

    /* Bootstrap the first process by switching from the kernel stack to
       its prepared process stack. */
    {
        u32int dummy_esp = 0;
        process_t *table = process_get_table();
        for (i = 0; i < MAX_PROCESSES; i++) {
            if ((int)table[i].id == shell_pid) {
                extern void process_switch(u32int *, u32int);
                process_switch(&dummy_esp, table[i].esp);
                break;
            }
        }
    }

    for (;;) {}
}
