#include "vga.h"
#include "common.h"
#include "cpu.h"

#define VGA_WIDTH  80
#define VGA_HEIGHT 25
#define VGA_COLOR_WHITE 0x0F

static volatile u16 *vga_buf = (u16*)0xB8000;
static u32 cursor_x = 0;
static u32 cursor_y = 0;

void vga_update_cursor(void) {
    u16 pos = cursor_y * VGA_WIDTH + cursor_x;
    outb(0x3D4, 0x0F);
    outb(0x3D5, (u8)(pos & 0xFF));
    outb(0x3D4, 0x0E);
    outb(0x3D5, (u8)((pos >> 8) & 0xFF));
}

static void scroll(void) {
    /* Move all rows up by one */
    for (u32 y = 1; y < VGA_HEIGHT; y++) {
        for (u32 x = 0; x < VGA_WIDTH; x++) {
            vga_buf[(y-1) * VGA_WIDTH + x] = vga_buf[y * VGA_WIDTH + x];
        }
    }
    
    /* Clear last line */
    for (u32 x = 0; x < VGA_WIDTH; x++) {
        vga_buf[(VGA_HEIGHT-1) * VGA_WIDTH + x] = (u16)(' ' | (VGA_COLOR_WHITE << 8));
    }
    
    /* Adjust cursor position */
    if (cursor_y > 0) {
        cursor_y--;
    }
}

void vga_clear(void) {
    for (u32 i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++) {
        vga_buf[i] = (u16)(' ' | (VGA_COLOR_WHITE << 8));
    }
    cursor_x = 0;
    cursor_y = 0;
    vga_update_cursor();
}

void vga_move_cursor(int dx, int dy) {
    if (dx < 0) {
        u32 delta = (u32)(-dx);
        cursor_x = (cursor_x < delta) ? 0 : cursor_x - delta;
    } else {
        cursor_x += (u32)dx;
        if (cursor_x >= VGA_WIDTH) cursor_x = VGA_WIDTH - 1;
    }

    if (dy < 0) {
        u32 delta = (u32)(-dy);
        cursor_y = (cursor_y < delta) ? 0 : cursor_y - delta;
    } else {
        cursor_y += (u32)dy;
        if (cursor_y >= VGA_HEIGHT) cursor_y = VGA_HEIGHT - 1;
    }
    
    vga_update_cursor();
}

static void vga_putch_at(char c, u32 x, u32 y) {
    if (x < VGA_WIDTH && y < VGA_HEIGHT) {
        vga_buf[y * VGA_WIDTH + x] = (u16)(c | (VGA_COLOR_WHITE << 8));
    }
}

void vga_backspace(void) {
    if (cursor_x > 0) {
        cursor_x--;
        vga_putch_at(' ', cursor_x, cursor_y);
    } else if (cursor_y > 0) {
        cursor_y--;
        cursor_x = VGA_WIDTH - 1;
        vga_putch_at(' ', cursor_x, cursor_y);
    }
    vga_update_cursor();
}

void vga_newline(void) {
    cursor_x = 0;
    cursor_y++;
    
    if (cursor_y >= VGA_HEIGHT) {
        scroll();
        cursor_y = VGA_HEIGHT - 1;
    }
    vga_update_cursor();
}

void vga_init(void) {
    vga_clear();
}

void vga_putch(char c) {
    if (c == '\n') {
        vga_newline();
    } else if (c == '\t') {
        /* Tab = 4 spaces */
        for (int i = 0; i < 4; i++) {
            vga_putch(' ');
        }
    } else if (c == '\b') {
        vga_backspace();
    } else if (c >= 32 && c < 127) {
        /* Printable character */
        vga_putch_at(c, cursor_x, cursor_y);
        cursor_x++;
        
        if (cursor_x >= VGA_WIDTH) {
            vga_newline();
        }
        vga_update_cursor();
    }
}

void vga_puts(const char *s) {
    while (*s) {
        vga_putch(*s++);
    }
}