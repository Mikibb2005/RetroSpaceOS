#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "types.h"

// Códigos especiales
#define KEY_UP 200
#define KEY_DOWN 201
#define KEY_LEFT 202
#define KEY_RIGHT 203
#define KEY_PGUP 204
#define KEY_PGDN 205
#define KEY_HOME 206
#define KEY_END 207

void keyboard_handler(void);
void keyboard_init(void);
unsigned char kb_getchar(void);
bool kb_has_char(void);

#endif
