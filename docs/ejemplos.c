/*
 * ============================================================================
 * RETROSPACEOS - EJEMPLOS DE CÓDIGO PARA EL DESARROLLO
 * ============================================================================
 * Este archivo contiene ejemplos completos de código para cada fase del
 * desarrollo del kernel. Úsalo como referencia mientras implementas las
 * funciones en kernel.c
 * ============================================================================
 */

// ============================================================================
// FASE 1: EJEMPLOS DE FUNCIONES DE TERMINAL
// ============================================================================

/*
 * Ejemplo completo de terminal_initialize
 */
void ejemplo_terminal_initialize(void) {
  // Variables globales que deberías declarar al inicio del kernel.c
  // static unsigned int terminal_row = 0;
  // static unsigned int terminal_column = 0;
  // static unsigned char terminal_color;
  // static unsigned short* terminal_buffer;

  // Inicializar posición
  // terminal_row = 0;
  // terminal_column = 0;

  // Establecer color por defecto (blanco sobre negro)
  // terminal_color = vga_entry_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);

  // Obtener puntero al buffer de video
  // terminal_buffer = (unsigned short*)VGA_MEMORY;

  // Limpiar toda la pantalla (80x25 = 2000 caracteres)
  // for (unsigned int y = 0; y < VGA_HEIGHT; y++) {
  //     for (unsigned int x = 0; x < VGA_WIDTH; x++) {
  //         const unsigned int index = y * VGA_WIDTH + x;
  //         terminal_buffer[index] = vga_entry(' ', terminal_color);
  //     }
  // }
}

/*
 * Ejemplo completo de terminal_putchar con scroll
 */
void ejemplo_terminal_putchar(char c) {
  // Si es un salto de línea
  // if (c == '\n') {
  //     terminal_column = 0;
  //     terminal_row++;
  //     if (terminal_row >= VGA_HEIGHT) {
  //         terminal_scroll();  // Hacer scroll
  //         terminal_row = VGA_HEIGHT - 1;
  //     }
  //     return;
  // }

  // Escribir el carácter en la posición actual
  // terminal_putentryat(c, terminal_color, terminal_column, terminal_row);

  // Avanzar el cursor
  // terminal_column++;

  // Si llegamos al final de la línea, pasar a la siguiente
  // if (terminal_column >= VGA_WIDTH) {
  //     terminal_column = 0;
  //     terminal_row++;
  //
  //     // Si llegamos al final de la pantalla, hacer scroll
  //     if (terminal_row >= VGA_HEIGHT) {
  //         terminal_scroll();
  //         terminal_row = VGA_HEIGHT - 1;
  //     }
  // }
}

/*
 * Ejemplo de función de scroll (desplazar todo hacia arriba)
 */
void ejemplo_terminal_scroll(void) {
  // unsigned short blank = vga_entry(' ', terminal_color);
  //
  // // Copiar cada línea a la anterior (empezando desde la línea 1)
  // for (unsigned int row = 0; row < VGA_HEIGHT - 1; row++) {
  //     for (unsigned int col = 0; col < VGA_WIDTH; col++) {
  //         unsigned int src = (row + 1) * VGA_WIDTH + col;  // Línea de abajo
  //         unsigned int dst = row * VGA_WIDTH + col;        // Línea actual
  //         terminal_buffer[dst] = terminal_buffer[src];
  //     }
  // }
  //
  // // Limpiar la última línea
  // for (unsigned int col = 0; col < VGA_WIDTH; col++) {
  //     unsigned int index = (VGA_HEIGHT - 1) * VGA_WIDTH + col;
  //     terminal_buffer[index] = blank;
  // }
}

// ============================================================================
// FASE 2: EJEMPLOS DE SISTEMA DE INTERRUPCIONES (IDT)
// ============================================================================

/*
 * Estructura completa de la IDT
 */
