/*
 * ============================================================================
 * RETROSPACEOS KERNEL - Núcleo del Sistema Operativo
 * ============================================================================
 * Este archivo contiene el código principal del kernel de RetroSpaceOS.
 * El kernel es el componente central del sistema operativo que gestiona:
 * - Memoria
 * - Procesos
 * - Dispositivos de E/S
 * - Sistema de archivos
 * ============================================================================
 */

// ============================================================================
// DEFINICIONES Y CONSTANTES DEL SISTEMA
// ============================================================================

/*
 * Dirección de memoria del buffer de video VGA en modo texto.
 * 0xB8000 es la ubicación estándar en memoria donde se mapea la pantalla VGA.
 * Cada carácter ocupa 2 bytes:
 *   - Byte 0: El carácter ASCII a mostrar
 *   - Byte 1: Atributo de color (4 bits fondo + 4 bits texto)
 */
#define VGA_MEMORY 0xB8000
#define NULL ((void *)0)

/*
 * Dimensiones de la pantalla en modo texto VGA (80 columnas x 25 filas).
 * Esto nos da un total de 2000 caracteres que podemos mostrar.
 */
#define VGA_WIDTH 80
#define VGA_HEIGHT 25

/*
 * Enumeración de colores VGA disponibles en modo texto.
 * Estos valores se pueden combinar para crear el byte de atributo.
 */
enum vga_color {
  VGA_COLOR_BLACK = 0,
  VGA_COLOR_BLUE = 1,
  VGA_COLOR_GREEN = 2,
  VGA_COLOR_CYAN = 3,
  VGA_COLOR_RED = 4,
  VGA_COLOR_MAGENTA = 5,
  VGA_COLOR_BROWN = 6,
  VGA_COLOR_LIGHT_GREY = 7,
  VGA_COLOR_DARK_GREY = 8,
  VGA_COLOR_LIGHT_BLUE = 9,
  VGA_COLOR_LIGHT_GREEN = 10,
  VGA_COLOR_LIGHT_CYAN = 11,
  VGA_COLOR_LIGHT_RED = 12,
  VGA_COLOR_LIGHT_MAGENTA = 13,
  VGA_COLOR_LIGHT_BROWN = 14,
  VGA_COLOR_WHITE = 15,
};

// ============================================================================
// VARIABLES GLOBALES DEL KERNEL
// ============================================================================

/*
 * TODO: Añadir aquí variables globales necesarias para:
 * - Posición actual del cursor (terminal_row, terminal_column)
 * - Color actual del texto (terminal_color)
 * - Puntero al buffer de video (terminal_buffer)
 */
unsigned int terminal_row;       // posicion Y cursor
unsigned int terminal_column;    // posicion X cursor
unsigned int terminal_color;     // color del texto
unsigned short *terminal_buffer; // puntero al buffer

// ============================================================================
// FUNCIONES AUXILIARES PARA MANEJO DE VIDEO VGA
// ============================================================================

/*
 * vga_entry_color: Crea un byte de atributo de color VGA
 *
 * @param fg: Color del texto (foreground)
 * @param bg: Color del fondo (background)
 * @return: Byte de color combinado (bg << 4 | fg)
 *
 * TODO: Implementar esta función para combinar colores de fondo y texto
 * Ejemplo: vga_entry_color(VGA_COLOR_WHITE, VGA_COLOR_BLUE) = 0x1F
 */
static inline unsigned char vga_entry_color(enum vga_color fg,
                                            enum vga_color bg) {
  // TODO: Implementar combinación de colores (bg << 4 | fg)
  if (bg << 4 | fg) {
    return bg << 4 | fg;
  }

  return 0x0F; // Placeholder: blanco sobre negro
}

/*
 * vga_entry: Crea una entrada VGA completa (carácter + color)
 *
 * @param uc: Carácter a mostrar
 * @param color: Byte de color (creado con vga_entry_color)
 * @return: Palabra de 16 bits con el carácter y su atributo
 *
 * TODO: Implementar esta función para crear entradas VGA
 * Los 8 bits bajos son el carácter, los 8 bits altos son el color
 */
