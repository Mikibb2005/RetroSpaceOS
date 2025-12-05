#include "chess.h"
#include "fs.h"
#include "io.h"
#include "keyboard.h"
#include "memory.h"
#include "rtc.h"
#include "spce.h"
#include "string.h"
#include "terminal.h"
#include "types.h"

#define VGA_WIDTH 80
#define VGA_HEIGHT 25

// ============================================================================
// ESTRUCTURAS Y VARIABLES GLOBALES
// ============================================================================

struct idt_entry {
  uint16_t base_lo;
  uint16_t sel;
  uint8_t always0;
  uint8_t flags;
  uint16_t base_hi;
} __attribute__((packed));

struct idt_ptr {
  uint16_t limit;
  uint32_t base;
} __attribute__((packed));

struct idt_entry idt[256];
struct idt_ptr idtp;

extern void isr0();
extern void isr1();
extern void isr2();
extern void isr3();
extern void isr4();
extern void isr5();
extern void isr6();
extern void isr7();
extern void isr8();
extern void isr9();
extern void isr10();
extern void isr11();
extern void isr12();
extern void isr13();
extern void isr14();
extern void isr15();
extern void isr16();
extern void isr17();
extern void isr18();
extern void isr19();
extern void isr20();
extern void isr21();
extern void isr22();
extern void isr23();
extern void isr24();
extern void isr25();
extern void isr26();
extern void isr27();
extern void isr28();
extern void isr29();
extern void isr30();
extern void isr31();

extern void irq0();
extern void irq1();
extern void irq2();
extern void irq3();
extern void irq4();
extern void irq5();
extern void irq6();
extern void irq7();
extern void irq8();
extern void irq9();
extern void irq10();
extern void irq11();
extern void irq12();
extern void irq13();
extern void irq14();
extern void irq15();

static void idt_set_gate(uint8_t num, uint32_t base, uint16_t sel,
                         uint8_t flags) {
  idt[num].base_lo = base & 0xFFFF;
  idt[num].base_hi = (base >> 16) & 0xFFFF;
  idt[num].sel = sel;
  idt[num].always0 = 0;
  idt[num].flags = flags;
}

// ============================================================================
// PIC
// ============================================================================

static void pic_remap(void) {
  outb(0x20, 0x11);
  outb(0xA0, 0x11);
  io_wait();
  outb(0x21, 0x20);
  io_wait();
  outb(0xA1, 0x28);
  io_wait();
  outb(0x21, 0x04);
  io_wait();
  outb(0xA1, 0x02);
  io_wait();
  outb(0x21, 0x01);
  io_wait();
  outb(0xA1, 0x01);
  io_wait();
  outb(0x21, 0x0);
  outb(0xA1, 0x0);
}

static void pic_send_eoi(uint8_t irq) {
  if (irq >= 8)
    outb(0xA0, 0x20);
  outb(0x20, 0x20);
}

// ============================================================================
// TIMER
// ============================================================================

static volatile uint32_t timer_ticks = 0;
static uint32_t timer_freq = 100;

static void timer_handler(void) { timer_ticks++; }

static void timer_init(uint32_t freq) {
  timer_freq = freq;
  uint32_t divisor = 1193180 / freq;
  outb(0x43, 0x36);
  outb(0x40, divisor & 0xFF);
  outb(0x40, (divisor >> 8) & 0xFF);
}

// ============================================================================
// VARIABLES GLOBALES DE ESTADO
// ============================================================================
static bool show_interrupts = false;
static volatile uint32_t irq_counter = 0;

// ============================================================================
// IRQ HANDLER
// ============================================================================

struct regs {
  unsigned int gs, fs, es, ds;
  unsigned int edi, esi, ebp, esp, ebx, edx, ecx, eax;
  unsigned int int_no, err_code;
  unsigned int eip, cs, eflags, useresp, ss;
};

static const char *irq_names[] = {
    "TIMER    ", "KEYBOARD ", "CASCADE  ", "COM2     ",
    "COM1     ", "LPT2     ", "FLOPPY   ", "LPT1     ",
    "RTC      ", "FREE     ", "FREE     ", "FREE     ",
    "MOUSE    ", "FPU      ", "ATA1     ", "ATA2     "};

