# 🚀 RetroSpaceOS v1.0

<div align="center">

```
  ____      _             ____                       ___  ____  
 |  _ \ ___| |_ _ __ ___ / ___| _ __   __ _  ___ ___/ _ \/ ___| 
 | |_) / _ \ __| '__/ _ \\___ \| '_ \ / _` |/ __/ _ \ | | \___ \ 
 |  _ <  __/ |_| | | (_) |___) | |_) | (_| | (_|  __/ |_| |___) |
 |_| \_\___|\___|_|  \___/|____/| .__/ \___,_|\___\___|\___|____/ 
                                |_|                       v1.0   
```

**Un sistema operativo educativo de 32 bits desarrollado desde cero**

</div>

---

## 📋 Tabla de Contenidos

- [Características](#-características)
- [Requisitos](#-requisitos)
- [Instalación Rápida](#-instalación-rápida)
- [Uso](#-uso)
- [Comandos del Shell](#-comandos-del-shell)
- [Estructura del Proyecto](#-estructura-del-proyecto)
- [Documentación](#-documentación)
- [Desarrollo](#-desarrollo)

---

## ✨ Características

### Sistema Base
- ✅ **Bootloader Multiboot** - Compatible con GRUB2
- ✅ **Kernel de 32 bits** - Modo protegido x86
- ✅ **Terminal VGA** - 80x25, 16 colores, scroll automático

### Hardware
- ✅ **IDT Completa** - 256 entradas de interrupciones
- ✅ **Timer PIT** - 100 Hz con soporte para sleep()
- ✅ **Teclado PS/2** - Layout US, Shift, Caps Lock, Ctrl

### Shell Interactivo
- ✅ **12 comandos** disponibles
- ✅ **Prompt personalizado** con colores
- ✅ **Atajos de teclado** (Ctrl+C, Ctrl+L)
- ✅ **Efecto Matrix** y animaciones

---

## 📦 Requisitos

### Compilación
```bash
# Ubuntu/Debian
sudo apt update
sudo apt install build-essential nasm grub-pc-bin xorriso mtools

# Arch Linux
sudo pacman -S base-devel nasm grub libisoburn mtools
```

### Ejecución
```bash
# QEMU (recomendado)
sudo apt install qemu-system-x86    # Ubuntu/Debian
sudo pacman -S qemu-system-x86      # Arch Linux
```

---

## 🚀 Instalación Rápida

```bash
# 1. Clonar el repositorio (si no lo tienes)
git clone https://github.com/tu-usuario/RetroSpaceOS.git
cd RetroSpaceOS

# 2. Compilar
./build.sh

# 3. Ejecutar
./run.sh
```

---

## 💻 Uso

### Compilación
```bash
./build.sh
```

Esto generará:
- `RetroSpaceOS.bin` - Kernel binario (~28 KB)
- `RetroSpaceOS.iso` - ISO booteable (~12 MB)

### Ejecución
```bash
# Modo normal
./run.sh

# Con más memoria
./run.sh -m 1G

# Modo debug (GDB)
./run.sh -d

# Sin display (solo serial)
./run.sh -n
```

---

## 🖥️ Comandos del Shell

| Comando | Descripción |
|---------|-------------|
| `help` | Muestra la ayuda con todos los comandos |
| `clear` | Limpia la pantalla |
| `echo <texto>` | Imprime texto en la terminal |
| `time` | Muestra el tiempo desde el arranque |
| `info` | Información del sistema |
| `about` | Información sobre RetroSpaceOS |
| `reboot` | Reinicia el sistema |
| `color <0-15>` | Cambia el color del texto |
| `rainbow` | Muestra todos los colores disponibles |
| `matrix` | Efecto Matrix (presiona tecla para salir) |
| `calc <n1> <op> <n2>` | Calculadora simple (+, -, *, /) |

### Atajos de Teclado
- `Ctrl+C` - Cancelar línea actual
- `Ctrl+L` - Limpiar pantalla (mantiene prompt)

---

## 📁 Estructura del Proyecto

```
RetroSpaceOS/
├── bootloader/
│   ├── boot.asm           # Bootloader Multiboot
│   ├── interrupts.asm     # ISRs e IRQs en Assembly
│   └── grub.cfg           # Configuración de GRUB
├── src/
│   └── kernel.c           # Código principal del kernel
├── docs/
│   ├── PROGRESS.md        # Progreso del desarrollo
│   ├── ROADMAP.md         # Guía paso a paso
│   ├── QUICK_REFERENCE.md # Referencia rápida
│   └── RESUMEN.md         # Resumen del proyecto
├── linker.ld              # Script del enlazador
├── build.sh               # Script de compilación
├── run.sh                 # Script para ejecutar en QEMU
└── README.md              # Este archivo
```

---

## 📚 Documentación

| Documento | Descripción |
|-----------|-------------|
| [PROGRESS.md](docs/PROGRESS.md) | Estado actual y tareas completadas |
| [ROADMAP.md](docs/ROADMAP.md) | Guía de desarrollo paso a paso |
| [QUICK_REFERENCE.md](docs/QUICK_REFERENCE.md) | Referencia rápida de código |
| [INDEX.md](INDEX.md) | Índice general del proyecto |

---

## 🛠️ Desarrollo

### Componentes Implementados

#### Terminal VGA (kernel.c)
```c
terminal_initialize()    // Inicializa el terminal
terminal_putchar()       // Escribe un carácter
terminal_writestring()   // Escribe una cadena
terminal_clear()         // Limpia la pantalla
terminal_scroll()        // Scroll de la pantalla
```

#### Sistema de Interrupciones
```c
idt_init()              // Inicializa la IDT
idt_set_gate()          // Configura una entrada
isr_handler()           // Maneja excepciones CPU
irq_handler()           // Maneja interrupciones HW
pic_remap()             // Remapea los PICs
```

#### Timer (PIT)
```c
timer_init(100)         // Configura a 100 Hz
timer_handler()         // Incrementa ticks
sleep(1000)             // Espera 1 segundo
get_ticks()             // Obtiene ticks del sistema
```

#### Teclado PS/2
```c
keyboard_handler()              // Maneja IRQ del teclado
keyboard_getchar()              // Lee carácter (bloqueante)
keyboard_getchar_nonblocking()  // Lee carácter (no bloqueante)
keyboard_has_char()             // Verifica buffer
```

---

## 🎨 Colores VGA

| Código | Color |
|--------|-------|
| 0 | Negro |
| 1 | Azul |
| 2 | Verde |
| 3 | Cyan |
| 4 | Rojo |
| 5 | Magenta |
| 6 | Marrón |
| 7 | Gris claro |
| 8 | Gris oscuro |
| 9 | Azul claro |
| 10 | Verde claro |
| 11 | Cyan claro |
| 12 | Rojo claro |
| 13 | Magenta claro |
| 14 | Amarillo |
| 15 | Blanco |

---

## 📊 Estadísticas

- **Líneas de código**: ~1300
- **Tamaño del kernel**: 28 KB
- **Comandos del shell**: 12
- **Interrupciones manejadas**: 48

---

## 🔮 Futuras Mejoras

Las siguientes características están planificadas para versiones futuras:

- [ ] **Gestión de memoria** - kmalloc/kfree
- [ ] **Paginación** - Memoria virtual
- [ ] **Multitarea** - Múltiples procesos
- [ ] **Sistema de archivos** - FAT16 o similar
- [ ] **Modo gráfico VBE** - Interfaz gráfica

---

## 📖 Recursos de Aprendizaje

- [OSDev Wiki](https://wiki.osdev.org/) - Documentación de desarrollo de OS
- [Intel Software Developer Manuals](https://software.intel.com/content/www/us/en/develop/articles/intel-sdm.html)
- [James Molloy's Kernel Tutorial](http://www.jamesmolloy.co.uk/tutorial_html/)

---

## 📜 Licencia

Este proyecto es software libre y puede ser utilizado con fines educativos.

---

<div align="center">

**Desarrollado con ❤️ para aprender sobre sistemas operativos**

*RetroSpaceOS v1.0 - Un paso hacia entender cómo funciona un OS desde cero*

</div>
