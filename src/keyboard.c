/* keyboard.c - PS/2 Keyboard driver */
#include "keyboard.h"
#include "irq.h"
#include "kprintf.h"
#include "vga.h"
#include <stddef.h>

#define KEYBOARD_DATA 0x60
#define KEYBOARD_CTRL 0x64

#define LSHIFT_PRESSED  0x2A
#define RSHIFT_PRESSED  0x36
#define LCTRL_PRESSED   0x1D
#define LALT_PRESSED    0x38

static u8 shift_pressed = 0;
static u8 ctrl_pressed = 0;
static u8 alt_pressed = 0;
static key_callback_t user_callback = NULL;

/* Scancode to ASCII lookup table (without shift) */
static const u8 scancode_table[] = {
    0,  27,  '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b', '\t',
    'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n', 0, 'a', 's',
    'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0, '\\', 'z', 'x', 'c', 'v',
    'b', 'n', 'm', ',', '.', '/', 0, '*', 0, ' ', 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, '7', '8', '9', '-', '4', '5', '6', '+', '1',
    '2', '3', '0', '.', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

/* Scancode to ASCII lookup table (with shift) */
static const u8 scancode_table_shift[] = {
    0,  27,  '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b', '\t',
    'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n', 0, 'A', 'S',
    'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~', 0, '|', 'Z', 'X', 'C', 'V',
    'B', 'N', 'M', '<', '>', '?', 0, '*', 0, ' ', 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, '7', '8', '9', '-', '4', '5', '6', '+', '1',
    '2', '3', '0', '.', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

static void keyboard_handler(registers_t *regs) {
    (void)regs;
    u8 scancode = inb(KEYBOARD_DATA);
    u8 is_pressed = !(scancode & 0x80);
    scancode &= 0x7F;

    /* Track modifier keys */
    if (scancode == LSHIFT_PRESSED || scancode == RSHIFT_PRESSED) {
        shift_pressed = is_pressed;
    } else if (scancode == LCTRL_PRESSED) {
        ctrl_pressed = is_pressed;
    } else if (scancode == LALT_PRESSED) {
        alt_pressed = is_pressed;
    }

    if (user_callback) {
        user_callback(scancode, is_pressed);
    }

    /* Echo to screen if key pressed */
    if (is_pressed && scancode < 60) {
        u8 ascii = shift_pressed ? scancode_table_shift[scancode] : scancode_table[scancode];
        if (ascii > 0) {
            vga_putch(ascii);
        }
    }
}

void keyboard_init(void) {
    /* Install keyboard handler on IRQ1 */
    interrupt_install_handler(33, keyboard_handler);
    irq_unmask(1);
    kprintf("Keyboard initialized\n");
}

void keyboard_install_callback(key_callback_t callback) {
    user_callback = callback;
}

u8 scancode_to_ascii(u8 scancode) {
    if (scancode < 60) {
        return shift_pressed ? scancode_table_shift[scancode] : scancode_table[scancode];
    }
    return 0;
}
