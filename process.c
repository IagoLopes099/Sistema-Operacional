#include "process.h"
#include "fb.h"
#include "kheap.h"

#define PROCESS_STACK_SIZE 4080

extern void process_switch(u32int *old_esp, u32int new_esp);

static process_t process_table[MAX_PROCESSES];
static int current_index = -1;
static u32int next_pid = 1;

static unsigned int strlen(const char *s) {
    unsigned int n = 0;
    while (s[n]) n++;
    return n;
}

static void str_copy(char *dst, const char *src, u32int max) {
    u32int i = 0;
    if (!max) return;
    while (src[i] && i < max - 1) {
        dst[i] = src[i];
        i++;
    }
    dst[i] = '\0';
}

static void fb_write_num(u32int n) {
    char buf[11];
    int i = 10;
    buf[i] = '\0';
    if (!n) {
        fb_write("0", 1);
        return;
    }
    while (n && i) {
        buf[--i] = (char)('0' + n % 10);
        n /= 10;
    }
    fb_write(&buf[i], strlen(&buf[i]));
}

static int find_slot(void) {
    int i;
    for (i = 0; i < MAX_PROCESSES; i++)
        if (process_table[i].state == PROCESS_UNUSED)
            return i;
    return -1;
}

static int find_index_by_pid(u32int pid) {
    int i;
    for (i = 0; i < MAX_PROCESSES; i++)
        if (process_table[i].state != PROCESS_UNUSED &&
            process_table[i].id == pid)
            return i;
    return -1;
}

static int next_ready_index(int from) {
    int n, idx;
    if (from < 0) from = 0;
    for (n = 1; n <= MAX_PROCESSES; n++) {
        idx = (from + n) % MAX_PROCESSES;
        if (process_table[idx].state == PROCESS_READY)
            return idx;
    }
    return -1;
}

static void process_bootstrap(void (*entry)(void)) {
    entry();
    process_exit(0);
    for (;;) {}
}

void process_init(void) {
    int i;
    for (i = 0; i < MAX_PROCESSES; i++) {
        process_table[i].id = 0;
        process_table[i].parent_id = 0;
        process_table[i].state = PROCESS_UNUSED;
        process_table[i].esp = 0;
        process_table[i].stack_base = 0;
        process_table[i].priority = 1;
        process_table[i].cpu_time = 0;
        process_table[i].exit_status = 0;
    }
    current_index = -1;
    next_pid = 1;
}

int process_create(const char *name, void (*entry_point)(void), u32int parent_id) {
    int slot = find_slot();
    if (slot < 0 || !entry_point) return -1;

    u32int stack = (u32int)kmalloc(PROCESS_STACK_SIZE);
    if (!stack) return -1;

    /* Initial stack expected by process_switch:
       [esp+0] edi, [esp+4] esi, [esp+8] ebx, [esp+12] ebp,
       [esp+16] return address, [esp+20] first C argument. */
    u32int *sp = (u32int *)(stack + PROCESS_STACK_SIZE);
    *(--sp) = (u32int)entry_point;       /* argument to process_bootstrap */
    *(--sp) = 0;                          /* fake return address */
    *(--sp) = (u32int)process_bootstrap;  /* return address */
    *(--sp) = 0;                          /* ebp */
    *(--sp) = 0;                          /* ebx */
    *(--sp) = 0;                          /* esi */
    *(--sp) = 0;                          /* edi */

    process_table[slot].id = next_pid++;
    process_table[slot].parent_id = parent_id;
    str_copy(process_table[slot].name, name, PROCESS_NAME_LEN);
    process_table[slot].state = PROCESS_READY;
    process_table[slot].esp = (u32int)sp;
    process_table[slot].stack_base = stack;
    process_table[slot].priority = 1;
    process_table[slot].cpu_time = 0;
    process_table[slot].exit_status = 0;

    return (int)process_table[slot].id;
}

void process_start(int pid) {
    int idx = find_index_by_pid((u32int)pid);

    if (idx < 0)
        return;

    current_index = idx;
    process_table[idx].state = PROCESS_RUNNING;
}

