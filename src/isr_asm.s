; isr_asm.s - ISR and IRQ stubs with error code handling (fixed)
; Builds register frame and preserves segment registers correctly.

BITS 32

; External handlers
extern isr_handler
extern irq_handler
extern syscall_handler

; Macro to define an ISR that pushes a dummy error code
%macro ISR_NOERRCODE 1
global isr%1
isr%1:
    push 0              ; dummy error code
    push %1             ; interrupt number
    jmp isr_common_stub
%endmacro

; Macro to define an ISR that has an error code
%macro ISR_ERRCODE 1
global isr%1
isr%1:
    push %1             ; interrupt number (error code already pushed by CPU)
    jmp isr_common_stub
%endmacro

; Macro to define an IRQ handler
%macro IRQ_STUB 2
global irq%1
irq%1:
    push 0              ; dummy error code
    push %2             ; interrupt number (32 + irq_num)
    jmp irq_common_stub
%endmacro

; ISRs 0-31: CPU Exceptions
ISR_NOERRCODE 0
ISR_NOERRCODE 1
ISR_NOERRCODE 2
ISR_NOERRCODE 3
ISR_NOERRCODE 4
ISR_NOERRCODE 5
ISR_NOERRCODE 6
ISR_NOERRCODE 7
ISR_ERRCODE 8        ; Double fault has error code
ISR_NOERRCODE 9
ISR_ERRCODE 10       ; Invalid TSS has error code
ISR_ERRCODE 11       ; Segment not present has error code
ISR_ERRCODE 12       ; Stack-segment fault has error code
ISR_ERRCODE 13       ; General protection fault has error code
ISR_ERRCODE 14       ; Page fault has error code
ISR_NOERRCODE 15
ISR_NOERRCODE 16
ISR_ERRCODE 17       ; Alignment check has error code
ISR_NOERRCODE 18
ISR_NOERRCODE 19
ISR_NOERRCODE 20
ISR_NOERRCODE 21
ISR_NOERRCODE 22
ISR_NOERRCODE 23
ISR_NOERRCODE 24
ISR_NOERRCODE 25
ISR_NOERRCODE 26
ISR_NOERRCODE 27
ISR_NOERRCODE 28
ISR_NOERRCODE 29
ISR_ERRCODE 30       ; Security exception has error code
ISR_NOERRCODE 31

; IRQs 0-15: Hardware interrupts
IRQ_STUB 0, 32
IRQ_STUB 1, 33
IRQ_STUB 2, 34
IRQ_STUB 3, 35
IRQ_STUB 4, 36
IRQ_STUB 5, 37
IRQ_STUB 6, 38
IRQ_STUB 7, 39
IRQ_STUB 8, 40
IRQ_STUB 9, 41
IRQ_STUB 10, 42
IRQ_STUB 11, 43
IRQ_STUB 12, 44
IRQ_STUB 13, 45
IRQ_STUB 14, 46
IRQ_STUB 15, 47

; Syscall handler (INT 0x80)
global isr_syscall
isr_syscall:
    push 0              ; dummy error code
    push 0x80           ; syscall number
    jmp isr_common_stub

; Common ISR stub - builds register structure and calls C handler
isr_common_stub:
    pusha               ; push all general purpose registers
    push ds             ; save current data segment selector

    mov ax, 0x10        ; kernel data segment selector
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    call isr_handler

    pop ds              ; restore original data segment selector
    mov ax, ds
    mov es, ax
    mov fs, ax
    mov gs, ax

    popa
    add esp, 8          ; remove error code and interrupt number
    iret

; Common IRQ stub - builds register structure and calls C handler
irq_common_stub:
    pusha               ; push all general purpose registers
    push ds             ; save current data segment selector

    mov ax, 0x10        ; kernel data segment selector
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    call irq_handler

    pop ds              ; restore original data segment selector
    mov ax, ds
    mov es, ax
    mov fs, ax
    mov gs, ax

    popa
    add esp, 8          ; remove error code and interrupt number
    iret