static inline unsigned short vga_entry(unsigned char uc, unsigned char color) {
  // TODO: Implementar (color << 8 | uc)
  return color << 8 | uc;
}

/*
 * strlen: Calcula la longitud de una cadena
 *
 * @param str: Puntero a la cadena terminada en null
 * @return: Número de caracteres antes del '\0'
 *
 * TODO: Implementar esta función recorriendo la cadena hasta encontrar '\0'
 */
unsigned int strlen(const char *str) {
  // TODO: Implementar conteo de caracteres
  unsigned int len = 0;
  while (str[len]) {
    len++;
  }
  return len;
}

// ============================================================================
// FUNCIONES DE INICIALIZACIÓN DEL TERMINAL
// ============================================================================

/*
 * terminal_initialize: Inicializa el terminal VGA
 *
 * Esta función debe:
 * 1. Establecer la posición del cursor en (0,0)
 * 2. Configurar el color por defecto
 * 3. Obtener el puntero al buffer de video VGA
 * 4. Limpiar toda la pantalla (llenar con espacios)
 *
 * TODO: Implementar inicialización completa del terminal
 */
void terminal_initialize(void) {
  // TODO: Configurar variables globales del terminal
  terminal_row = 0;
  terminal_column = 0;
  terminal_color = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
  terminal_buffer = (unsigned short *)VGA_MEMORY;

  // Limpiar toda la pantalla (80x25 caracteres)
  for (unsigned int y = 0; y < VGA_HEIGHT; y++) {
    for (unsigned int x = 0; x < VGA_WIDTH; x++) {
      const unsigned int index = y * VGA_WIDTH + x;
      terminal_buffer[index] = vga_entry(' ', terminal_color);
    }
  }
}

/*
 * terminal_setcolor: Cambia el color actual del terminal
 *
 * @param color: Nuevo byte de color a usar
 *
 * TODO: Implementar esta función para cambiar terminal_color
 */
void terminal_setcolor(unsigned char color) {
  // TODO: Actualizar variable global de color
  terminal_color = color;
}

/*
 * terminal_putentryat: Escribe un carácter con color en una posición específica
 *
 * @param c: Carácter a escribir
 * @param color: Color del carácter
 * @param x: Columna (0-79)
 * @param y: Fila (0-24)
 *
 * TODO: Implementar escritura directa en buffer VGA
 * Recordar: posición en buffer = y * VGA_WIDTH + x
 */
void terminal_putentryat(char c, unsigned char color, unsigned int x,
                         unsigned int y) {
  // TODO: Calcular índice en el buffer
  // TODO: Escribir la entrada VGA en esa posición
  const unsigned int index = y * VGA_WIDTH + x;
  unsigned short *terminal_buffer = (unsigned short *)VGA_MEMORY;
  terminal_buffer[index] = vga_entry(c, color);
}

// ============================================================================
// FUNCIONES DE ESCRITURA EN TERMINAL
// ============================================================================

/*
 * terminal_putchar: Escribe un carácter en la posición actual del cursor
 *
 * @param c: Carácter a escribir
 *
 * Esta función debe:
 * 1. Si el carácter es '\n', mover a la siguiente línea
 * 2. Escribir el carácter en la posición actual
 * 3. Avanzar la posición del cursor
 * 4. Si llegamos al final de la línea, pasar a la siguiente
 * 5. Si llegamos al final de la pantalla, hacer scroll
 *
 * TODO: Implementar lógica completa de escritura de caracteres
 */

void terminal_scroll() {
  unsigned short *buffer = (unsigned short *)VGA_MEMORY;

  for (unsigned int y = 0; y < VGA_HEIGHT; y++)
    for (unsigned int x = 0; x < VGA_WIDTH; x++)
      buffer[y * VGA_WIDTH + x] = buffer[(y + 1) * VGA_WIDTH + x];

  unsigned short blank = vga_entry(' ', terminal_color);
  for (unsigned int x = 0; x < VGA_WIDTH; x++)
    buffer[(VGA_HEIGHT - 1) * VGA_WIDTH + x] = blank;
}

