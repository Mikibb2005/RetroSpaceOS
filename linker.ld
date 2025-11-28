ENTRY(_start)

SECTIONS {
    . = 1M;  /* Comenzamos en el mega byte (dirección física 0x100000) */

    /* Sección de código y header multiboot */
    .text : ALIGN(4K) {
        *(.multiboot)  /* El header DEBE estar primero */
        *(.text)
        *(.text.*)
    }

    /* Datos de solo lectura */
    .rodata : ALIGN(4K) {
        *(.rodata)
        *(.rodata.*)
    }

    /* Datos modificables */
    .data : ALIGN(4K) {
        *(.data)
        *(.data.*)
    }

    /* BSS (variables sin inicializar) */
    .bss : ALIGN(4K) {
        *(COMMON)
        *(.bss)
        *(.bootstrap_stack)
    }

    /* Descartar secciones no deseadas */
    /DISCARD/ : {
        *(.note.GNU-stack)
        *(.eh_frame)
        *(.comment)
    }
}