struct idt_entry_example {
  unsigned short base_lo; // Bits 0-15 de la dirección del handler
  unsigned short sel;     // Selector de segmento (0x08 para código kernel)
  unsigned char always0;  // Siempre debe ser 0
  unsigned char flags;    // Flags de tipo y atributos
  unsigned short base_hi; // Bits 16-31 de la dirección del handler
} __attribute__((packed));

struct idt_ptr_example {
  unsigned short limit; // Tamaño de la IDT - 1
  unsigned int base;    // Dirección base de la IDT
} __attribute__((packed));

/*
 * Ejemplo de cómo instalar un handler en la IDT
 */
void ejemplo_idt_set_gate(int num, unsigned long base, unsigned short sel,
                          unsigned char flags) {
  // struct idt_entry idt[256];  // Declarar globalmente
  //
  // idt[num].base_lo = base & 0xFFFF;
  // idt[num].base_hi = (base >> 16) & 0xFFFF;
  // idt[num].sel = sel;
  // idt[num].always0 = 0;
  // idt[num].flags = flags;
}

/*
 * Ejemplo de inicialización de la IDT
 */
void ejemplo_idt_init(void) {
  // Preparar el puntero a la IDT
  // struct idt_ptr idtp;
  // idtp.limit = (sizeof(struct idt_entry) * 256) - 1;
  // idtp.base = (unsigned int)&idt;
  //
  // // Limpiar toda la IDT
  // for (int i = 0; i < 256; i++) {
  //     idt_set_gate(i, 0, 0, 0);
  // }
  //
  // // Cargar la IDT
  // __asm__ volatile ("lidt (%0)" : : "r"(&idtp));
}

/*
 * Ejemplo de handler de interrupción básico
 */
void ejemplo_isr_handler(void) {
  // Esta función sería llamada cuando ocurre una interrupción
  // En assembly debes crear un stub que guarde registros y llame a esto

  // terminal_writestring("Interrupción recibida!\n");

  // Si es una IRQ de hardware (8-15), debes enviar EOI al PIC
  // outb(0x20, 0x20);  // EOI al PIC maestro
}

// ============================================================================
// FASE 3: EJEMPLOS DE TIMER Y TECLADO
// ============================================================================

/*
 * Ejemplo completo del driver del Timer (PIT)
 */
void ejemplo_timer_init(unsigned int frequency) {
  // La frecuencia base del PIT es 1193180 Hz
  // unsigned int divisor = 1193180 / frequency;
  //
  // // Enviar byte de comando al PIT
  // // 0x36 = 00110110b = Canal 0, lobyte/hibyte, modo 3, binario
  // outb(0x43, 0x36);
  //
  // // Enviar el divisor (primero byte bajo, luego byte alto)
  // outb(0x40, divisor & 0xFF);
  // outb(0x40, (divisor >> 8) & 0xFF);
}

/*
 * Variable global para contar ticks del timer
 */
// unsigned int timer_ticks = 0;

/*
 * Handler del timer (IRQ 0)
 */
void ejemplo_timer_handler(void) {
  // Incrementar contador de ticks
  // timer_ticks++;
  //
  // // Aquí puedes hacer scheduling, actualizar reloj, etc.
  //
  // // Enviar EOI (End of Interrupt) al PIC
  // outb(0x20, 0x20);
}

/*
 * Función sleep (espera X ticks)
 */
void ejemplo_sleep(unsigned int ticks) {
  // unsigned int start = timer_ticks;
  // while (timer_ticks < start + ticks) {
  //     __asm__ volatile ("hlt");  // Esperar interrupciones
  // }
}

/*
 * Tabla de conversión de scancodes a ASCII (layout US)
 */
