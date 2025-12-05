#include "spce.h"
#include "fs.h"
#include "io.h"
#include "keyboard.h"
#include "memory.h"
#include "string.h"
#include "terminal.h"

#define INITIAL_CAPACITY 4096
#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define EDITOR_HEIGHT (VGA_HEIGHT - 1) // Status bar

static char *spce_buffer = NULL;
static size_t spce_capacity = 0;
static size_t spce_len = 0;
static size_t spce_cursor = 0;
static bool spce_running = false;
static char spce_filename[32];

// Viewport
static size_t row_offset = 0;
static size_t col_offset = 0;

// Clipboard simple (static buffer for simplicity)
static char clipboard[2048];
static size_t clipboard_len = 0;

// Helpers
static void spce_ensure_capacity(size_t needed) {
  if (spce_capacity >= needed)
    return;
  size_t new_cap = spce_capacity * 2;
  if (new_cap < needed)
    new_cap = needed + 1024;

  char *new_buf = (char *)kmalloc(new_cap);
  if (spce_buffer) {
    kmemcpy(new_buf, spce_buffer, spce_len);
    kfree(spce_buffer);
  }
  spce_buffer = new_buf;
  spce_capacity = new_cap;
}

static void spce_get_cursor_row_col(size_t *row, size_t *col) {
  *row = 0;
  *col = 0;
  for (size_t i = 0; i < spce_cursor; i++) {
    if (spce_buffer[i] == '\n') {
      (*row)++;
      *col = 0;
    } else {
      (*col)++;
    }
  }
}

static void spce_scroll_to_cursor() {
  size_t r, c;
  spce_get_cursor_row_col(&r, &c);

  if (r < row_offset)
    row_offset = r;
  if (r >= row_offset + EDITOR_HEIGHT)
    row_offset = r - EDITOR_HEIGHT + 1;

  if (c < col_offset)
    col_offset = c;
  if (c >= col_offset + VGA_WIDTH)
    col_offset = c - VGA_WIDTH + 1;
}

static void spce_draw_status() {
  size_t r, c;
  spce_get_cursor_row_col(&r, &c);

  // Save terminal state
  size_t old_row = terminal_row;
  size_t old_col = terminal_column;
  uint8_t old_color = terminal_color;

  terminal_row = VGA_HEIGHT - 1;
  terminal_column = 0;
  terminal_setcolor(vga_entry_color(VGA_COLOR_BLACK, VGA_COLOR_LIGHT_GREY));

  for (int i = 0; i < VGA_WIDTH; i++)
    terminal_putchar(' ');
  terminal_column = 0;

  terminal_writestring(" SPCE | ");
  if (spce_filename[0])
    terminal_writestring(spce_filename);
  else
    terminal_writestring("Sin Nombre");

  terminal_writestring(" | ^S:Save ^Q:Quit ^C:Copy ^V:Paste | L:");
  terminal_write_dec(r + 1);
  terminal_writestring(" C:");
  terminal_write_dec(c + 1);

  // Restore
  terminal_row = old_row;
  terminal_column = old_col;
  terminal_setcolor(old_color);

  // Hardware cursor
  uint16_t pos = (r - row_offset) * VGA_WIDTH + (c - col_offset);
  if (r < row_offset || r >= row_offset + EDITOR_HEIGHT || c < col_offset ||
      c >= col_offset + VGA_WIDTH) {
    // Cursor out of view
    pos = 2000; // Hide it
  }
  outb(0x3D4, 0x0F);
  outb(0x3D5, (uint8_t)(pos & 0xFF));
  outb(0x3D4, 0x0E);
  outb(0x3D5, (uint8_t)((pos >> 8) & 0xFF));
}

static void spce_refresh() {
  terminal_clear();

  size_t curr_row = 0;
  size_t curr_col = 0;

  for (size_t i = 0; i < spce_len; i++) {
    char c = spce_buffer[i];

    if (curr_row >= row_offset && curr_row < row_offset + EDITOR_HEIGHT) {
      if (curr_col >= col_offset && curr_col < col_offset + VGA_WIDTH) {
        // Draw char
        terminal_row = curr_row - row_offset;
        terminal_column = curr_col - col_offset;

        // Syntax Highlighting
        if (c >= '0' && c <= '9')
          terminal_setcolor(
              vga_entry_color(VGA_COLOR_LIGHT_MAGENTA, VGA_COLOR_BLACK));
        else if (c == '#' || c == '{' || c == '}' || c == '(' || c == ')' ||
                 c == ';')
          terminal_setcolor(
              vga_entry_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK));
        else if (c == '"' || c == '\'')
          terminal_setcolor(
              vga_entry_color(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK));
        else
          terminal_setcolor(
              vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK));

        if (c != '\n')
          terminal_putchar(c);
      }
    }

    if (c == '\n') {
      curr_row++;
      curr_col = 0;
    } else {
      curr_col++;
    }
  }

  spce_draw_status();
}

