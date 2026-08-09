#ifndef INCLUDE_PROCESS_H
#define INCLUDE_PROCESS_H

#include "types.h"

#define MAX_PROCESSES 16
#define PROCESS_NAME_LEN 32

typedef enum {
    PROCESS_UNUSED = 0,
    PROCESS_READY,
    PROCESS_RUNNING,
    PROCESS_BLOCKED,
    PROCESS_ZOMBIE
} process_state_t;

typedef struct process {
    u32int id;
    u32int parent_id;
    char name[PROCESS_NAME_LEN];
    process_state_t state;
    u32int esp;
    u32int stack_base;
    u32int priority;
    u32int cpu_time;
    int exit_status;
} process_t;

void process_init(void);
int process_create(const char *name, void (*entry_point)(void), u32int parent_id);
void process_start(int pid);
void process_yield(void);
void process_account_tick(void);
void process_exit(int status);
int process_reap(u32int pid);
int process_current_pid(void);
u32int process_current_parent(void);
process_t *process_get_table(void);
void process_list(void);

#endif