static void draw_system_monitor(uint32_t last_irq) {
  // Guardar posición cursor
  size_t old_row = terminal_row;
  size_t old_col = terminal_column;
  uint8_t old_color = terminal_color;

  // Dibujar barra fondo azul
  for (int i = 0; i < VGA_WIDTH; i++) {
    terminal_putentryat(' ', vga_entry_color(VGA_COLOR_WHITE, VGA_COLOR_BLUE),
                        i, 0);
  }

  // Escribir info
  terminal_row = 0;
  terminal_column = 0;
  terminal_setcolor(vga_entry_color(VGA_COLOR_WHITE, VGA_COLOR_BLUE));

  terminal_writestring(" [SYS MON] Ticks: ");
  terminal_write_dec(irq_counter);

  terminal_writestring(" | Last IRQ: ");
  if (last_irq < 16) {
    terminal_writestring(irq_names[last_irq]);
  } else {
    terminal_writestring("UNKNOWN  ");
  }

  terminal_writestring(" (");
  terminal_write_dec(last_irq);
  terminal_writestring(")");

  // Restaurar
  terminal_row = old_row;
  terminal_column = old_col;
  terminal_setcolor(old_color);
}

// Variable para recordar la última interrupción NO-Timer
static uint32_t last_significant_irq = 0;

void irq_handler(struct regs *r) {
  irq_counter++;
  uint32_t irq = r->int_no - 32;

  if (irq != 0) {
    last_significant_irq = irq;
  }

  if (show_interrupts) {
    // Actualizar monitor (optimización: el timer solo actualiza cada 10 ticks
    // para no parpadear tanto, o siempre) Para suavidad, actualizamos siempre
    // pero solo la barra superior
    draw_system_monitor(last_significant_irq);
  }

  if (irq == 0)
    timer_handler();
  else if (irq == 1)
    keyboard_handler();

  pic_send_eoi(irq);
}

void isr_handler(struct regs *r) {
  (void)r;
  // Fault handler stub
}

// ============================================================================
// INICIALIZACIÓN IDT
// ============================================================================

static void idt_init(void) {
  idtp.limit = (sizeof(struct idt_entry) * 256) - 1;
  idtp.base = (uint32_t)&idt;

  kmemset(&idt, 0, sizeof(idt));

  pic_remap();

  // ISRs (0-31)
  idt_set_gate(0, (uint32_t)isr0, 0x08, 0x8E);
  idt_set_gate(1, (uint32_t)isr1, 0x08, 0x8E);
  idt_set_gate(2, (uint32_t)isr2, 0x08, 0x8E);
  idt_set_gate(3, (uint32_t)isr3, 0x08, 0x8E);
  idt_set_gate(4, (uint32_t)isr4, 0x08, 0x8E);
  idt_set_gate(5, (uint32_t)isr5, 0x08, 0x8E);
  idt_set_gate(6, (uint32_t)isr6, 0x08, 0x8E);
  idt_set_gate(7, (uint32_t)isr7, 0x08, 0x8E);
  idt_set_gate(8, (uint32_t)isr8, 0x08, 0x8E);
  idt_set_gate(9, (uint32_t)isr9, 0x08, 0x8E);
  idt_set_gate(10, (uint32_t)isr10, 0x08, 0x8E);
  idt_set_gate(11, (uint32_t)isr11, 0x08, 0x8E);
  idt_set_gate(12, (uint32_t)isr12, 0x08, 0x8E);
  idt_set_gate(13, (uint32_t)isr13, 0x08, 0x8E);
  idt_set_gate(14, (uint32_t)isr14, 0x08, 0x8E);
  idt_set_gate(15, (uint32_t)isr15, 0x08, 0x8E);
  idt_set_gate(16, (uint32_t)isr16, 0x08, 0x8E);
  idt_set_gate(17, (uint32_t)isr17, 0x08, 0x8E);
  idt_set_gate(18, (uint32_t)isr18, 0x08, 0x8E);
  idt_set_gate(19, (uint32_t)isr19, 0x08, 0x8E);
  idt_set_gate(20, (uint32_t)isr20, 0x08, 0x8E);
  idt_set_gate(21, (uint32_t)isr21, 0x08, 0x8E);
  idt_set_gate(22, (uint32_t)isr22, 0x08, 0x8E);
  idt_set_gate(23, (uint32_t)isr23, 0x08, 0x8E);
  idt_set_gate(24, (uint32_t)isr24, 0x08, 0x8E);
  idt_set_gate(25, (uint32_t)isr25, 0x08, 0x8E);
  idt_set_gate(26, (uint32_t)isr26, 0x08, 0x8E);
  idt_set_gate(27, (uint32_t)isr27, 0x08, 0x8E);
  idt_set_gate(28, (uint32_t)isr28, 0x08, 0x8E);
  idt_set_gate(29, (uint32_t)isr29, 0x08, 0x8E);
  idt_set_gate(30, (uint32_t)isr30, 0x08, 0x8E);
  idt_set_gate(31, (uint32_t)isr31, 0x08, 0x8E);

  // IRQs (32-47)
  idt_set_gate(32, (uint32_t)irq0, 0x08, 0x8E);
  idt_set_gate(33, (uint32_t)irq1, 0x08, 0x8E);
  idt_set_gate(34, (uint32_t)irq2, 0x08, 0x8E);
  idt_set_gate(35, (uint32_t)irq3, 0x08, 0x8E);
  idt_set_gate(36, (uint32_t)irq4, 0x08, 0x8E);
  idt_set_gate(37, (uint32_t)irq5, 0x08, 0x8E);
  idt_set_gate(38, (uint32_t)irq6, 0x08, 0x8E);
  idt_set_gate(39, (uint32_t)irq7, 0x08, 0x8E);
  idt_set_gate(40, (uint32_t)irq8, 0x08, 0x8E);
  idt_set_gate(41, (uint32_t)irq9, 0x08, 0x8E);
  idt_set_gate(42, (uint32_t)irq10, 0x08, 0x8E);
  idt_set_gate(43, (uint32_t)irq11, 0x08, 0x8E);
  idt_set_gate(44, (uint32_t)irq12, 0x08, 0x8E);
  idt_set_gate(45, (uint32_t)irq13, 0x08, 0x8E);
  idt_set_gate(46, (uint32_t)irq14, 0x08, 0x8E);
  idt_set_gate(47, (uint32_t)irq15, 0x08, 0x8E);

  __asm__ volatile("lidt (%0)" : : "r"(&idtp));
}

