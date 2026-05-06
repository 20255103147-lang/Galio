#include "vga.h"
#include "gdt.h"
#include "idt.h"
#include "irq.h"

void kmain(void) {
    vga_init();
    vga_puts("Starting Galio kernel...\n");

    gdt_init();
    vga_puts("GDT initialized\n");

    idt_init();

    irq_install();

    vga_puts("Galio: kernel initialized.\n");

    /* Idle loop */
    for (;;) {
        __asm__ volatile("sti; hlt");
    }
}
