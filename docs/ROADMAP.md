# 🗺️ Roadmap de Desarrollo - RetroSpaceOS

Este documento te guía paso a paso en el desarrollo de tu sistema operativo, desde las funciones más básicas hasta un OS funcional completo.

---

## 📊 Progreso General

```
[█░░░░░░░░░] 10% - Bootloader funcional + Kernel básico
[██████████] 100% - Terminal VGA
[░░░░░░░░░░]  0% - Sistema de interrupciones
[░░░░░░░░░░]  0% - Timer
[░░░░░░░░░░]  0% - Teclado
[░░░░░░░░░░]  0% - Gestión de memoria
[░░░░░░░░░░]  0% - Procesos
[░░░░░░░░░░]  0% - Sistema de archivos
[░░░░░░░░░░]  0% - Shell
```

---

## ✅ Fase 0: Fundamentos (COMPLETADO)

**Estado**: ✅ Completado

**Lo que ya tienes**:
- ✅ Bootloader en Assembly (`boot.asm`)
- ✅ Configuración de Multiboot
- ✅ Script de compilación (`build.sh`)
- ✅ Linker script (`linker.ld`)
- ✅ Kernel básico que muestra un mensaje

**Próximo paso**: Fase 1 - Terminal VGA completo

---

## ✅ Fase 1: Terminal VGA Completo (COMPLETADO)

**Objetivo**: Poder escribir texto en pantalla de forma completa

**Complejidad**: ⭐⭐ (2/5)

**Estado**: ✅ Completado

### Tareas

#### 1.1 - Declarar Variables Globales
**Estado**: ✅ Completado

#### 1.2 - Implementar `vga_entry_color`
**Estado**: ✅ Completado

#### 1.3 - Implementar `terminal_initialize`
**Estado**: ✅ Completado

#### 1.4 - Implementar `terminal_setcolor`
**Estado**: ✅ Completado

#### 1.5 - Implementar `terminal_scroll`
**Estado**: ✅ Completado

#### 1.6 - Implementar `terminal_putchar`
**Estado**: ✅ Completado

#### 1.7 - Implementar `terminal_clear`
**Estado**: ✅ Completado

---

### ✅ Verificación de Fase 1

Cuando termines esta fase, deberías poder ejecutar esto en `kernel_main`:

```c
void kernel_main(void) {
    terminal_initialize();
    terminal_writestring("===========================================\n");
    terminal_writestring("  RETROSPACEOS v0.1\n");
    terminal_writestring("===========================================\n\n");
    
    terminal_setcolor(vga_entry_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK));
    terminal_writestring("[OK] Terminal VGA inicializado\n");
    
    terminal_setcolor(vga_entry_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK));
    terminal_writestring("\nProbando scroll...\n");
    
    for (int i = 0; i < 30; i++) {
        terminal_writestring("Esta es una linea de prueba\n");
    }
    
    for (;;) {
        __asm__ volatile ("hlt");
    }
}
```

**Resultado esperado**: Ver texto con colores, y scroll automático cuando se llene la pantalla.

---

## 🔧 Fase 2: Sistema de Interrupciones (IDT)

**Objetivo**: Configurar la tabla de descriptores de interrupciones

**Complejidad**: ⭐⭐⭐⭐ (4/5)

**Tiempo estimado**: 6-10 horas

### Pre-requisitos
- ✅ Fase 1 completada
- 📖 Leer sobre la IDT: https://wiki.osdev.org/Interrupt_Descriptor_Table

### Tareas

#### 2.1 - Definir Estructuras de la IDT
**Ubicación**: Añadir después de las funciones de terminal

```c
struct idt_entry {
    unsigned short base_lo;
    unsigned short sel;
    unsigned char always0;
    unsigned char flags;
    unsigned short base_hi;
} __attribute__((packed));

struct idt_ptr {
    unsigned short limit;
    unsigned int base;
} __attribute__((packed));

struct idt_entry idt[256];
struct idt_ptr idtp;
```

**Estado**: ⬜ Pendiente

---

#### 2.2 - Implementar `idt_set_gate`
**Nueva función**