void terminal_putchar(char c) {
  // TODO: Manejar saltos de línea ('\n')
  // TODO: Escribir carácter usando terminal_putentryat
  // TODO: Actualizar posición del cursor (terminal_column++)
  // TODO: Manejar fin de línea (pasar a siguiente fila)
  // TODO: Implementar scroll si es necesario
  if (c == '\n') {
    terminal_row = terminal_row + 1;
    terminal_column = 0;
  } else {
    terminal_putentryat(c, terminal_color, terminal_column, terminal_row);
    terminal_column = terminal_column + 1;

    if (terminal_column == VGA_WIDTH) {
      terminal_row = terminal_row + 1;
      terminal_column = 0;
    }
  }
  if (terminal_row == VGA_HEIGHT) {
    terminal_scroll();
    terminal_row = VGA_HEIGHT - 1;
  }
}

/*
 * terminal_write: Escribe una cadena de longitud específica
 *
 * @param data: Puntero a los datos a escribir
 * @param size: Número de caracteres a escribir
 *
 * TODO: Implementar bucle que llame a terminal_putchar para cada carácter
 */
void terminal_write(const char *data, unsigned int size) {
  // TODO: Iterar sobre los caracteres y llamar a terminal_putchar
  for (unsigned int i = 0; i < size; i++) {
    terminal_putchar(data[i]);
  }
}

/*
 * terminal_writestring: Escribe una cadena terminada en null
 *
 * @param data: Puntero a la cadena a escribir
 *
 * TODO: Implementar usando strlen y terminal_write
 */
void terminal_writestring(const char *data) {
  // TODO: Calcular longitud con strlen
  // TODO: Llamar a terminal_write con la longitud
  terminal_write(data, strlen(data));
}

/*
 * terminal_clear: Limpia toda la pantalla
 *
 * TODO: Implementar limpieza de pantalla (llenar con espacios)
 * TODO: Resetear posición del cursor a (0,0)
 */
void terminal_clear(void) {
  // TODO: Llenar toda la pantalla con espacios en blanco
  // TODO: Resetear terminal_row = 0, terminal_column = 0
}

// ============================================================================
// FUNCIONES DE GESTIÓN DE INTERRUPCIONES
// ============================================================================

/*
 * IDT (Interrupt Descriptor Table): Tabla de descriptores de interrupciones
 * La IDT es una estructura que el procesador usa para saber qué código ejecutar
 * cuando ocurre una interrupción (hardware o software).
 *
 * TODO: Implementar estructura de la IDT
 * TODO: Crear funciones para instalar manejadores de interrupciones
 * TODO: Cargar la IDT usando la instrucción lidt
 */

struct idt_entry {
  unsigned short offset_low;
  unsigned short selector;
  unsigned char zero;
  unsigned char flags;
  unsigned short offset_high;
};

// ============================================================================
// FUNCIONES DE GESTIÓN DE MEMORIA
// ============================================================================

/*
 * TODO: Implementar un gestor de memoria básico
 * Funciones necesarias:
 * - kmalloc: Asignar memoria en el kernel
 * - kfree: Liberar memoria
 * - memory_map: Obtener mapa de memoria del bootloader (Multiboot)
 *
 * El bootloader Multiboot proporciona un mapa de memoria que indica
 * qué regiones están disponibles para usar.
 */

// ============================================================================
// FUNCIONES DE ENTRADA/SALIDA DE PUERTOS
// ============================================================================

/*
 * outb: Escribe un byte en un puerto de E/S
 *
 * @param port: Número de puerto (0-65535)
 * @param value: Valor a escribir
 *
 * TODO: Implementar usando inline assembly
 * Instrucción: outb value, port
 */
