#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "common.h"
#include "cpu.h"

/* PS/2 Keyboard driver */

typedef void (*key_callback_t)(u8 scancode, u8 is_pressed);

/* Initialize keyboard */
void keyboard_init(void);

/* Install key callback */
void keyboard_install_callback(key_callback_t callback);

/* Get scancode for ASCII character */
u8 scancode_to_ascii(u8 scancode);

#endif /* KEYBOARD_H */
