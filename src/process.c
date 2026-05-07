/* process.c - Process management and scheduling */
#include "process.h"
#include "heap.h"
#include "paging.h"
#include "kprintf.h"

static process_t processes[MAX_PROCESSES];
static u32 next_pid = 1;
static process_t *current_process = NULL;
static u32 process_count = 0;

extern void process_switch_asm(register_state_t *old_regs, register_state_t *new_regs);

void process_init(void) {
    kprintf("Process manager initialized\n");
    
    /* Mark all processes as invalid */
    for (u32 i = 0; i < MAX_PROCESSES; i++) {
        processes[i].pid = 0;
        processes[i].state = PROCESS_ZOMBIE;
    }

    /* Create idle process */
    process_create(NULL, 0);
    current_process = &processes[0];
    kprintf("Idle process created (PID 1)\n");
}

u32 process_create(void (*entry)(void), u32 priority) {
    if (process_count >= MAX_PROCESSES) {
        kprintf("process_create: Too many processes\n");
        return 0;
    }

    process_t *proc = NULL;
    for (u32 i = 0; i < MAX_PROCESSES; i++) {
        if (processes[i].pid == 0) {
            proc = &processes[i];
            break;
        }
    }

    if (!proc) {
        kprintf("process_create: No free process slots\n");
        return 0;
    }

    proc->pid = next_pid++;
    proc->parent_pid = current_process ? current_process->pid : 0;
    proc->state = PROCESS_READY;
    proc->priority = priority;
    proc->ticks = 0;

    /* Allocate kernel stack */
    proc->stack = (u32 *)kmalloc(PROCESS_STACK_SIZE);
    proc->stack_size = PROCESS_STACK_SIZE;

    if (!proc->stack) {
        kprintf("process_create: Failed to allocate stack\n");
        return 0;
    }

    /* Initialize stack and registers */
    u32 stack_top = (u32)proc->stack + PROCESS_STACK_SIZE - 4;
    
    proc->regs.esp = stack_top;
    proc->regs.ebp = stack_top;
    proc->regs.eip = (u32)entry;
    proc->regs.eflags = 0x202;  /* IF flag set */
    proc->regs.eax = 0;
    proc->regs.ebx = 0;
    proc->regs.ecx = 0;
    proc->regs.edx = 0;
    proc->regs.esi = 0;
    proc->regs.edi = 0;

    process_count++;
    kprintf("Process created: PID=%u, priority=%u\n", proc->pid, priority);

    return proc->pid;
}

process_t *process_current(void) {
    return current_process;
}

void process_yield(void) {
    /* Find next ready process */
    process_t *next = NULL;
    u32 start = (current_process - processes + 1) % MAX_PROCESSES;

    for (u32 i = 0; i < MAX_PROCESSES; i++) {
        u32 idx = (start + i) % MAX_PROCESSES;
        if (processes[idx].pid != 0 && processes[idx].state == PROCESS_READY) {
            next = &processes[idx];
            break;
        }
    }

    if (!next) {
        next = current_process;  /* Run same process */
    }

    if (next != current_process) {
        process_t *old = current_process;
        current_process = next;
        next->state = PROCESS_RUNNING;
        old->state = PROCESS_READY;
        process_switch(old, next);
    }
}

void process_switch(process_t *from, process_t *to) {
    (void)from;
    (void)to;
    /* Assembly function will handle actual context switch */
    /* process_switch_asm(&from->regs, &to->regs); */
}

void process_exit(i32 code) {
    (void)code;
    current_process->state = PROCESS_ZOMBIE;
    process_count--;
    process_yield();
}

process_t *process_get(u32 pid) {
    for (u32 i = 0; i < MAX_PROCESSES; i++) {
        if (processes[i].pid == pid && processes[i].state != PROCESS_ZOMBIE) {
            return &processes[i];
        }
    }
    return NULL;
}
