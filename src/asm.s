; src/asm.S
; Provides gdt_flush, idt_load, and paging operations
BITS 32
GLOBAL gdt_flush
GLOBAL idt_load
GLOBAL paging_enable_asm

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

; paging_enable_asm: arg = page directory address
paging_enable_asm:
    mov eax, [esp + 4]
    mov cr3, eax            ; Load page directory
    mov eax, cr0
    or eax, 0x80000000     ; Set PG bit
    mov cr0, eax            ; Enable paging
    ret

