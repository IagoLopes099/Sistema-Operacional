#ifndef INCLUDE_KEYBOARD_H
#define INCLUDE_KEYBOARD_H

#include "types.h"

void keyboard_handler(void);
u8int keyboard_read(void);
int keyboard_has_char(void);

#endif