// ============================================================================
// SHELL
// ============================================================================

#define CMD_SIZE 256
static char cmd_buf[CMD_SIZE];
static uint32_t cmd_pos = 0;

static void cmd_help(void) {
  terminal_setcolor(vga_entry_color(VGA_COLOR_YELLOW, VGA_COLOR_BLACK));
  terminal_writestring("\n=== RetroSpaceOS v2.1 ===\n\n");
  terminal_setcolor(vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK));
  terminal_writestring("Comandos:\n");
  terminal_writestring("  help   - Muestra esta ayuda\n");
  terminal_writestring("  clear  - Limpia la pantalla\n");
  terminal_writestring("  time   - Muestra uptime\n");
  terminal_writestring("  irq    - Toggle monitor de interrupciones\n");
  terminal_writestring("  about  - Sobre RetroSpaceOS\n");
  terminal_writestring("  ls     - Listar archivos (RAMFS)\n");
  terminal_writestring("  spce   - SpaceEditor (spce [archivo])\n");
  terminal_writestring("  reboot - Reinicia el sistema\n\n");
}

static void cmd_irq(void) {
  show_interrupts = !show_interrupts;
  if (show_interrupts) {
    terminal_setcolor(vga_entry_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK));
    terminal_writestring("Monitor IRQ: ACTIVADO\n");
  } else {
    terminal_setcolor(vga_entry_color(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK));
    terminal_writestring("Monitor IRQ: DESACTIVADO\n");
    // Limpiar área
    for (int j = 65; j < VGA_WIDTH; j++) {
      terminal_putentryat(
          ' ', vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK), j, 0);
    }
  }
  terminal_setcolor(vga_entry_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK));
}

static void cmd_time(void) {
  rtc_time_t t;
  rtc_get_time(&t);

  terminal_writestring("Fecha: ");
  if (t.day < 10)
    terminal_writestring("0");
  terminal_write_dec(t.day);
  terminal_writestring("/");
  if (t.month < 10)
    terminal_writestring("0");
  terminal_write_dec(t.month);
  terminal_writestring("/");
  terminal_write_dec(2000 + t.year);
  terminal_writestring("\n");

  terminal_writestring("Hora:  ");
  if (t.hour < 10)
    terminal_writestring("0");
  terminal_write_dec(t.hour);
  terminal_writestring(":");
  if (t.minute < 10)
    terminal_writestring("0");
  terminal_write_dec(t.minute);
  terminal_writestring(":");
  if (t.second < 10)
    terminal_writestring("0");
  terminal_write_dec(t.second);
  terminal_writestring("\n");
}

