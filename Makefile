# Simple build for a 32-bit freestanding kernel
CC = gcc
AS = nasm
LD = ld

CFLAGS = -m32 -ffreestanding -O2 -Wall -Wextra -Iinclude
ASFLAGS = -f elf32
LDFLAGS = -m elf_i386 -T boot/linker.ld

SRCS = src/kernel.c src/kmain.c src/vga.c src/gdt.c src/idt.c src/irq.c
OBJS = $(SRCS:.c=.o) src/asm.o boot/boot.o

.PHONY: all clean run

all: galio.bin

galio.bin: $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $(OBJS)

boot/boot.o: boot/boot.S
	$(CC) $(CFLAGS) -c boot/boot.S -o boot/boot.o

src/asm.o: src/asm.s
	$(AS) $(ASFLAGS) src/asm.s -o src/asm.o

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f galio.bin $(OBJS)

run: galio.bin
	qemu-system-i386 -kernel galio.bin -m 128M -serial stdio
