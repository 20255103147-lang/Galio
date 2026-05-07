#ifndef SYSCALL_H
#define SYSCALL_H

#include "common.h"
#include "cpu.h"

/* Syscall numbers */
#define SYS_EXIT     1
#define SYS_WRITE    2
#define SYS_GETPID   3
#define SYS_FORK     4
#define SYS_EXEC     5
#define SYS_SLEEP    6

/* Initialize syscall interface (register INT 0x80 handler) */
void syscall_init(void);

#endif /* SYSCALL_H */
