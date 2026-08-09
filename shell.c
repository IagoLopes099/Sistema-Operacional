#include "shell.h"
#include "fb.h"
#include "keyboard.h"
#include "process.h"
#include "timer.h"

#define BUFFER_LEN 128

static char input_buf[BUFFER_LEN];
static unsigned int buf_pos;

static unsigned int strlen(const char *s) {
    unsigned int n = 0;
    while (s[n]) n++;
    return n;
}

static int streq(const char *a, const char *b) {
    unsigned int i = 0;
    while (a[i] && b[i]) {
        if (a[i] != b[i]) return 0;
        i++;
    }
    return a[i] == b[i];
}

static void write_num(u32int n) {
    char b[11];
    int i = 10;
    b[i] = 0;
    if (!n) { fb_write("0", 1); return; }
    while (n) {
        b[--i] = (char)('0' + n % 10);
        n /= 10;
    }
    fb_write(&b[i], strlen(&b[i]));
}

static void clear_screen(void) {
    unsigned int i;
    char *fb = (char *)0xB8000;
    for (i = 0; i < 80 * 25; i++) {
        fb[i * 2] = ' ';
        fb[i * 2 + 1] = 0x0F;
    }
    fb_move_cursor(0);
}

/* Carga cooperativa que permanece viva por cerca de 2 segundos.
   Isso torna visível o comportamento de vários jobs em background sem
   depender de um atraso ocupado (busy wait). */
static void worker_task(void) {
    u32int start_tick = timer_get_ticks();

    while ((timer_get_ticks() - start_tick) < 500) {
        process_yield();
    }
}

static void top_task(void) {
    u32int last_refresh = 0;

    for (;;) {
        if (keyboard_has_char()) {
            char c = (char)keyboard_read();
            if (c == 'q' || c == 'Q')
                return;
        }

        /* Atualiza 5 vezes por segundo, independente da velocidade da CPU. */
        if ((timer_get_ticks() - last_refresh) >= 20) {
            last_refresh = timer_get_ticks();

            clear_screen();

            fb_write(
                "MYOS2 TOP - cooperative process monitor\n\n",
                strlen("MYOS2 TOP - cooperative process monitor\n\n")
            );

            process_list();

            fb_write(
                "\nPressione q para sair.\n",
                strlen("\nPressione q para sair.\n")
            );
        }

        process_yield();
    }
}


static void reap_children(void) {
    process_t *t = process_get_table();
    int i;
    u32int me = (u32int)process_current_pid();

    for (i = 0; i < MAX_PROCESSES; i++) {
        if (t[i].state == PROCESS_ZOMBIE && t[i].parent_id == me) {
            process_reap(t[i].id);
        }
    }
}

static int process_state_by_pid(u32int pid, process_state_t *state) {
    process_t *t = process_get_table();
    int i;
    for (i = 0; i < MAX_PROCESSES; i++) {
        if (t[i].state != PROCESS_UNUSED && t[i].id == pid) {
            *state = t[i].state;
            return 1;
        }
    }
    return 0;
}

static void wait_for_child(u32int pid) {
    process_state_t state;
    while (process_state_by_pid(pid, &state) &&
           state != PROCESS_ZOMBIE) {
        process_yield();
    }
    process_reap(pid);
}

static void execute_command(char *cmd) {
    unsigned int len = strlen(cmd);
    int background = 0;
    int pid;

    if (len && cmd[len - 1] == '&') {
        background = 1;
        cmd[--len] = '\0';
        while (len && cmd[len - 1] == ' ') cmd[--len] = '\0';
    }

    if (streq(cmd, "help")) {
        const char *msg =
            "\nComandos:\n"
            "  help       mostra esta ajuda\n"
            "  ps         lista processos\n"
            "  top        monitor em tempo real\n"
            "  worker     cria uma tarefa de teste\n"
            "  exit       encerra o shell\n"
            "Use '&' para background: worker &\n";
        fb_write((char *)msg, strlen(msg));
    } else if (streq(cmd, "ps")) {
        fb_write("\n", 1);
        process_list();
    } else if (streq(cmd, "worker")) {
        pid = process_create("worker", worker_task,
                             (u32int)process_current_pid());
        if (pid < 0) {
            const char *msg = "\nErro: tabela cheia ou memoria insuficiente.\n";
            fb_write((char *)msg, strlen(msg));
        } else {
            fb_write("\n[", 2);
            write_num((u32int)pid);
            fb_write("] iniciado", 10);
            if (background)
                fb_write(" em background", 14);
            fb_write("\n", 1);

            if (!background)
                wait_for_child((u32int)pid);
        }
    } else if (streq(cmd, "top")) {
        pid = process_create("top", top_task,
                             (u32int)process_current_pid());
        if (pid < 0) {
            const char *msg = "\nNao foi possivel iniciar top.\n";
            fb_write((char *)msg, strlen(msg));
        } else {
            wait_for_child((u32int)pid);
        }
    } else if (streq(cmd, "exit")) {
        process_exit(0);
    } else if (cmd[0]) {
        const char *msg = "\nComando desconhecido. Digite help.\n";
        fb_write((char *)msg, strlen(msg));
    }

    reap_children();
}

void shell_init(void) {
    buf_pos = 0;
}

void shell_task(void) {
    fb_write("MyOS2 Mini-Shell\nDigite 'help' para ajuda.\n\nMyOS2> ", strlen("MyOS2 Mini-Shell\nDigite 'help' para ajuda.\n\nMyOS2> "));

    for (;;) {
        if (keyboard_has_char()) {
            char c = (char)keyboard_read();

            if (c == '\n') {
                input_buf[buf_pos] = '\0';
                execute_command(input_buf);
                buf_pos = 0;
                fb_write("\nMyOS2> ", strlen("\nMyOS2> "));
            } else if (c == '\b') {
                if (buf_pos) {
                    buf_pos--;
                    fb_write("\b \b", 3);
                }
            } else if (c >= 32 && buf_pos < BUFFER_LEN - 1) {
                input_buf[buf_pos++] = c;
                fb_write(&c, 1);
            }
        }
        process_yield();
    }
}
