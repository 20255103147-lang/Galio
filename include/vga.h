#ifndef VGA_H
#define VGA_H

void vga_init(void);
void vga_puts(const char *s);
void vga_putch(char c);
void vga_clear(void);
void vga_move_cursor(int dx, int dy);
void vga_scrollback_up(void);
void vga_scrollback_down(void);
void vga_show_live_screen(void);

void vga_update_cursor(void);
void vga_backspace(void);
void vga_newline(void);

/* Set current character color (VGA attribute byte).
 * High nibble = background color, low nibble = foreground color.
 * Common values:
 *   0x0A = light green on black
 *   0x0C = light red   on black
 *   0x0E = yellow      on black
 *   0x0F = white       on black  (default) */
void vga_set_color(unsigned char color);

#endif /* VGA_H */