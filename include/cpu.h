#ifndef CPU_H
#define CPU_H

#include "common.h"

/* CPU registers and interrupt context */
typedef struct {
    u32 ds;
    u32 edi, esi, ebp, esp, ebx, edx, ecx, eax;
    u32 interrupt_number;
    u32 error_code;
    u32 eip, cs, eflags, user_esp, user_ss;
} registers_t;

/* ISR/IRQ handler function pointer */
typedef void (*interrupt_handler_t)(registers_t *regs);

/* Install handler for interrupt number */
void interrupt_install_handler(u32 n, interrupt_handler_t handler);

/* Interrupt handlers - declared in C */
void isr_handler(registers_t *regs);
void irq_handler(registers_t *regs);

/* Common CPU utilities */
static inline void enable_interrupts(void) {
    __asm__ volatile("sti");
}

static inline void disable_interrupts(void) {
    __asm__ volatile("cli");
}

static inline void halt(void) {
    __asm__ volatile("hlt");
}

/* I/O port operations */
static inline u8 inb(u16 port) {
    u8 val;
    __asm__ volatile("inb %1, %0" : "=a"(val) : "Nd"(port));
    return val;
}

static inline void outb(u16 port, u8 val) {
    __asm__ volatile("outb %0, %1" : : "a"(val), "Nd"(port));
}

static inline u16 inw(u16 port) {
    u16 val;
    __asm__ volatile("inw %1, %0" : "=a"(val) : "Nd"(port));
    return val;
}

static inline void outw(u16 port, u16 val) {
    __asm__ volatile("outw %0, %1" : : "a"(val), "Nd"(port));
}

static inline u32 inl(u16 port) {
    u32 val;
    __asm__ volatile("inl %1, %0" : "=a"(val) : "Nd"(port));
    return val;
}

static inline void outl(u16 port, u32 val) {
    __asm__ volatile("outl %0, %1" : : "a"(val), "Nd"(port));
}

#endif /* CPU_H */
