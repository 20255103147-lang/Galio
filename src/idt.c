#include "idt.h"
#include "common.h"
#include "vga.h"

struct idt_entry {
    u16 base_lo;
    u16 sel;
    u8  always0;
    u8  flags;
    u16 base_hi;
} __attribute__((packed));

struct idt_ptr {
    u16 limit;
    u32 base;
} __attribute__((packed));

static struct idt_entry idt[256];
static struct idt_ptr idtp;

extern void idt_load(u32);

/* helper to set an IDT gate */
static void idt_set_gate(int n, u32 handler, u16 sel, u8 flags) {
    idt[n].base_lo = handler & 0xFFFF;
    idt[n].base_hi = (handler >> 16) & 0xFFFF;
    idt[n].sel = sel;
    idt[n].always0 = 0;
    idt[n].flags = flags;
}

void idt_init(void) {
    idtp.limit = (sizeof(struct idt_entry) * 256) - 1;
    idtp.base = (u32)&idt;

    /* Clear IDT */
    for (int i = 0; i < 256; ++i) {
        idt_set_gate(i, 0, 0x08, 0x8E);
    }

    /* Load IDT */
    idt_load((u32)&idtp);

    vga_puts("IDT loaded\n");
}
