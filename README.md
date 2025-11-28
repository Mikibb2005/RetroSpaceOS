# 🚀 RetroSpaceOS - Sistema Operativo de Aprendizaje

**RetroSpaceOS** es un sistema operativo básico escrito desde cero en Assembly y C, diseñado para aprender sobre el desarrollo de sistemas operativos.

## 📋 Índice

- [Estructura del Proyecto](#estructura-del-proyecto)
- [Requisitos](#requisitos)
- [Compilación](#compilación)
- [Ejecución](#ejecución)
- [Guía de Desarrollo](#guía-de-desarrollo)
- [Arquitectura](#arquitectura)
- [Recursos](#recursos)

---

## 📁 Estructura del Proyecto

```
RetroSpaceOS/
├── bootloader/           # Código del bootloader
│   ├── boot.asm         # Entry point en Assembly (32 bits)
│   └── grub.cfg         # Configuración de GRUB
├── src/                 # Código fuente del kernel
│   └── kernel.c         # Kernel principal en C
├── build.sh             # Script de compilación
├── linker.ld            # Script del linker
└── README.md            # Este archivo
```

---

## 🔧 Requisitos

Para compilar y ejecutar RetroSpaceOS necesitas:

### Herramientas de Compilación
- **NASM**: Ensamblador para `boot.asm`
- **GCC**: Compilador C con soporte para cross-compilation
- **LD**: Linker de GNU
- **GRUB**: Para crear ISOs booteables
  - `grub-mkrescue`
  - `xorriso`

### Para Ejecutar
- **QEMU**: Emulador de sistemas (recomendado)
  - `qemu-system-x86_64`

### Instalación en Ubuntu/Debian

```bash
sudo apt update
sudo apt install nasm gcc binutils grub-pc-bin xorriso qemu-system-x86
```

### Instalación en Arch Linux

```bash
sudo pacman -S nasm gcc binutils grub xorriso qemu
```

---

## 🔨 Compilación

Para compilar el sistema operativo, simplemente ejecuta:

```bash
bash build.sh
```

Este script:
1. ✅ Limpia archivos anteriores
2. ✅ Ensambla `boot.asm` con NASM
3. ✅ Compila `kernel.c` con GCC
4. ✅ Enlaza los objetos con LD
5. ✅ Crea una ISO booteable con GRUB

Si todo va bien, obtendrás:
- `boot.o` - Bootloader ensamblado
- `kernel.o` - Kernel compilado
- `RetroSpaceOS.bin` - Binario enlazado
- `RetroSpaceOS.iso` - ISO booteable

---

## 🚀 Ejecución

### Con QEMU (Emulador)

Para ejecutar el OS en QEMU:

```bash
qemu-system-x86_64 -cdrom RetroSpaceOS.iso
```

Para ejecutar con más opciones (recomendado):

```bash
qemu-system-x86_64 -cdrom RetroSpaceOS.iso -m 512M -serial stdio
```

Opciones:
- `-m 512M`: Asigna 512 MB de RAM
- `-serial stdio`: Redirige puerto serial a la consola

### En Hardware Real

⚠️ **ADVERTENCIA**: Solo intenta esto cuando estés seguro de que tu código funciona bien.

1. Graba la ISO en un USB:
   ```bash
   sudo dd if=RetroSpaceOS.iso of=/dev/sdX bs=4M status=progress
   ```
   (Reemplaza `/dev/sdX` con tu dispositivo USB)

2. Bootea desde el USB en tu PC

---

## 📚 Guía de Desarrollo

El kernel está estructurado en **7 fases** de desarrollo. Cada fase añade funcionalidad nueva.

### 📌 FASE 1: Fundamentos del Terminal VGA

**Objetivo**: Poder escribir texto en pantalla

**Tareas**:
1. ✅ Completa `terminal_initialize()`:
   - Inicializa variables globales (row, column, color)
   - Obtén el puntero al buffer VGA (`0xB8000`)
   - Limpia la pantalla (llena con espacios)

2. ✅ Completa `terminal_putchar()`:
   - Maneja saltos de línea (`\n`)
   - Escribe caracteres en la posición del cursor
   - Actualiza la posición del cursor
   - Implementa scroll cuando llegas al final

3. ✅ Completa `vga_entry_color()`:
   ```c
   return (bg << 4) | fg;
   ```

**Prueba**:
```c
terminal_initialize();
terminal_writestring("Hola RetroSpaceOS!\n");
```

---

### 📌 FASE 2: Sistema de Interrupciones (IDT)

**Objetivo**: Configurar la tabla IDT para manejar interrupciones

**Conceptos**:
- **IDT** (Interrupt Descriptor Table): Tabla que le dice al CPU qué hacer cuando ocurre una interrupción
- **ISR** (Interrupt Service Routine): Función que maneja una interrupción específica

**Tareas**:

1. ✅ Define la estructura de una entrada IDT:
   ```c
   struct idt_entry {
       unsigned short base_lo;  // Bits 0-15 de la dirección del handler
       unsigned short sel;      // Selector de segmento de código
       unsigned char always0;   // Siempre 0
       unsigned char flags;     // Flags de tipo y atributos
       unsigned short base_hi;  // Bits 16-31 de la dirección
   } __attribute__((packed));
   ```

2. ✅ Crea un array de 256 entradas:
   ```c
   struct idt_entry idt[256];
   ```

3. ✅ Implementa `idt_set_gate()`:
   ```c
   void idt_set_gate(int num, unsigned long base, unsigned short sel, unsigned char flags) {
       idt[num].base_lo = base & 0xFFFF;
       idt[num].base_hi = (base >> 16) & 0xFFFF;
       idt[num].sel = sel;
       idt[num].always0 = 0;
       idt[num].flags = flags;
   }
   ```

4. ✅ Carga la IDT:
   ```c
   struct idt_ptr {
       unsigned short limit;
       unsigned int base;
   } __attribute__((packed));

   struct idt_ptr idtp;
   idtp.limit = (sizeof(struct idt_entry) * 256) - 1;
   idtp.base = (unsigned int)&idt;

   __asm__ volatile ("lidt (%0)" : : "r"(&idtp));
   ```

---

### 📌 FASE 3: Timer y Teclado

**Objetivo**: Implementar drivers básicos de hardware

#### Timer (PIT - Programmable Interval Timer)

El PIT genera interrupciones a intervalos regulares (IRQ 0).

**Puertos**:
- `0x40`: Canal 0 (genera IRQ 0)
- `0x43`: Registro de comando

**Tarea**:
```c
void timer_init(unsigned int frequency) {
    // Calcular divisor: 1193180 Hz / frequency
    unsigned int divisor = 1193180 / frequency;
    
    // Enviar comando
    outb(0x43, 0x36);
    
    // Enviar divisor (low byte, high byte)
    outb(0x40, divisor & 0xFF);
    outb(0x40, (divisor >> 8) & 0xFF);
}

// Handler de timer
void timer_handler() {
    static unsigned int ticks = 0;
    ticks++;
    // Aquí puedes hacer scheduling, etc.
}
```

#### Teclado (PS/2)

El teclado PS/2 usa los puertos `0x60` (datos) y `0x64` (estado/comando).

**Tarea**:
```c
void keyboard_handler() {
    unsigned char scancode = inb(0x60);
    
    // Convertir scancode a ASCII y mostrar
    if (scancode < 128) {
        char c = scancode_to_ascii(scancode);
        if (c) {
            terminal_putchar(c);
        }
    }
}
```

---

### 📌 FASE 4: Gestión de Memoria

**Objetivo**: Implementar un allocador de memoria básico

**Conceptos**:
- **Paginación**: Divide la memoria en bloques de 4KB
- **Heap del Kernel**: Área de memoria dinámica para `kmalloc()`

**Recursos del Multiboot**:
El bootloader GRUB pasa información sobre la memoria disponible.

```c
// Estructura Multiboot (se pasa en EBX al iniciar)
struct multiboot_info {
    unsigned int flags;
    unsigned int mem_lower;  // KB de memoria baja
    unsigned int mem_upper;  // KB de memoria alta
    // ... más campos
};
```

**Tarea**: Implementar un allocador simple (bump allocator primero, luego mejorar).

---

### 📌 FASE 5: Procesos y Multitarea

**Objetivo**: Ejecutar múltiples "procesos" de forma concurrente

**Conceptos**:
- **PCB** (Process Control Block): Estructura que guarda el estado de un proceso
- **Context Switch**: Cambiar de un proceso a otro
- **Scheduler**: Decide qué proceso ejecutar

---

### 📌 FASE 6: Sistema de Archivos

**Objetivo**: Leer/escribir archivos en disco

**Opciones**:
- Implementar un driver ATA/IDE para discos duros
- Crear un sistema de archivos simple (o usar FAT16)

---

### 📌 FASE 7: Shell y Userspace

**Objetivo**: Crear un shell interactivo

**Funcionalidades**:
- Leer comandos del usuario
- Ejecutar comandos básicos (`ls`, `cat`, `echo`, `clear`)
- Cargar y ejecutar programas de usuario

---

## 🏗️ Arquitectura

### Proceso de Boot

1. **BIOS/UEFI**: 
   - Busca un dispositivo booteable
   - Carga GRUB desde la ISO

2. **GRUB** (Bootloader):
   - Lee `grub.cfg`
   - Carga `RetroSpaceOS.bin` en memoria
   - Salta a `_start` (en `boot.asm`)

3. **boot.asm**:
   - Configura el stack
   - Llama a `kernel_main()` en C

4. **kernel.c** (Kernel):
   - Inicializa subsistemas
   - Entra en bucle infinito

### Modo Protegido de 32 bits

RetroSpaceOS funciona en **modo protegido de 32 bits**, lo que significa:
- ✅ Acceso a 4 GB de memoria
- ✅ Segmentación de memoria
- ✅ Protección entre procesos
- ❌ No tiene las ventajas del modo largo (64 bits)

Para pasar a **64 bits** necesitas:
1. Habilitar PAE (Physical Address Extension)
2. Configurar tablas de paginación de 4 niveles
3. Saltar a modo largo

---

## 🎯 Debugging

### Compilar con símbolos de debug

Modifica `build.sh`:
```bash
gcc -m32 -c src/kernel.c -o kernel.o -ffreestanding -O0 -g -Wall -Wextra
```

### Usar GDB con QEMU

Terminal 1:
```bash
qemu-system-x86_64 -cdrom RetroSpaceOS.iso -s -S
```

Terminal 2:
```bash
gdb RetroSpaceOS.bin
(gdb) target remote localhost:1234
(gdb) break kernel_main
(gdb) continue
```

### Logs del Serial Port

Añade en el kernel:
```c
void serial_putchar(char c) {
    while (!(inb(0x3FD) & 0x20));  // Esperar a que esté listo
    outb(0x3F8, c);
}
```

Ejecuta con:
```bash
qemu-system-x86_64 -cdrom RetroSpaceOS.iso -serial stdio
```

---

## 📖 Recursos Útiles

### Documentación
- 📘 [OSDev Wiki](https://wiki.osdev.org/) - **Recurso principal**
- 📙 [Intel Software Developer Manuals](https://software.intel.com/content/www/us/en/develop/articles/intel-sdm.html)
- 📕 [Writing a Simple Operating System from Scratch](https://www.cs.bham.ac.uk/~exr/lectures/opsys/10_11/lectures/os-dev.pdf) - PDF gratuito

### Tutoriales
- [James Molloy's Kernel Tutorial](http://www.jamesmolloy.co.uk/tutorial_html/)
- [Bran's Kernel Development](http://www.osdever.net/bkerndev/Docs/intro.htm)
- [BareMetal OS](https://github.com/ReturnInfinity/BareMetal-OS)

### Comunidades
- [r/osdev](https://reddit.com/r/osdev) - Subreddit de desarrollo de OS
- [OSDev Forums](https://forum.osdev.org/)
- [Discord: Low Level Programming](https://discord.gg/osdev)

---

## 🐛 Problemas Comunes

### "No se puede cargar el kernel"
- Verifica que el header Multiboot esté correcto
- Asegúrate de que `_start` sea el entry point

### "Triple Fault / Reboot infinito"
- Probablemente una interrupción sin manejar
- Verifica que la IDT esté bien configurada
- Desactiva interrupciones con `cli` si no está la IDT lista

### "Nada aparece en pantalla"
- Verifica que estés escribiendo en `0xB8000`
- Asegúrate de que el color no sea negro sobre negro
- Comprueba el endianness de las entradas VGA

---

## 📄 Licencia

(por poner)
---


---

**Autor**: Desarrollado como proyecto de aprendizaje  
**Versión**: 0.1  
**Última actualización**: 2025