```c
void idt_set_gate(int num, unsigned long base, unsigned short sel, unsigned char flags) {
    idt[num].base_lo = base & 0xFFFF;
    idt[num].base_hi = (base >> 16) & 0xFFFF;
    idt[num].sel = sel;
    idt[num].always0 = 0;
    idt[num].flags = flags;
}
```

**Estado**: ⬜ Pendiente

---

#### 2.3 - Implementar `idt_init`
**Nueva función**

```c
void idt_init(void) {
    idtp.limit = (sizeof(struct idt_entry) * 256) - 1;
    idtp.base = (unsigned int)&idt;
    
    // Limpiar IDT
    for (int i = 0; i < 256; i++) {
        idt_set_gate(i, 0, 0, 0);
    }
    
    // Cargar IDT
    __asm__ volatile ("lidt (%0)" : : "r"(&idtp));
}
```

**Estado**: ⬜ Pendiente

---

#### 2.4 - Crear Archivo de ISRs en Assembly
**Nuevo archivo**: `bootloader/interrupts.asm`

Este archivo define los stubs de interrupciones. Ejemplo:

```nasm
global isr0
isr0:
    cli
    push byte 0      ; Dummy error code
    push byte 0      ; Número de interrupción
    jmp isr_common_stub

; Repetir para ISR 1-31...

extern isr_handler
isr_common_stub:
    pusha
    push ds
    push es
    push fs
    push gs
    
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov eax, esp
    push eax
    
    call isr_handler
    
    pop eax
    pop gs
    pop fs
    pop es
    pop ds
    popa
    add esp, 8
    iret
```

**Estado**: ⬜ Pendiente

---

#### 2.5 - Implementar Handler de Interrupciones en C
**Nueva función** en `kernel.c`

```c
void isr_handler(struct regs* r) {
    terminal_writestring("Interrupcion recibida: ");
    // Aquí mostrarías el número de interrupción
    terminal_writestring("\n");
}
```

**Estado**: ⬜ Pendiente

---

#### 2.6 - Remapear el PIC
**Nueva función**

```c
void pic_remap(void) {
    outb(0x20, 0x11);
    outb(0xA0, 0x11);
    outb(0x21, 0x20);
    outb(0xA1, 0x28);
    outb(0x21, 0x04);
    outb(0xA1, 0x02);
    outb(0x21, 0x01);
    outb(0xA1, 0x01);
    outb(0x21, 0x0);
    outb(0xA1, 0x0);
}
```

**Estado**: ⬜ Pendiente

---

#### 2.7 - Actualizar `build.sh`
Añadir compilación de `interrupts.asm`:

```bash
nasm -f elf64 bootloader/interrupts.asm -o interrupts.o
```

Y enlazar:
```bash
ld -T linker.ld -o RetroSpaceOS.bin boot.o interrupts.o kernel.o
```

**Estado**: ⬜ Pendiente

---

### ✅ Verificación de Fase 2

```c
void kernel_main(void) {
    terminal_initialize();
    terminal_writestring("Inicializando IDT...\n");
    
    idt_init();
    pic_remap();
    
    terminal_writestring("[OK] IDT Cargada\n");
    
    __asm__ volatile ("sti");  // Habilitar interrupciones
    terminal_writestring("[OK] Interrupciones habilitadas\n");
    
    // Provocar interrupción de división por cero (para probar)
    // int x = 1 / 0;  // Debería llamar a isr_handler
    
    for (;;) {
        __asm__ volatile ("hlt");
    }
}
```

**Resultado esperado**: El sistema no debería hacer triple fault y debería manejar interrupciones.

---

## ⏱️ Fase 3: Timer (PIT)

**Objetivo**: Configurar el timer para generar interrupciones periódicas

**Complejidad**: ⭐⭐⭐ (3/5)

**Tiempo estimado**: 3-5 horas

### Tareas

#### 3.1 - Implementar `timer_init`
```c
void timer_init(unsigned int frequency) {
    unsigned int divisor = 1193180 / frequency;
    outb(0x43, 0x36);
    outb(0x40, divisor & 0xFF);
    outb(0x40, (divisor >> 8) & 0xFF);
}
```

**Estado**: ⬜ Pendiente

---

#### 3.2 - Implementar `timer_handler`
```c
unsigned int timer_ticks = 0;

void timer_handler(struct regs* r) {
    timer_ticks++;
    outb(0x20, 0x20);  // EOI
}
```

**Estado**: ⬜ Pendiente

---

#### 3.3 - Instalar Handler en la IDT
En `idt_init`, después de limpiar:

```c
idt_set_gate(32, (unsigned)timer_handler, 0x08, 0x8E);
```

**Estado**: ⬜ Pendiente

---

### ✅ Verificación de Fase 3

```c
terminal_writestring("Timer inicializado\n");
timer_init(100);  // 100 Hz

for (int i = 0; i < 5; i++) {
    unsigned int start = timer_ticks;
    while (timer_ticks < start + 100) {  // Esperar 1 segundo
        __asm__ volatile ("hlt");
    }
    terminal_writestring("Tick!\n");
}
```

**Resultado esperado**: Ver "Tick!" cada segundo.

---

## ⌨️ Fase 4: Teclado

**Objetivo**: Capturar teclas presionadas

**Complejidad**: ⭐⭐⭐ (3/5)

**Tiempo estimado**: 4-6 horas

### Tareas

#### 4.1 - Crear Tabla de Scancodes
**Ver `docs/ejemplos.c`** para la tabla completa

**Estado**: ⬜ Pendiente

---

#### 4.2 - Implementar `keyboard_handler`
```c
void keyboard_handler(struct regs* r) {
    unsigned char scancode = inb(0x60);
    
    if (scancode < 128) {
        char c = scancode_to_ascii[scancode];
        if (c) {
            terminal_putchar(c);
        }
    }
    
    outb(0x20, 0x20);  // EOI
}
```

**Estado**: ⬜ Pendiente

---

#### 4.3 - Instalar Handler
```c
idt_set_gate(33, (unsigned)keyboard_handler, 0x08, 0x8E);
```

**Estado**: ⬜ Pendiente

---

### ✅ Verificación de Fase 4

```c
terminal_writestring("Escribe algo:\n> ");

for (;;) {
    __asm__ volatile ("hlt");
}
```

**Resultado esperado**: Lo que escribas debería aparecer en pantalla.

---

## 🧠 Fases Restantes (Resumen)

### Fase 5: Gestión de Memoria (⭐⭐⭐⭐⭐)
- Implementar `kmalloc` y `kfree`
- Configurar paginación
- Leer mapa de memoria de Multiboot

### Fase 6: Procesos (⭐⭐⭐⭐⭐)
- Crear estructura de PCB
- Implementar scheduler
- Context switching

### Fase 7: Sistema de Archivos (⭐⭐⭐⭐)
- Driver de disco ATA
- Implementar FS básico

### Fase 8: Shell (⭐⭐⭐)
- Buffer de comandos
- Parser de comandos
- Comandos básicos

---

## 🎯 Hitos Importantes

- [ ] **Hito 1**: Terminal funcional con scroll
- [ ] **Hito 2**: Interrupciones configuradas
- [ ] **Hito 3**: Timer funcionando
- [ ] **Hito 4**: Entrada de teclado
- [ ] **Hito 5**: Memoria dinámica (kmalloc)
- [ ] **Hito 6**: Multitarea básica
- [ ] **Hito 7**: Leer/escribir archivos
- [ ] **Hito 8**: Shell interactivo completo

---

## 📌 Consejos

1. **No te saltes fases**: Cada fase construye sobre la anterior
2. **Prueba constantemente**: Compila y ejecuta después de cada función
3. **Usa QEMU para debug**: Es más rápido que hardware real
4. **Lee la documentación**: OSDev Wiki es tu mejor amigo
5. **No tengas miedo de experimentar**: Los triple faults son parte del aprendizaje

---

## 🆘 ¿Atascado?

Si tienes problemas:

1. Revisa `docs/ejemplos.c` para ver implementaciones de referencia
2. Consulta el README.md para recursos adicionales
3. Busca en OSDev Wiki
4. Usa GDB para debuggear

---

**¡Buena suerte con tu desarrollo! 🚀**
