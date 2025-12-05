#include "keyboard.h"
#include "io.h"
#include "terminal.h"

#define KB_BUF_SIZE 256
static unsigned char kb_buffer[KB_BUF_SIZE];
static volatile uint32_t kb_head = 0;
static volatile uint32_t kb_tail = 0;
static volatile bool shift_pressed = false;
static volatile bool extended_key = false;
static volatile bool ctrl_pressed = false;

static const char scancode_ascii[128] = {
    0,   27,  '1',  '2',  '3',  '4', '5', '6',  '7', '8', '9', '0',
    '-', '=', '\b', '\t', 'q',  'w', 'e', 'r',  't', 'y', 'u', 'i',
    'o', 'p', '[',  ']',  '\n', 0,   'a', 's',  'd', 'f', 'g', 'h',
    'j', 'k', 'l',  ';',  '\'', '`', 0,   '\\', 'z', 'x', 'c', 'v',
    'b', 'n', 'm',  ',',  '.',  '/', 0,   '*',  0,   ' ', 0};

static const char scancode_shift[128] = {
    0,   27,  '!',  '@',  '#',  '$', '%', '^', '&', '*', '(', ')',
    '_', '+', '\b', '\t', 'Q',  'W', 'E', 'R', 'T', 'Y', 'U', 'I',
    'O', 'P', '{',  '}',  '\n', 0,   'A', 'S', 'D', 'F', 'G', 'H',
    'J', 'K', 'L',  ':',  '"',  '~', 0,   '|', 'Z', 'X', 'C', 'V',
    'B', 'N', 'M',  '<',  '>',  '?', 0,   '*', 0,   ' ', 0};

void keyboard_init(void) {
  // Vaciar buffer de entrada
  while (inb(0x64) & 1)
    inb(0x60);

  // Habilitar teclado
  outb(0x64, 0xAE);
}

void keyboard_handler(void) {
  uint8_t sc = inb(0x60);

  if (sc == 0xE0) {
    extended_key = true;
    return;
  }

  // Key Release
  if (sc & 0x80) {
    sc &= 0x7F;
    if (sc == 0x2A || sc == 0x36)
      shift_pressed = false;
    if (sc == 0x1D)
      ctrl_pressed = false;
    extended_key = false;
    return;
  }

  // Key Press
  if (sc == 0x2A || sc == 0x36) {
    shift_pressed = true;
    return;
  }
  if (sc == 0x1D) {
    ctrl_pressed = true;
    return;
  }

  unsigned char c = 0;

  if (extended_key) {
    switch (sc) {
    case 0x48:
      c = KEY_UP;
      break;
    case 0x50:
      c = KEY_DOWN;
      break;
    case 0x4B:
      c = KEY_LEFT;
      break;
    case 0x4D:
      c = KEY_RIGHT;
      break;
    case 0x49:
      c = KEY_PGUP;
      break;
    case 0x51:
      c = KEY_PGDN;
      break;
    case 0x47:
      c = KEY_HOME;
      break;
    case 0x4F:
      c = KEY_END;
      break;
    }
    extended_key = false;
  } else {
    c = shift_pressed ? scancode_shift[sc] : scancode_ascii[sc];

    // Mapear Ctrl+Letra a códigos ASCII 1-26
    if (ctrl_pressed) {
      if (c >= 'a' && c <= 'z')
        c = c - 'a' + 1;
      else if (c >= 'A' && c <= 'Z')
        c = c - 'A' + 1;
    }
  }

  if (c) {
    uint32_t next = (kb_head + 1) % KB_BUF_SIZE;
    if (next != kb_tail) {
      kb_buffer[kb_head] = c;
      kb_head = next;
    }
  }
}

bool kb_has_char(void) { return kb_head != kb_tail; }

unsigned char kb_getchar(void) {
  while (!kb_has_char())
    __asm__ volatile("hlt");
  unsigned char c = kb_buffer[kb_tail];
  kb_tail = (kb_tail + 1) % KB_BUF_SIZE;
  return c;
}