const char scancode_to_ascii_table[128] = {
    0,    27,   '1', '2', '3', '4', '5', '6', '7', '8', '9',  '0', '-',
    '=',  '\b',
    '\t', // Tab
    'q',  'w',  'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[',  ']', '\n',
    0, // Control
    'a',  's',  'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
    0, // Left shift
    '\\', 'z',  'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/',
    0, // Right shift
    '*',
    0,                                                // Alt
    ' ',                                              // Space
    0,                                                // Caps lock
    0,    0,    0,   0,   0,   0,   0,   0,   0,   0, // F1-F10
    0,                                                // Num lock
    0,                                                // Scroll lock
    0,                                                // Home
    0,                                                // Up arrow
    0,                                                // Page up
    '-',
    0, // Left arrow
    0,
    0, // Right arrow
    '+',
    0,                       // End
    0,                       // Down arrow
    0,                       // Page down
    0,                       // Insert
    0,                       // Delete
    0,    0,    0,   0,   0, // F11, F12
};

/*
 * Ejemplo completo del driver del teclado
 */
void ejemplo_keyboard_handler(void) {
  // Leer el scancode del puerto del teclado
  // unsigned char scancode = inb(0x60);
  //
  // // Solo procesar key press (scancodes < 128)
  // if (scancode < 128) {
  //     char c = scancode_to_ascii_table[scancode];
  //
  //     if (c != 0) {
  //         // Mostrar el carácter en pantalla
  //         terminal_putchar(c);
  //     }
  // }
  //
  // // Enviar EOI al PIC
  // outb(0x20, 0x20);
}

// ============================================================================
// FASE 4: EJEMPLOS DE GESTIÓN DE MEMORIA
// ============================================================================

/*
 * Estructura de información Multiboot (proporcionada por GRUB)
 */
struct multiboot_info_example {
  unsigned int flags;
  unsigned int mem_lower; // KB de memoria baja (0-1MB)
  unsigned int mem_upper; // KB de memoria alta (>1MB)
  unsigned int boot_device;
  unsigned int cmdline;
  unsigned int mods_count;
  unsigned int mods_addr;
  // ... más campos
};

/*
 * Allocador de memoria simple (bump allocator)
 */
// unsigned int kernel_heap_start = 0x100000;  // Empieza después de 1MB
// unsigned int kernel_heap_current = 0x100000;
// unsigned int kernel_heap_end = 0x500000;    // Termina en 5MB (ejemplo)

void *ejemplo_kmalloc_simple(unsigned int size) {
  // if (kernel_heap_current + size > kernel_heap_end) {
  //     return 0;  // Sin memoria
  // }
  //
  // void* ptr = (void*)kernel_heap_current;
  // kernel_heap_current += size;
  // return ptr;
}

/*
 * Ejemplo de allocador con alineación
 */
void *ejemplo_kmalloc_aligned(unsigned int size, int align) {
  // if (align && (kernel_heap_current & 0xFFFFF000)) {
  //     // Alinear a 4KB (página)
  //     kernel_heap_current &= 0xFFFFF000;
  //     kernel_heap_current += 0x1000;
  // }
  //
  // return kmalloc_simple(size);
}

// ============================================================================
// FASE 5: EJEMPLOS DE PROCESOS Y MULTITAREA
// ============================================================================

/*
 * Estructura de un Process Control Block (PCB)
 */
struct process_example {
  unsigned int pid;             // Process ID
  unsigned int esp;             // Stack pointer
  unsigned int ebp;             // Base pointer
  unsigned int eip;             // Instruction pointer
  unsigned int page_directory;  // Directorio de páginas
  struct process_example *next; // Siguiente proceso
};

/*
 * Ejemplo de cambio de contexto (context switch)
 */
void ejemplo_switch_to_process(struct process_example *proc) {
  // Esto se implementaría en Assembly para guardar/restaurar registros
  //
  // 1. Guardar registros del proceso actual
  // 2. Cargar registros del nuevo proceso
  // 3. Cambiar el directorio de páginas si es necesario
  // 4. Saltar a la nueva EIP
}

// ============================================================================
// FASE 6: EJEMPLOS DE SISTEMA DE ARCHIVOS
// ============================================================================