int process_current_pid(void) {
    if (current_index < 0) return 0;
    return (int)process_table[current_index].id;
}

u32int process_current_parent(void) {
    if (current_index < 0) return 0;
    return process_table[current_index].parent_id;
}

void process_account_tick(void) {
    if (current_index >= 0 &&
        process_table[current_index].state == PROCESS_RUNNING) {
        process_table[current_index].cpu_time++;
    }
}

void process_yield(void) {
    int next;
    int old = current_index;

    if (old < 0) return;

    /*
     * O contexto salvo por process_switch() não pode ser interrompido pelo
     * PIT. Se uma IRQ ocorrer depois que ESP já foi trocado para o processo
     * destino, a CPU colocará o frame da interrupção na pilha do destino
     * antes dos POPs do process_switch(), corrompendo a pilha e causando
     * travamentos aparentemente aleatórios.
     */
    __asm__ __volatile__("cli");

    next = next_ready_index(old);
    if (next < 0) {
        __asm__ __volatile__("sti");
        return;
    }

    process_table[old].state = PROCESS_READY;
    process_table[next].state = PROCESS_RUNNING;
    current_index = next;

    /*
     * O processo destino retorna aqui quando seu contexto salvo for
     * restaurado. Assim, o STI abaixo será executado no contexto correto.
     */
    process_switch(&process_table[old].esp, process_table[next].esp);

    __asm__ __volatile__("sti");
}

void process_exit(int status) {
    int old, next;

    if (current_index < 0) {
        for (;;) {}
    }

    old = current_index;

    /* A mudança RUNNING -> ZOMBIE e a troca de ESP também precisam ser
       indivisíveis em relação ao PIT. */
    __asm__ __volatile__("cli");

    process_table[old].exit_status = status;
    process_table[old].state = PROCESS_ZOMBIE;

    next = next_ready_index(old);
    if (next < 0) {
        /* No other process can run. Keep the zombie visible instead of
           returning through a dead process stack. */
        for (;;) {}
    }

    process_table[next].state = PROCESS_RUNNING;
    process_table[next].cpu_time++;
    current_index = next;

    process_switch(&process_table[old].esp, process_table[next].esp);
    for (;;) {}
}

int process_reap(u32int pid) {
    int idx = find_index_by_pid(pid);
    if (idx < 0 || process_table[idx].state != PROCESS_ZOMBIE)
        return -1;

    if (process_table[idx].stack_base)
        kfree((void *)process_table[idx].stack_base);

    process_table[idx].id = 0;
    process_table[idx].parent_id = 0;
    process_table[idx].state = PROCESS_UNUSED;
    process_table[idx].esp = 0;
    process_table[idx].stack_base = 0;
    return 0;
}

process_t *process_get_table(void) {
    return process_table;
}

void process_list(void) {
    int i;

    fb_write("PID  PPID STATE      NAME                 TICKS\n",
             strlen("PID  PPID STATE      NAME                 TICKS\n"));

    for (i = 0; i < MAX_PROCESSES; i++) {
        process_t *p = &process_table[i];

        if (p->state == PROCESS_UNUSED)
            continue;

        fb_write_num(p->id);
        fb_write("    ", 4);

        fb_write_num(p->parent_id);
        fb_write("    ", 4);

        switch (p->state) {
            case PROCESS_READY:   fb_write("READY     ", 10); break;
            case PROCESS_RUNNING: fb_write("RUNNING   ", 10); break;
            case PROCESS_BLOCKED:  fb_write("BLOCKED   ", 10); break;
            case PROCESS_ZOMBIE:  fb_write("ZOMBIE    ", 10); break;
            default:              fb_write("UNKNOWN   ", 10); break;
        }

        fb_write(p->name, strlen(p->name));

        /* Preenche a coluna NAME até 20 caracteres. */
        {
            unsigned int name_len = strlen(p->name);
            while (name_len++ < 20)
                fb_write(" ", 1);
        }

        fb_write_num(p->cpu_time);
        fb_write("\n", 1);
    }
}

