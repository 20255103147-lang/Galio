#ifndef PROCESS_H
#define PROCESS_H

#include "common.h"

/* Process/Task abstraction */

#define MAX_PROCESSES 128
#define PROCESS_STACK_SIZE 8192
#define PROCESS_MAX_FDS 16
#define PROCESS_MAX_MMAPS 16

#define USER_HEAP_START 0x40000000
#define USER_HEAP_END   0x50000000
#define USER_STACK_TOP  0xBFFFE000
#define USER_STACK_SIZE 0x10000
typedef enum {
    PROCESS_READY,
    PROCESS_RUNNING,
    PROCESS_WAITING,
    PROCESS_ZOMBIE
} process_state_t;

typedef struct {
    u32 esp;
    u32 ebp;
    u32 esi;
    u32 edi;
    u32 ebx;
    u32 edx;
    u32 ecx;
    u32 eax;
    u32 eflags;
    u32 eip;  /* For initial setup */
} register_state_t;

typedef struct {
    u32 start;
    u32 length;
    u32 prot;
    u32 flags;
    u32 fd;
    u32 offset;
    u8 anonymous;
} mmap_region_t;

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
    u32 fd_table[PROCESS_MAX_FDS];  /* File descriptor table per-process */
    u32 heap_start;
    u32 brk;
    u32 mmap_count;
    mmap_region_t mmap_regions[PROCESS_MAX_MMAPS];
} process_t;

/* Initialize process manager */
void process_init(void);

/* Create a new process */
u32 process_create(void (*entry)(void), u32 priority);

/* Set the current process to the boot context for initial scheduling */
 void process_set_boot_current(void);
process_t *process_current(void);

/* Yield to next process */
void process_yield(void);

/* Switch to another process (called by scheduler) */
void process_switch(process_t *from, process_t *to);

/* Terminate current process */
void process_exit(i32 code);

/* Get process by PID */
process_t *process_get(u32 pid);

/* Kill a process when memory pressure is too high */
void process_oom_kill(void);

#endif /* PROCESS_H */