static inline void outb(unsigned short port, unsigned char value) {
  // TODO: Implementar con __asm__ volatile
  __asm__ volatile("outb %0, %1" : : "a"(value), "Nd"(port));
}

/*
 * inb: Lee un byte de un puerto de E/S
 *
 * @param port: Número de puerto a leer
 * @return: Byte leído del puerto
 *
 * TODO: Implementar usando inline assembly
 * Instrucción: inb port, %al
 */
static inline unsigned char inb(unsigned short port) {
  // TODO: Implementar con __asm__ volatile
  unsigned char ret;
  __asm__ volatile("inb %1, %0" : "=a"(ret) : "Nd"(port));
  return ret;
}

// ============================================================================
// FUNCIONES DE CONTROL DEL TECLADO
// ============================================================================

/*
 * TODO: Implementar driver básico del teclado
 * El teclado PS/2 usa los puertos 0x60 (datos) y 0x64 (comandos/estado)
 *
 * Funciones a implementar:
 * - keyboard_init: Inicializar el controlador del teclado
 * - keyboard_handler: Manejador de interrupción del teclado (IRQ 1)
 * - scancode_to_ascii: Convertir códigos de escaneo a ASCII
 */

// ============================================================================
// FUNCIONES DE CONTROL DEL TIMER
// ============================================================================

/*
 * TODO: Implementar driver del temporizador (PIT - Programmable Interval Timer)
 * El PIT genera interrupciones a intervalos regulares (IRQ 0)
 *
 * Funciones a implementar:
 * - timer_init: Configurar la frecuencia del timer
 * - timer_handler: Manejador de interrupción del timer
 * - sleep: Función para pausar la ejecución por un tiempo determinado
 */

// ============================================================================
// MAIN DEL KERNEL - PUNTO DE ENTRADA
// ============================================================================

/*
 * kernel_main: Punto de entrada principal del kernel
 *
 * Esta función es llamada desde el bootloader (boot.asm) después de que
 * el sistema ha arrancado y está en modo protegido de 32 bits.
 *
 * El bootloader ya ha configurado:
 * - Stack funcional
 * - Modo protegido de 32 bits
 * - Información Multiboot en los registros EBX y EAX
 *
 * Aquí debes:
 * 1. Inicializar el terminal VGA
 * 2. Mostrar un mensaje de bienvenida
 * 3. Inicializar subsistemas (IDT, interrupciones, etc.)
 * 4. Entrar en un bucle infinito o iniciar el shell
 */
void kernel_main(void) {
  // Inicialización básica del terminal
  unsigned short *video = (unsigned short *)VGA_MEMORY;

  // Mostrar mensaje de bienvenida
  const char *welcome = "RetroSpaceOS v0.1 - Kernel Loaded!";
  unsigned char color = 0x4F; // Blanco sobre rojo

  // Escribir mensaje carácter por carácter
  for (int i = 0; welcome[i] != '\0'; i++) {
    video[i] = (unsigned short)welcome[i] | ((unsigned short)color << 8);
  }

  /*
   * TODO: Descomentar y completar cuando implementes las funciones
   *
   * terminal_initialize();
   * terminal_writestring("===========================================\n");
   * terminal_writestring("  BIENVENIDO A RETROSPACEOS\n");
   * terminal_writestring("  Sistema Operativo en Desarrollo\n");
   * terminal_writestring("===========================================\n\n");
   *
   * terminal_writestring("Inicializando subsistemas...\n");
   *
   * // TODO: Inicializar IDT
   * // idt_init();
   * // terminal_writestring("[OK] IDT inicializada\n");
   *
   * // TODO: Habilitar interrupciones
   * // __asm__ volatile ("sti");
   * // terminal_writestring("[OK] Interrupciones habilitadas\n");
   *
   * // TODO: Inicializar timer
   * // timer_init();
   * // terminal_writestring("[OK] Timer inicializado\n");
   *
   * // TODO: Inicializar teclado
   * // keyboard_init();
   * // terminal_writestring("[OK] Teclado inicializado\n");
   *
   * terminal_writestring("\nSistema listo. Escribe algo!\n> ");
   */

  // Bucle infinito - El kernel nunca debe terminar

  terminal_initialize();
  terminal_writestring("===========================================\n");
  terminal_writestring("  BIENVENIDO A RETROSPACEOS\n");
  terminal_writestring("  Sistema Operativo en Desarrollo\n");
  terminal_writestring("===========================================\n\n");

  terminal_writestring("Inicializando subsistemas...\n");

  idt_init();
  terminal_writestring("[OK] IDT inicializada\n");

  for (;;) {
    /*
     * Instrucción HLT (Halt): Detiene el procesador hasta la próxima
     * interrupción Esto ahorra energía y evita que el CPU esté ocupado al 100%
     *
     * TODO: Cuando implementes interrupciones, aquí podrías procesar eventos
     * del teclado, timer, etc.
     */
    __asm__ volatile("hlt");
  }
}

