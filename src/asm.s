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

; process_switch_asm: arg1 = old_regs, arg2 = new_regs
GLOBAL process_switch_asm
process_switch_asm:
    ; Save current registers to old_regs
    mov eax, [esp + 4]      ; old_regs
    mov [eax + 0], esp      ; esp
    mov [eax + 4], ebp      ; ebp
    mov [eax + 8], esi      ; esi
    mov [eax + 12], edi     ; edi
    mov [eax + 16], ebx     ; ebx
    mov [eax + 20], edx     ; edx
    mov [eax + 24], ecx     ; ecx
    mov [eax + 28], eax     ; eax (will be overwritten, but save current)
    pushfd
    pop dword [eax + 32]   ; eflags

    ; Load new registers from new_regs
    mov eax, [esp + 8]      ; new_regs
    mov esp, [eax + 0]      ; esp
    mov ebp, [eax + 4]      ; ebp
    mov esi, [eax + 8]      ; esi
    mov edi, [eax + 12]     ; edi
    mov ebx, [eax + 16]     ; ebx
    mov edx, [eax + 20]     ; edx
    mov ecx, [eax + 24]     ; ecx
    ; Skip eax for now
    push dword [eax + 32]   ; eflags
    popfd
    mov eax, [eax + 28]     ; eax

    ret

