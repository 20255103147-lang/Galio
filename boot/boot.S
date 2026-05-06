/* boot/boot.S — Multiboot v1 header + simple entry */
    .section .multiboot
    .align 4
    .long 0x1BADB002            /* magic */
    .long 0x00000003            /* flags: request memory info and boot device */
    .long -(0x1BADB002 + 0x00000003) /* checksum */

    .section .text
    .code32
    .global start
    .extern kmain

start:
    cli
    xor %ebp, %ebp
    mov $0x90000, %esp
    call kmain

.hang:
    hlt
    jmp .hang