static void cmd_about(void) {
  terminal_setcolor(vga_entry_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK));
  terminal_writestring("\n  RetroSpaceOS v2.1\n");
  terminal_writestring("  - Kernel Modular\n");
  terminal_writestring("  - Dynamic Heap Memory\n");
  terminal_writestring("  - Advanced RAMFS\n");
  terminal_writestring("  - SpaceEditor Pro\n");
  terminal_writestring("  - IRQ Monitoring\n\n");
  terminal_setcolor(vga_entry_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK));
}

static void cmd_reboot(void) {
  terminal_writestring("Reiniciando...\n");
  outb(0x64, 0xFE);
  for (;;)
    __asm__ volatile("cli; hlt");
}

// ============================================================================
// HISTORIAL DE COMANDOS
// ============================================================================
#define HISTORY_SIZE 10
static char history[HISTORY_SIZE][CMD_SIZE];
static int history_count = 0;
static int history_nav_idx = -1;

static void history_add(const char *cmd) {
  if (cmd[0] == 0)
    return;
  // Evitar duplicados consecutivos
  if (history_count > 0) {
    int last_idx = (history_count - 1) % HISTORY_SIZE;
    if (kstrcmp(history[last_idx], cmd) == 0)
      return;
  }
  kstrcpy(history[history_count % HISTORY_SIZE], cmd);
  history_count++;
}

// ============================================================================
// COMANDOS DE ARCHIVOS
// ============================================================================

static void cmd_cat(const char *arg) {
  if (!arg) {
    terminal_setcolor(vga_entry_color(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK));
    terminal_writestring("Uso: cat <archivo>\n");
    terminal_setcolor(vga_entry_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK));
    return;
  }
  fs_node_t *f = fs_open(arg);
  if (!f) {
    terminal_writestring("Archivo no encontrado.\n");
    return;
  }
  if (f->type == FS_DIR) {
    terminal_writestring("Es un directorio.\n");
    return;
  }
  terminal_writestring(f->content);
  terminal_writestring("\n");
}

static void cmd_rm(const char *arg) {
  if (!arg) {
    terminal_setcolor(vga_entry_color(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK));
    terminal_writestring("Uso: rm <archivo>\n");
    terminal_setcolor(vga_entry_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK));
    return;
  }
  fs_delete(arg);
  terminal_writestring("Eliminado (si existia).\n");
}

static void cmd_touch(const char *arg) {
  if (!arg) {
    terminal_setcolor(vga_entry_color(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK));
    terminal_writestring("Uso: touch <archivo>\n");
    terminal_setcolor(vga_entry_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK));
    return;
  }
  if (fs_create_file(arg)) {
    terminal_writestring("Archivo creado.\n");
  } else {
    terminal_writestring("Error: No se pudo crear (¿ya existe?)\n");
  }
}

static void cmd_mkdir(const char *arg) {
  if (!arg) {
    terminal_setcolor(vga_entry_color(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK));
    terminal_writestring("Uso: mkdir <nombre>\n");
    terminal_setcolor(vga_entry_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK));
    return;
  }
  if (fs_create_dir(arg)) {
    terminal_writestring("Directorio creado.\n");
  } else {
    terminal_writestring("Error: No se pudo crear (¿ya existe?)\n");
  }
}

static void cmd_cd(const char *arg) {
  if (!arg) {
    terminal_setcolor(vga_entry_color(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK));
    terminal_writestring("Uso: cd <ruta>\n");
    terminal_setcolor(vga_entry_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK));
    return;
  }
  fs_cd(arg);
}

static void cmd_pwd(void) {
  fs_pwd();
  terminal_writestring("\n");
}

static void cmd_chess(void) {
  chess_start();
  terminal_setcolor(vga_entry_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK));
  terminal_clear();
  terminal_writestring("Gracias por jugar a RetroSpace Chess.\n");
}

