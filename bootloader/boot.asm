bits 32
global _start

; ===== MULTIBOOT HEADER =====
section .multiboot
align 4
multiboot_header:
    dd 0x1BADB002          ; Magic number (obligatorio)
    dd 0x00                ; Flags
    dd -(0x1BADB002 + 0x00) ; Checksum (debe sumar 0)

; ===== GDT (Global Descriptor Table) =====
section .data
align 4

; GDT con 3 entradas: null, código, datos
gdt_start:
    ; Null descriptor (requerido)
    dq 0x0

gdt_code:
    ; Code segment: base=0, limit=4GB, executable, readable
    dw 0xFFFF       ; Límite 0-15
    dw 0x0000       ; Base 0-15
    db 0x00         ; Base 16-23
    db 10011010b    ; Access byte: present, ring 0, code segment, executable, readable
    db 11001111b    ; Flags (granularidad 4KB) + límite 16-19
    db 0x00         ; Base 24-31

gdt_data:
    ; Data segment: base=0, limit=4GB, writable
    dw 0xFFFF       ; Límite 0-15
    dw 0x0000       ; Base 0-15
    db 0x00         ; Base 16-23
    db 10010010b    ; Access byte: present, ring 0, data segment, writable
    db 11001111b    ; Flags (granularidad 4KB) + límite 16-19
    db 0x00         ; Base 24-31

gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1  ; Límite (tamaño de GDT - 1)
    dd gdt_start                 ; Base de la GDT

; Selectores de segmento
CODE_SEG equ gdt_code - gdt_start  ; 0x08
DATA_SEG equ gdt_data - gdt_start  ; 0x10

; ===== CÓDIGO DE INICIO =====
section .text
_start:
    ; Deshabilitar interrupciones mientras configuramos
    cli
    
    ; Cargar nuestra GDT
    lgdt [gdt_descriptor]
    
    ; Actualizar selectores de datos
    mov ax, DATA_SEG
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    
    ; Far jump para actualizar CS y entrar en modo protegido correcto
    jmp CODE_SEG:.flush_cs

.flush_cs:
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

; Marcar el stack como no ejecutable (para silenciar warning del linker)
section .note.GNU-stack noalloc noexec nowrite progbits