/*
 * Estructura de un inodo simple (similar a ext2)
 */
struct inode_example {
  unsigned int size;            // Tamaño del archivo en bytes
  unsigned int blocks[12];      // Punteros a bloques de datos
  unsigned int indirect;        // Puntero a bloque indirecto
  unsigned int double_indirect; // Puntero a bloque doblemente indirecto
};

/*
 * Ejemplo de lectura de un archivo
 */
int ejemplo_file_read(const char *path, char *buffer, unsigned int size) {
  // 1. Buscar el inodo del archivo por su path
  // 2. Leer los bloques de datos
  // 3. Copiar al buffer
  // 4. Retornar bytes leídos
  return 0;
}

// ============================================================================
// FASE 7: EJEMPLOS DE SHELL
// ============================================================================

/*
 * Buffer para comandos del shell
 */
// char command_buffer[256];
// unsigned int command_index = 0;

/*
 * Ejemplo de shell básico
 */
void ejemplo_shell_process_command(const char *cmd) {
  // if (strcmp(cmd, "clear") == 0) {
  //     terminal_clear();
  // }
  // else if (strcmp(cmd, "help") == 0) {
  //     terminal_writestring("Comandos disponibles:\n");
  //     terminal_writestring("  clear - Limpiar pantalla\n");
  //     terminal_writestring("  help  - Mostrar ayuda\n");
  // }
  // else {
  //     terminal_writestring("Comando no reconocido: ");
  //     terminal_writestring(cmd);
  //     terminal_writestring("\n");
  // }
}

/*
 * Loop principal del shell
 */
void ejemplo_shell_main(void) {
  // terminal_writestring("> ");
  //
  // while (1) {
  //     // Esperar entrada del teclado
  //     // Cuando se presione Enter, procesar comando
  //
  //     // if (tecla == '\n') {
  //     //     command_buffer[command_index] = '\0';
  //     //     terminal_putchar('\n');
  //     //
  //     //     if (command_index > 0) {
  //     //         shell_process_command(command_buffer);
  //     //     }
  //     //
  //     //     command_index = 0;
  //     //     terminal_writestring("> ");
  //     // }
  //     // else if (tecla == '\b') { // Backspace
  //     //     if (command_index > 0) {
  //     //         command_index--;
  //     //         // Borrar carácter en pantalla
  //     //     }
  //     // }
  //     // else {
  //     //     command_buffer[command_index++] = tecla;
  //     //     terminal_putchar(tecla);
  //     // }
  // }
}

// ============================================================================
// UTILIDADES ADICIONALES
// ============================================================================

/*
 * Implementación de strcmp (comparar cadenas)
 */
int ejemplo_strcmp(const char *s1, const char *s2) {
  // while (*s1 && (*s1 == *s2)) {
  //     s1++;
  //     s2++;
  // }
  // return *(unsigned char*)s1 - *(unsigned char*)s2;
}

/*
 * Implementación de memcpy (copiar memoria)
 */
void *ejemplo_memcpy(void *dest, const void *src, unsigned int n) {
  // unsigned char* d = (unsigned char*)dest;
  // const unsigned char* s = (const unsigned char*)src;
  //
  // for (unsigned int i = 0; i < n; i++) {
  //     d[i] = s[i];
  // }
  //
  // return dest;
}

/*
 * Implementación de memset (llenar memoria)
 */
void *ejemplo_memset(void *s, int c, unsigned int n) {
  // unsigned char* p = (unsigned char*)s;
  //
  // for (unsigned int i = 0; i < n; i++) {
  //     p[i] = (unsigned char)c;
  // }
  //
  // return s;
}

/*
 * Conversión de número a string (itoa)
 */
