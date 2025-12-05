; ============================================================================
; RETROSPACEOS - INTERRUPT SERVICE ROUTINES (ISRs)
; ============================================================================
; Este archivo contiene los stubs de las interrupciones en Assembly.
; Las ISRs manejan tanto excepciones del CPU (0-31) como IRQs de hardware (32-47)
; ============================================================================

bits 32
section .text

; ============================================================================
; DECLARACIONES EXTERNAS
; ============================================================================
extern isr_handler      ; Handler de excepciones del CPU en C
extern irq_handler      ; Handler de IRQs de hardware en C

; ============================================================================
; MACRO PARA ISRs SIN CÓDIGO DE ERROR
; ============================================================================
%macro ISR_NOERRCODE 1
global isr%1
isr%1:
    push dword 0            ; Código de error dummy
    push dword %1           ; Número de interrupción
    jmp isr_common_stub
%endmacro

; ============================================================================
; MACRO PARA ISRs CON CÓDIGO DE ERROR
; ============================================================================
%macro ISR_ERRCODE 1
global isr%1
isr%1:
    ; El código de error ya fue pusheado por el CPU
    push dword %1           ; Número de interrupción
    jmp isr_common_stub
%endmacro

; ============================================================================
; MACRO PARA IRQs (Interrupciones de Hardware)
; ============================================================================
%macro IRQ 2
global irq%1
irq%1:
    push dword 0            ; Código de error dummy
    push dword %2           ; Número de interrupción (32-47)
    jmp irq_common_stub
%endmacro

; ============================================================================
; DEFINICIÓN DE ISRs (Excepciones del CPU 0-31)
; ============================================================================
ISR_NOERRCODE 0     ; División por cero
ISR_NOERRCODE 1     ; Debug
ISR_NOERRCODE 2     ; Non-Maskable Interrupt (NMI)
ISR_NOERRCODE 3     ; Breakpoint
ISR_NOERRCODE 4     ; Overflow
ISR_NOERRCODE 5     ; Bound Range Exceeded
ISR_NOERRCODE 6     ; Invalid Opcode
ISR_NOERRCODE 7     ; Device Not Available
ISR_ERRCODE 8       ; Double Fault
ISR_NOERRCODE 9     ; Coprocessor Segment Overrun (obsoleto)
ISR_ERRCODE 10      ; Invalid TSS
ISR_ERRCODE 11      ; Segment Not Present
ISR_ERRCODE 12      ; Stack Fault
ISR_ERRCODE 13      ; General Protection Fault
ISR_ERRCODE 14      ; Page Fault
ISR_NOERRCODE 15    ; Reservado
ISR_NOERRCODE 16    ; x87 Floating-Point Exception
ISR_ERRCODE 17      ; Alignment Check
ISR_NOERRCODE 18    ; Machine Check
ISR_NOERRCODE 19    ; SIMD Floating-Point Exception
ISR_NOERRCODE 20    ; Virtualization Exception
ISR_NOERRCODE 21    ; Control Protection Exception
ISR_NOERRCODE 22    ; Reservado
ISR_NOERRCODE 23    ; Reservado
ISR_NOERRCODE 24    ; Reservado
ISR_NOERRCODE 25    ; Reservado
ISR_NOERRCODE 26    ; Reservado
ISR_NOERRCODE 27    ; Reservado
ISR_NOERRCODE 28    ; Reservado
ISR_NOERRCODE 29    ; Reservado
ISR_NOERRCODE 30    ; Security Exception
ISR_NOERRCODE 31    ; Reservado

; ============================================================================
; DEFINICIÓN DE IRQs (Interrupciones de Hardware 0-15)
; ============================================================================
IRQ 0, 32           ; Timer (PIT)
IRQ 1, 33           ; Teclado
IRQ 2, 34           ; Cascade (para segundo PIC)
IRQ 3, 35           ; COM2
IRQ 4, 36           ; COM1
IRQ 5, 37           ; LPT2
IRQ 6, 38           ; Floppy Disk
IRQ 7, 39           ; LPT1 (puede ser espurio)
IRQ 8, 40           ; CMOS Real Time Clock
IRQ 9, 41           ; Disponible
IRQ 10, 42          ; Disponible
IRQ 11, 43          ; Disponible
IRQ 12, 44          ; PS/2 Mouse
IRQ 13, 45          ; FPU / Coprocessor
IRQ 14, 46          ; Primary ATA
IRQ 15, 47          ; Secondary ATA

; ============================================================================
; ISR COMMON STUB - Punto de entrada común para excepciones
; ============================================================================
; ============================================================================
; ISR COMMON STUB
; ============================================================================
isr_common_stub:
    pusha                   ; Pushes edi, esi, ebp, esp, ebx, edx, ecx, eax
    
    xor eax, eax
    mov ax, ds
    push eax                ; Salvar DS
    mov ax, es
    push eax                ; Salvar ES
    mov ax, fs
    push eax                ; Salvar FS
    mov ax, gs
    push eax                ; Salvar GS
    
    mov ax, 0x10            ; Cargar Data Segment del Kernel
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    
    mov eax, esp            ; Puntero a regs
    push eax
    
    call isr_handler
    
    add esp, 4              ; Limpiar argumento
    
    pop eax                 ; Restaurar GS
    mov gs, ax
    pop eax                 ; Restaurar FS
    mov fs, ax
    pop eax                 ; Restaurar ES
    mov es, ax
    pop eax                 ; Restaurar DS
    mov ds, ax
    
    popa
    add esp, 8              ; Limpiar error code e int_no
    iret

; ============================================================================
; IRQ COMMON STUB
; ============================================================================
irq_common_stub:
    pusha
    
    xor eax, eax
    mov ax, ds
    push eax
    mov ax, es
    push eax
    mov ax, fs
    push eax
    mov ax, gs
    push eax
    
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    
    mov eax, esp
    push eax
    
    call irq_handler
    
    add esp, 4
    
    pop eax
    mov gs, ax
    pop eax
    mov fs, ax
    pop eax
    mov es, ax
    pop eax
    mov ds, ax
    
    popa
    add esp, 8
    iret

; ============================================================================
; SECCIÓN PARA MARCAR EL STACK COMO NO EJECUTABLE
; ============================================================================
section .note.GNU-stack noalloc noexec nowrite progbits
