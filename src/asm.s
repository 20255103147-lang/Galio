; src/asm.S
; Provides gdt_flush and idt_load wrappers and a simple isr stub (unused but handy)
BITS 32
GLOBAL gdt_flush
GLOBAL idt_load

; gdt_flush: arg = pointer to gdt_ptr (limit:16, base:32)
gdt_flush:
    mov eax, [esp + 4]      ; address of gdt_ptr
    lgdt [eax]
    ; reload segments
    mov ax, 0x10            ; data selector (index 2)
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    ; far jump to reload cs
    jmp 0x08:flush_cs
flush_cs:
    ret

; idt_load: arg = pointer to idt_ptr (limit:16, base:32)
idt_load:
    mov eax, [esp + 4]
    lidt [eax]
    ret