void ejemplo_itoa(int n, char *str, int base) {
  // int i = 0;
  // int is_negative = 0;
  //
  // if (n == 0) {
  //     str[i++] = '0';
  //     str[i] = '\0';
  //     return;
  // }
  //
  // if (n < 0 && base == 10) {
  //     is_negative = 1;
  //     n = -n;
  // }
  //
  // while (n != 0) {
  //     int rem = n % base;
  //     str[i++] = (rem > 9) ? (rem - 10) + 'a' : rem + '0';
  //     n = n / base;
  // }
  //
  // if (is_negative) {
  //     str[i++] = '-';
  // }
  //
  // str[i] = '\0';
  //
  // // Invertir la cadena
  // reverse_string(str);
}

// ============================================================================
// CONFIGURACIÓN DEL PIC (Programmable Interrupt Controller)
// ============================================================================

/*
 * El PIC gestiona las interrupciones de hardware (IRQs).
 * Por defecto, las IRQs están mapeadas en 0-15, lo cual choca con
 * las excepciones del CPU (0-31). Debemos remapear las IRQs.
 */
void ejemplo_pic_remap(void) {
  // unsigned char a1, a2;
  //
  // // Guardar máscaras actuales
  // a1 = inb(0x21);
  // a2 = inb(0xA1);
  //
  // // Iniciar secuencia de inicialización
  // outb(0x20, 0x11);  // ICW1: Iniciar PIC maestro
  // outb(0xA0, 0x11);  // ICW1: Iniciar PIC esclavo
  //
  // // ICW2: Remapear interrupciones
  // outb(0x21, 0x20);  // PIC maestro: IRQ 0-7 → INT 32-39
  // outb(0xA1, 0x28);  // PIC esclavo: IRQ 8-15 → INT 40-47
  //
  // // ICW3: Configurar cascada
  // outb(0x21, 0x04);  // PIC maestro: esclavo en IRQ2
  // outb(0xA1, 0x02);  // PIC esclavo: cascada en IRQ2
  //
  // // ICW4: Modo 8086
  // outb(0x21, 0x01);
  // outb(0xA1, 0x01);
  //
  // // Restaurar máscaras
  // outb(0x21, a1);
  // outb(0xA1, a2);
}

/*
 * Habilitar una IRQ específica
 */
void ejemplo_irq_enable(unsigned char irq) {
  // unsigned short port;
  // unsigned char value;
  //
  // if (irq < 8) {
  //     port = 0x21;  // PIC maestro
  // } else {
  //     port = 0xA1;  // PIC esclavo
  //     irq -= 8;
  // }
  //
  // value = inb(port) & ~(1 << irq);
  // outb(port, value);
}

/*
 * Deshabilitar una IRQ específica
 */
void ejemplo_irq_disable(unsigned char irq) {
  // unsigned short port;
  // unsigned char value;
  //
  // if (irq < 8) {
  //     port = 0x21;
  // } else {
  //     port = 0xA1;
  //     irq -= 8;
  // }
  //
  // value = inb(port) | (1 << irq);
  // outb(port, value);
}

// ============================================================================
// NOTAS FINALES
// ============================================================================

/*
 * IMPORTANTE: Este archivo contiene EJEMPLOS de cómo implementar cada función.
 * No copies todo directamente a kernel.c. En su lugar:
 *
 * 1. Lee y entiende cada ejemplo
 * 2. Adapta el código a tu estructura en kernel.c
 * 3. Descomenta las líneas que necesites
 * 4. Declara las variables globales necesarias
 * 5. Prueba cada función individualmente
 *
 * RECURSOS ADICIONALES:
 *
 * - Para entender los puertos de E/S: https://wiki.osdev.org/I/O_Ports
 * - Para la IDT: https://wiki.osdev.org/Interrupt_Descriptor_Table
 * - Para el PIC: https://wiki.osdev.org/PIC
 * - Para paginación: https://wiki.osdev.org/Paging
 * - Para el teclado: https://wiki.osdev.org/PS/2_Keyboard
 *
 * ¡Mucho éxito con tu desarrollo! 🚀
 */
