/* syscall.c - System calls interface */
#include "syscall.h"
#include "cpu.h"
#include "vga.h"
#include "kprintf.h"
#include "process.h"

/* Syscall handler - called from INT 0x80 */
static void syscall_handler(registers_t *regs) {
    /* For INT 0x80, we need to distinguish syscall number from interrupt_number */
    /* The actual syscall number is in EAX */
    u32 syscall_num = regs->eax;
    u32 arg1 = regs->ebx;
    u32 arg2 = regs->ecx;
    u32 arg3 = regs->edx;

    switch (syscall_num) {
        case SYS_EXIT:
            kprintf("Process exit with code %d\n", arg1);
            process_exit((i32)arg1);
            break;
            
        case SYS_WRITE:
            if (arg1 == 1) {  /* stdout */
                for (u32 i = 0; i < arg3; i++) {
                    vga_putch(((char *)arg2)[i]);
                }
            }
            regs->eax = arg3;
            break;

        case SYS_GETPID: {
            process_t *proc = process_current();
            regs->eax = proc ? proc->pid : 0;
            break;
        }

        case SYS_FORK:
            kprintf("SYS_FORK not yet implemented\n");
            regs->eax = -1;
            break;

        case SYS_EXEC:
            kprintf("SYS_EXEC not yet implemented\n");
            regs->eax = -1;
            break;

        case SYS_SLEEP:
            kprintf("SYS_SLEEP not yet implemented\n");
            regs->eax = 0;
            break;

        default:
            kprintf("Unknown syscall: %u\n", syscall_num);
            regs->eax = -1;
            break;
    }
}

void syscall_init(void) {
    /* Register INT 0x80 as syscall handler */
    interrupt_install_handler(0x80, syscall_handler);
    kprintf("Syscall interface initialized\n");
}
