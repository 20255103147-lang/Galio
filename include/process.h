#ifndef PROCESS_H
#define PROCESS_H

#include "common.h"

/* Process/Task abstraction */

#define MAX_PROCESSES 128
#define PROCESS_STACK_SIZE 8192

typedef enum {
    PROCESS_READY,
    PROCESS_RUNNING,
    PROCESS_WAITING,
    PROCESS_ZOMBIE
} process_state_t;

typedef struct {
    u32 esp;
    u32 ebp;
    u32 eip;
    u32 eax, ebx, ecx, edx, esi, edi;
    u32 eflags;
} register_state_t;

typedef struct {
    u32 pid;
    u32 parent_pid;
    process_state_t state;
    register_state_t regs;
    u32 *stack;
    u32 stack_size;
    void *pagedir;
    u32 priority;
    u32 ticks;
} process_t;

/* Initialize process manager */
void process_init(void);

/* Create a new process */
u32 process_create(void (*entry)(void), u32 priority);

/* Get current process */
process_t *process_current(void);

/* Yield to next process */
void process_yield(void);

/* Switch to another process (called by scheduler) */
void process_switch(process_t *from, process_t *to);

/* Terminate current process */
void process_exit(i32 code);

/* Get process by PID */
process_t *process_get(u32 pid);

#endif /* PROCESS_H */
