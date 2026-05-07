#ifndef IDT_H
#define IDT_H

#include "common.h"

/* IDT Entry structure */
typedef struct {
    u16 base_lo;      /* Lower 16 bits of handler address */
    u16 sel;          /* Kernel code segment selector */
    u8 always0;       /* Reserved, set to 0 */
    u8 flags;         /* Type, DPL, P bits */
    u16 base_hi;      /* Upper 16 bits of handler address */
} __attribute__((packed)) idt_entry_t;

struct idt_entry {
    u16 base_lo;
    u16 sel;
    u8 always0;
    u8 flags;
    u16 base_hi;
} __attribute__((packed));

/* IDT Pointer structure - passed to LIDT instruction */
typedef struct {
    u16 limit;        /* IDT size - 1 */
    u32 base;         /* Linear address of IDT */
} __attribute__((packed)) idt_ptr_t;

struct idt_ptr {
    u16 limit;
    u32 base;
} __attribute__((packed));

void idt_init(void);
void idt_set_gate(int n, u32 handler, u16 sel, u8 flags);

#endif /* IDT_H */
