#include "irq.h"
#include "vga.h"
#include "common.h"

/* PIC ports */
#define PIC1_COMMAND 0x20
#define PIC1_DATA    0x21
#define PIC2_COMMAND 0xA0
#define PIC2_DATA    0xA1

static inline void outb(u16 port, u8 val) {
    __asm__ volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

static inline u8 inb(u16 port) {
    u8 ret;
    __asm__ volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

/* Remap PIC to avoid conflicts with CPU exceptions */
static void pic_remap(void) {
    u8 a1, a2;

    a1 = inb(PIC1_DATA);
    a2 = inb(PIC2_DATA);

    outb(PIC1_COMMAND, 0x11);
    outb(PIC2_COMMAND, 0x11);
    outb(PIC1_DATA, 0x20); /* Master offset 0x20 */
    outb(PIC2_DATA, 0x28); /* Slave offset 0x28 */
    outb(PIC1_DATA, 0x04);
    outb(PIC2_DATA, 0x02);
    outb(PIC1_DATA, 0x01);
    outb(PIC2_DATA, 0x01);

    outb(PIC1_DATA, a1);
    outb(PIC2_DATA, a2);
}

void irq_install(void) {
    pic_remap();
    vga_puts("IRQ/PIC remapped\n");
    /* Mask all IRQs for now */
    outb(PIC1_DATA, 0xFF);
    outb(PIC2_DATA, 0xFF);
}