static void spce_save() {
  if (!spce_filename[0]) {
    // Prompt for name (simplified)
    kstrcpy(spce_filename, "untitled.txt");
  }

  file_t *f = fs_open(spce_filename);
  if (!f) {
    f = fs_create_file(spce_filename);
    if (!f) {
      terminal_writestring("Error al guardar archivo.\n");
      // Esperar tecla
      kb_getchar();
      return;
    }
  }

  if (f) {
    fs_write(f, spce_buffer, spce_len);
  }
}

static void spce_copy() {
  // Copy current line (simple version)
  // Or copy everything? Let's copy everything for now as selection is hard
  // Better: Copy word under cursor?
  // Let's implement copy-all for simplicity in this iteration
  if (spce_len < 2048) {
    kmemcpy(clipboard, spce_buffer, spce_len);
    clipboard_len = spce_len;
  }
}

static void spce_paste() {
  if (clipboard_len > 0) {
    spce_ensure_capacity(spce_len + clipboard_len);
    // Move
    for (size_t i = spce_len; i > spce_cursor; i--) {
      spce_buffer[i + clipboard_len - 1] = spce_buffer[i - 1];
    }
    // Insert
    for (size_t i = 0; i < clipboard_len; i++) {
      spce_buffer[spce_cursor + i] = clipboard[i];
    }
    spce_len += clipboard_len;
    spce_cursor += clipboard_len;
    spce_scroll_to_cursor();
    spce_refresh();
  }
}

void cmd_spce(const char *arg) {
  if (!spce_buffer) {
    spce_capacity = INITIAL_CAPACITY;
    spce_buffer = (char *)kmalloc(spce_capacity);
  }

  spce_len = 0;
  spce_cursor = 0;
  spce_filename[0] = '\0';
  row_offset = 0;
  col_offset = 0;
  spce_running = true;

  if (arg && arg[0]) {
    kstrcpy(spce_filename, arg);
    file_t *f = fs_open(arg);
    if (f) {
      spce_ensure_capacity(f->size);
      kmemcpy(spce_buffer, f->content, f->size);
      spce_len = f->size;
    }
  } else {
    const char *msg = "// New File\n";
    size_t len = kstrlen(msg);
    kmemcpy(spce_buffer, msg, len);
    spce_len = len;
    spce_cursor = len;
  }

  spce_scroll_to_cursor();
  spce_refresh();

  while (spce_running) {
    unsigned char c = kb_getchar();

    // Ctrl+S (19), Ctrl+Q (17), Ctrl+C (3), Ctrl+V (22)
    // Note: kb_getchar now returns ASCII mapped with Ctrl logic from keyboard.c

    if (c == 17) { // Ctrl+Q -> Quit
      spce_running = false;
    } else if (c == 19) { // Ctrl+S -> Save
      spce_save();
      // Flash status?
    } else if (c == 3) { // Ctrl+C -> Copy
      spce_copy();
    } else if (c == 22) { // Ctrl+V -> Paste
      spce_paste();
    } else if (c == 27) { // ESC -> Quit
      spce_running = false;
    } else if (c == KEY_LEFT) {
      if (spce_cursor > 0)
        spce_cursor--;
    } else if (c == KEY_RIGHT) {
      if (spce_cursor < spce_len)
        spce_cursor++;
    } else if (c == KEY_UP) {
      // Simple up
      size_t curr = spce_cursor;
      size_t col = 0;
      while (curr > 0 && spce_buffer[curr - 1] != '\n') {
        curr--;
        col++;
      }
      if (curr > 0) {
        curr--;
        while (curr > 0 && spce_buffer[curr - 1] != '\n')
          curr--;
        for (size_t i = 0; i < col && spce_buffer[curr] != '\n'; i++)
          curr++;
        spce_cursor = curr;
      }
    } else if (c == KEY_DOWN) {
      // Simple down
      size_t curr = spce_cursor;
      size_t col = 0;
      size_t temp = curr;
      while (temp > 0 && spce_buffer[temp - 1] != '\n') {
        temp--;
        col++;
      }
      while (curr < spce_len && spce_buffer[curr] != '\n')
        curr++;
      if (curr < spce_len) {
        curr++;
        for (size_t i = 0;
             i < col && spce_buffer[curr] != '\n' && curr < spce_len; i++)
          curr++;
        spce_cursor = curr;
      }
    } else if (c == '\b') {
      if (spce_cursor > 0) {
        for (size_t i = spce_cursor; i < spce_len; i++)
          spce_buffer[i - 1] = spce_buffer[i];
        spce_cursor--;
        spce_len--;
      }
    } else if (c >= 32 || c == '\n' || c == '\t') {
      spce_ensure_capacity(spce_len + 1);
      for (size_t i = spce_len; i > spce_cursor; i--)
        spce_buffer[i] = spce_buffer[i - 1];
      spce_buffer[spce_cursor] = c;
      spce_cursor++;
      spce_len++;
    }

    spce_scroll_to_cursor();
    spce_refresh();
  }

  terminal_clear();
  terminal_setcolor(vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK));
}