static void process_cmd(void) {
  cmd_buf[cmd_pos] = '\0';
  if (cmd_pos == 0)
    return;

  // Guardar en historial
  history_add(cmd_buf);
  history_nav_idx = -1; // Reset navegación

  char *cmd = cmd_buf;
  char *arg = NULL;

  int i = 0;
  while (cmd_buf[i]) {
    if (cmd_buf[i] == ' ') {
      cmd_buf[i] = '\0';
      arg = &cmd_buf[i + 1];
      break;
    }
    i++;
  }

  if (kstrcmp(cmd, "help") == 0)
    cmd_help();
  else if (kstrcmp(cmd, "clear") == 0)
    terminal_clear();
  else if (kstrcmp(cmd, "time") == 0)
    cmd_time();
  else if (kstrcmp(cmd, "irq") == 0)
    cmd_irq();
  else if (kstrcmp(cmd, "about") == 0)
    cmd_about();
  else if (kstrcmp(cmd, "ls") == 0)
    fs_list();
  else if (kstrcmp(cmd, "cat") == 0)
    cmd_cat(arg);
  else if (kstrcmp(cmd, "rm") == 0)
    cmd_rm(arg);
  else if (kstrcmp(cmd, "touch") == 0)
    cmd_touch(arg);
  else if (kstrcmp(cmd, "mkdir") == 0)
    cmd_mkdir(arg);
  else if (kstrcmp(cmd, "cd") == 0)
    cmd_cd(arg);
  else if (kstrcmp(cmd, "pwd") == 0)
    cmd_pwd();
  else if (kstrcmp(cmd, "reboot") == 0)
    cmd_reboot();
  else if (kstrcmp(cmd, "spce") == 0)
    cmd_spce(arg);
  else if (kstrcmp(cmd, "chess") == 0)
    cmd_chess();
  else {
    terminal_setcolor(vga_entry_color(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK));
    terminal_writestring("Comando no encontrado: ");
    terminal_writestring(cmd);
    terminal_writestring("\n");
    terminal_setcolor(vga_entry_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK));
  }
}

static void shell_prompt(void) {
  char cwd[128];
  fs_get_cwd(cwd, 128);

  terminal_setcolor(vga_entry_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK));
  terminal_writestring("user@retrospace");
  terminal_setcolor(vga_entry_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK));
  terminal_writestring(":");
  terminal_setcolor(vga_entry_color(VGA_COLOR_LIGHT_BLUE, VGA_COLOR_BLACK));
  terminal_writestring(cwd);
  terminal_setcolor(vga_entry_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK));
  terminal_writestring("> ");
}

// Auxiliar para autocompletado
static void autocomplete(char *buf, uint32_t *pos) {
  // 1. Encontrar inicio de la palabra actual (último espacio)
  int start = *pos;
  while (start > 0 && buf[start - 1] != ' ') {
    start--;
  }

  // Si no hay nada que completar
  if (start == *pos)
    return;

  char partial[32];
  int len = *pos - start;
  if (len >= 31)
    return;

  for (int i = 0; i < len; i++)
    partial[i] = buf[start + i];
  partial[len] = '\0';

  // 2. Buscar coincidencias en directorio actual
  fs_node_t *dir = fs_get_current_dir();
  if (!dir || dir->type != FS_DIR)
    return;

  fs_node_t *curr = dir->child;
  fs_node_t *match = NULL;
  int matches = 0;

  while (curr) {
    // Check prefix
    bool is_prefix = true;
    for (int i = 0; i < len; i++) {
      if (curr->name[i] != partial[i]) {
        is_prefix = false;
        break;
      }
    }

    if (is_prefix) {
      match = curr;
      matches++;
    }
    curr = curr->next;
  }

  // 3. Completar si es único
  if (matches == 1 && match) {
    // Escribir el resto del nombre
    const char *rest = match->name + len;
    while (*rest &&
           *pos < CMD_SIZE - 2) { // Dejar espacio para posible / o espacio
      buf[(*pos)++] = *rest;
      terminal_putchar(*rest);
      rest++;
    }
    // Si es directorio, añadir /
    if (match->type == FS_DIR) {
      buf[(*pos)++] = '/';
      terminal_putchar('/');
    } else {
      buf[(*pos)++] = ' ';
      terminal_putchar(' ');
    }
  } else if (matches > 1) {
    // Beep o listar opciones (simplificado: beep visual)
    // terminal_writestring("\a");
  }
}