/*
 * ============================================================================
 * GUÍA DE DESARROLLO DEL KERNEL
 * ============================================================================
 *
 * FASE 1 - FUNDAMENTOS (LO BÁSICO):
 * --------------------------------
 * 1. Completar las funciones de terminal (terminal_initialize,
 * terminal_putchar, etc.)
 * 2. Implementar terminal_scroll para hacer scroll cuando la pantalla está
 * llena
 * 3. Probar escribiendo mensajes en diferentes colores
 *
 * FASE 2 - INTERRUPCIONES:
 * -----------------------
 * 1. Crear estructura de la IDT (256 entradas)
 * 2. Implementar idt_set_gate para instalar manejadores
 * 3. Implementar idt_init para cargar la IDT
 * 4. Programar el PIC (Programmable Interrupt Controller) para remapear IRQs
 * 5. Crear manejadores básicos de excepciones (división por cero, page fault,
 * etc.)
 *
 * FASE 3 - TIMER Y TECLADO:
 * ------------------------
 * 1. Implementar driver del PIT para generar interrupciones de tiempo
 * 2. Crear un contador de ticks y funciones de tiempo
 * 3. Implementar driver del teclado PS/2
 * 4. Crear tabla de conversión de scancodes a ASCII
 * 5. Implementar buffer circular para entrada de teclado
 *
 * FASE 4 - GESTIÓN DE MEMORIA:
 * ---------------------------
 * 1. Leer el mapa de memoria del Multiboot
 * 2. Implementar allocador de memoria física (bitmap o buddy system)
 * 3. Implementar paginación (crear tablas de páginas)
 * 4. Implementar kmalloc/kfree para kernel heap
 *
 * FASE 5 - PROCESOS Y MULTITAREA:
 * ------------------------------
 * 1. Crear estructura de PCB (Process Control Block)
 * 2. Implementar cambio de contexto
 * 3. Crear scheduler básico (round-robin)
 * 4. Implementar syscalls básicas
 *
 * FASE 6 - SISTEMA DE ARCHIVOS:
 * ----------------------------
 * 1. Implementar driver de disco (ATA/IDE)
 * 2. Crear sistema de archivos simple (FAT16 o propio)
 * 3. Implementar funciones de archivo (open, read, write, close)
 *
 * FASE 7 - SHELL Y COMANDOS:
 * -------------------------
 * 1. Crear un shell básico
 * 2. Implementar comandos básicos (ls, cat, echo, clear, etc.)
 * 3. Agregar soporte para ejecutar programas
 *
 * ============================================================================
 * RECURSOS ÚTILES:
 * ============================================================================
 * - OSDev Wiki: https://wiki.osdev.org/
 * - Intel Software Developer Manuals:
 * https://software.intel.com/content/www/us/en/develop/articles/intel-sdm.html
 * - James Molloy's Kernel Tutorial: http://www.jamesmolloy.co.uk/tutorial_html/
 * - Writing a Simple Operating System from Scratch by Nick Blundell
 * ============================================================================
 */
