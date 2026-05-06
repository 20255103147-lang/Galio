#include "vga.h"
#include "common.h"

static volatile u16 *vga_buf = (u16*)0xB8000;
static u32 cursor_pos = 0;

static void update_cursor(void) {
    /* not implemented: hardware cursor optional */
    (void)cursor_pos;
}

void vga_clear(void) {
    for (u32 i = 0; i < 80*25; ++i) vga_buf[i] = (u16)(' ' | (0x07 << 8));
    cursor_pos = 0;
    update_cursor();
}

void vga_init(void) {
    vga_clear();
    vga_puts("Galio kernel booting...\n");
}

void vga_putch(char c) {
    if (c == '\n') {
        u32 col = cursor_pos % 80;
        cursor_pos += (80 - col);
    } else {
        vga_buf[cursor_pos++] = (u16)(c | (0x0F << 8));
    }
    if (cursor_pos >= 80*25) {
        /* simple scroll: move everything up one line */
        for (u32 row = 1; row < 25; ++row) {
            for (u32 col = 0; col < 80; ++col) {
                vga_buf[(row-1)*80 + col] = vga_buf[row*80 + col];
            }
        }
        /* clear last line */
        for (u32 col = 0; col < 80; ++col) vga_buf[(25-1)*80 + col] = (u16)(' ' | (0x07 << 8));
        cursor_pos = (25-1)*80;
    }
    update_cursor();
}

void vga_puts(const char *s) {
    while (*s) vga_putch(*s++);
}