static void shell_run(void) {
  terminal_writestring("\n");
  shell_prompt();

  while (1) {
    unsigned char c = kb_getchar();

    if (c == '\n') {
      terminal_putchar('\n');
      process_cmd();
      cmd_pos = 0;
      shell_prompt();
    } else if (c == '\t') { // TAB Autocomplete
      autocomplete(cmd_buf, &cmd_pos);
    } else if (c == '\b') {
      if (cmd_pos > 0) {
        cmd_pos--;
        terminal_putchar('\b');
      }
    } else if (c == KEY_UP) {
      if (history_count > 0) {
        // Calcular nuevo índice
        if (history_nav_idx == -1)
          history_nav_idx = history_count - 1;
        else if (history_nav_idx > 0 &&
                 history_nav_idx > history_count - HISTORY_SIZE)
          history_nav_idx--;

        // Borrar línea actual
        // Necesitamos saber longitud del prompt para borrar correctamente,
        // pero terminal_putchar('\b') solo mueve cursor.
        // Simplificación: Borrar visualmente todo lo escrito
        // (Asumimos que el usuario no ha hecho wrap de línea)
        while (cmd_pos > 0) {
          terminal_putchar('\b');
          cmd_pos--;
        }

        // Copiar del historial
        const char *hist_cmd = history[history_nav_idx % HISTORY_SIZE];
        kstrcpy(cmd_buf, hist_cmd);
        cmd_pos = kstrlen(cmd_buf);
        terminal_writestring(cmd_buf);
      }
    } else if (c == KEY_DOWN) {
      if (history_count > 0 && history_nav_idx != -1) {
        history_nav_idx++;

        // Borrar línea
        while (cmd_pos > 0) {
          terminal_putchar('\b');
          cmd_pos--;
        }

        if (history_nav_idx >= history_count) {
          history_nav_idx = -1; // Volver a línea vacía
        } else {
          const char *hist_cmd = history[history_nav_idx % HISTORY_SIZE];
          kstrcpy(cmd_buf, hist_cmd);
          cmd_pos = kstrlen(cmd_buf);
          terminal_writestring(cmd_buf);
        }
      }
    } else if (c >= 32 && c < 127 && cmd_pos < CMD_SIZE - 1) {
      cmd_buf[cmd_pos++] = c;
      terminal_putchar(c);
    }
  }
}

// ============================================================================
// KERNEL MAIN
// ============================================================================

void kernel_main(void) {
  terminal_initialize();

  terminal_setcolor(vga_entry_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK));
  terminal_writestring("\n\n");
  terminal_writestring(
      "  ____      _             ____                       \n");
  terminal_writestring(
      " |  _ \\ ___| |_ _ __ ___ / ___| _ __   __ _  ___ ___ \n");
  terminal_writestring(
      " | |_) / _ \\ __| '__/ _ \\\\___ \\| '_ \\ / _` |/ __/ _ \\\n");
  terminal_writestring(
      " |  _ <  __/ |_| | | (_) |___) | |_) | (_| | (_|  __/\n");
  terminal_writestring(
      " |_| \\_\\___|\\__|_|  \\___/|____/| .__/ \\__,_|\\___\\___|\n");
  terminal_writestring(
      "                               |_|        OS v2.0   \n");
  terminal_writestring("\n");

  terminal_setcolor(vga_entry_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK));
  terminal_writestring(
      "==========================================================\n\n");

  terminal_writestring("  [");
  terminal_setcolor(vga_entry_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK));
  terminal_writestring("OK");
  terminal_setcolor(vga_entry_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK));
  terminal_writestring("] Memory Manager (Heap)\n");
  kheap_init();

  terminal_writestring("  [");
  terminal_setcolor(vga_entry_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK));
  terminal_writestring("OK");
  terminal_setcolor(vga_entry_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK));
  terminal_writestring("] File System (RAMFS)\n");
  fs_init();

  terminal_writestring("  [");
  terminal_setcolor(vga_entry_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK));
  terminal_writestring("OK");
  terminal_setcolor(vga_entry_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK));
  terminal_writestring("] Interrupts & Timer\n");
  idt_init();
  timer_init(100);
  keyboard_init(); // Inicializar teclado explícitamente
  terminal_writestring("  [");
  terminal_setcolor(vga_entry_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK));
  terminal_writestring("OK");
  terminal_setcolor(vga_entry_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK));
  terminal_writestring("] Keyboard Initialized\n");

  // Force unmask IRQ 0 (Timer) and IRQ 1 (Keyboard) explicitly
  // 0xFC = 1111 1100 (Bit 0=0 enable Timer, Bit 1=0 enable Keyboard)
  outb(0x21, 0xFC);
  outb(0xA1, 0xFF);

  __asm__ volatile("sti");

  shell_run();
}
