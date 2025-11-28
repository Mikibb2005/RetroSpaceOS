bits 32
global _start

; ===== MULTIBOOT HEADER =====
section .multiboot
align 4
multiboot_header:
    dd 0x1BADB002          ; Magic number (obligatorio)
    dd 0x00                ; Flags
    dd -(0x1BADB002 + 0x00) ; Checksum (debe sumar 0)

; ===== CÓDIGO DE INICIO =====
section .text
_start:
    ; Inicializar stack
    mov esp, stack_top
    
    ; Llamar a kernel_main (tu código en C)
    extern kernel_main
    call kernel_main
    
    ; Bucle infinito si kernel_main retorna
    cli
.hang:
    hlt
    jmp .hang

; ===== STACK =====
section .bss
align 16
stack_bottom:
    resb 16384              ; 16 KiB de stack
stack_top:
