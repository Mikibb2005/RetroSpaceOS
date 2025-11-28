# 🔍 Guía de Referencia Rápida - RetroSpaceOS

Esta es una guía de consulta rápida con la información más importante.

---

## 🚀 Comandos Rápidos

```bash
# Compilar el OS
bash build.sh

# Ejecutar en QEMU (ventana gráfica)
bash run.sh

# Ejecutar sin display (solo serial)
bash run.sh -n

# Ejecutar con modo debug GDB
bash run.sh -d

# Ejecutar con más memoria
bash run.sh -m 1G

# Limpiar archivos compilados
rm -f *.o *.bin
rm -rf iso/
```

---

## 📝 Estructura de Archivos

```
RetroSpaceOS/
├── bootloader/
│   ├── boot.asm        # Código de arranque en Assembly
│   └── grub.cfg        # Configuración de GRUB
├── src/
│   └── kernel.c        # Kernel principal (AQUÍ TRABAJAS)
├── docs/
│   ├── ejemplos.c      # Ejemplos de código para cada fase
│   └── ROADMAP.md      # Roadmap paso a paso
├── build.sh            # Script de compilación
├── run.sh              # Script para ejecutar en QEMU
├── linker.ld           # Script del linker
└── README.md           # Documentación completa
```

---

## 🎯 Direcciones de Memoria Importantes

```
0xB8000          VGA Text Mode Buffer (pantalla)
0x100000         1 MB - Donde se carga el kernel
0x00000000       Inicio de memoria
0xFFFFFFFF       Fin de memoria (4GB en 32-bit)
```

---

## 🔌 Puertos de E/S (I/O Ports)

### Timer (PIT)
```
0x40             Canal 0 - Datos
0x43             Registro de comando
```

### Teclado (PS/2)
```
0x60             Datos / Comandos
0x64             Estado / Comandos
```

### PIC (Programmable Interrupt Controller)
```
0x20             PIC Maestro - Comando
0x21             PIC Maestro - Datos
0xA0             PIC Esclavo - Comando
0xA1             PIC Esclavo - Datos
```

### Serial Port (COM1)
```
0x3F8            Datos
0x3F9            Interrupt Enable Register
0x3FA            Interrupt ID Register
0x3FB            Line Control Register
0x3FC            Modem Control Register
0x3FD            Line Status Register
```

---

## 🎨 Colores VGA

| Color             | Valor | Color             | Valor |
|-------------------|-------|-------------------|-------|
| Negro             | 0     | Gris Oscuro       | 8     |
| Azul              | 1     | Azul Claro        | 9     |
| Verde             | 2     | Verde Claro       | 10    |
| Cyan              | 3     | Cyan Claro        | 11    |
| Rojo              | 4     | Rojo Claro        | 12    |
| Magenta           | 5     | Magenta Claro     | 13    |
| Marrón            | 6     | Amarillo          | 14    |
| Gris Claro        | 7     | Blanco            | 15    |

**Cómo crear color:**
```c
color = (background << 4) | foreground;
// Ejemplo: Blanco sobre azul = (1 << 4) | 15 = 0x1F
```

---

## ⚙️ Flags de la IDT

```
0x8E             Present, Ring 0, 32-bit Interrupt Gate
0x8F             Present, Ring 0, 32-bit Trap Gate
0xEE             Present, Ring 3, 32-bit Interrupt Gate
```

**Estructura**:
- Bit 7: Present (1 = presente)
- Bits 6-5: DPL (Privilege Level, 0-3)
- Bit 4: Storage Segment (0 para interrupciones)
- Bits 3-0: Gate Type (0xE = 32-bit interrupt, 0xF = 32-bit trap)

---

## 🔢 Interrupciones y Excepciones

### Excepciones del CPU (0-31)
```
0    División por cero
1    Debug
2    NMI (Non-Maskable Interrupt)
3    Breakpoint
4    Overflow
5    Bound Range Exceeded
6    Invalid Opcode
7    Device Not Available
8    Double Fault
9    Coprocessor Segment Overrun
10   Invalid TSS
11   Segment Not Present
12   Stack Segment Fault
13   General Protection Fault (GPF)
14   Page Fault
15   Reserved
16   x87 Floating Point Exception
17   Alignment Check
18   Machine Check
19   SIMD Floating Point Exception
20-31 Reserved
```

### IRQs de Hardware (32-47 después de remapear)
```
32 (IRQ 0)   Timer (PIT)
33 (IRQ 1)   Teclado
34 (IRQ 2)   Cascada (PIC esclavo)
35 (IRQ 3)   COM2
36 (IRQ 4)   COM1
37 (IRQ 5)   LPT2
38 (IRQ 6)   Floppy Disk
39 (IRQ 7)   LPT1
40 (IRQ 8)   CMOS Real-time clock
41 (IRQ 9)   Libre
42 (IRQ 10)  Libre
43 (IRQ 11)  Libre
44 (IRQ 12)  PS/2 Mouse
45 (IRQ 13)  Coprocessor
46 (IRQ 14)  Primary ATA
47 (IRQ 15)  Secondary ATA
```

---

## 🔧 Instrucciones de Assembly Útiles

```nasm
cli              ; Deshabilitar interrupciones
sti              ; Habilitar interrupciones
hlt              ; Detener CPU hasta la próxima interrupción
iret             ; Retornar de interrupción
lidt [ptr]       ; Cargar IDT
lgdt [ptr]       ; Cargar GDT
pusha            ; Push todos los registros (EAX, ECX, EDX, EBX, ESP, EBP, ESI, EDI)
popa             ; Pop todos los registros
```

**En C (inline assembly)**:
```c
__asm__ volatile ("cli");           // Deshabilitar interrupciones
__asm__ volatile ("sti");           // Habilitar interrupciones
__asm__ volatile ("hlt");           // Detener CPU
__asm__ volatile ("int $0x80");     // Llamar interrupción 0x80
__asm__ volatile ("lidt (%0)" : : "r"(&idtp));  // Cargar IDT
```

---

## 📦 Multiboot Header

El header Multiboot debe estar en los primeros 8KB del binario:

```nasm
dd 0x1BADB002          ; Magic (obligatorio)
dd <flags>             ; Flags
dd -(0x1BADB002 + <flags>)  ; Checksum (debe sumar 0)
```

**Flags comunes**:
- `0x00`: Alineación básica
- `0x01`: Cargar módulos en límites de página
- `0x02`: Proporcionar mapa de memoria

---

## 🧮 Operaciones Bit a Bit

```c
// Establecer bit
variable |= (1 << n);

// Limpiar bit
variable &= ~(1 << n);

// Alternar bit
variable ^= (1 << n);

// Verificar bit
if (variable & (1 << n)) { /* bit activado */ }

// Extraer rango de bits
value = (variable >> shift) & mask;
```

---

## 🐛 Debugging

### Uso de GDB con QEMU

**Terminal 1** (QEMU):
```bash
bash run.sh -d
```

**Terminal 2** (GDB):
```bash
gdb RetroSpaceOS.bin
(gdb) target remote localhost:1234
(gdb) break kernel_main
(gdb) continue
(gdb) step
(gdb) print variable_name
(gdb) x/10x 0xB8000    # Ver memoria
```

### Printf Debug en Kernel

```c
// Usar serial port para debug
void debug_print(const char* str) {
    while (*str) {
        while (!(inb(0x3FD) & 0x20));
        outb(0x3F8, *str++);
    }
}

// Ejecutar con: bash run.sh
// Los mensajes aparecerán en la terminal
```

---

## 📚 Recursos Online

- **OSDev Wiki**: https://wiki.osdev.org/
- **Intel Manual**: https://software.intel.com/sdm
- **Bran's Tutorial**: http://www.osdever.net/bkerndev/
- **James Molloy's Tutorial**: http://www.jamesmolloy.co.uk/tutorial_html/
- **Reddit r/osdev**: https://reddit.com/r/osdev

---

## ❓ Preguntas Frecuentes

### ¿Por qué triple fault?
- IDT no configurada correctamente
- Stack overflow
- Interrupción sin handler

### ¿Por qué no aparece nada en pantalla?
- Verificar dirección 0xB8000
- Verificar color (no negro sobre negro)
- Verificar que el código se esté ejecutando

### ¿Cómo saber si el kernel se cargó?
- El primer mensaje debería aparecer
- Usar QEMU con `-d int,cpu_reset` para ver debug

### ¿Cómo añadir más archivos .c?
1. Compilar: `gcc -m32 -c src/nuevo.c -o nuevo.o`
2. Enlazar: Añadir `nuevo.o` al comando `ld`

---

## 💡 Consejos Pro

1. **Siempre compila con warnings activados**: `-Wall -Wextra`
2. **Usa `-O0` mientras debuggeas**: Evita optimizaciones
3. **Escribe a serial port**: Mejor que solo pantalla para debug
4. **Commits frecuentes**: Git es tu amigo
5. **Lee el código de otros OS**: BareMetal, ToaruOS, etc.
6. **No copies y pegues sin entender**: Aprende cada línea

---

## 🎓 Próximos Pasos

Una vez tengas todo funcionando:

1. **Modo Largo (64-bit)**: Más moderno y potente
2. **Drivers gráficos**: VBE, framebuffer
3. **Red**: Driver E1000, stack TCP/IP
4. **USB**: Controlador UHCI/EHCI
5. **Sonido**: AC97, Intel HDA
6. **Multicore**: SMP (Symmetric Multiprocessing)

---

**¡Buena suerte! 🚀**

Si tienes dudas, consulta los docs en `docs/` o pregunta en r/osdev.
