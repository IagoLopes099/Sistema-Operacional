#include "fb.h"

void kmain(void) {
    char *msg = "Hello, OS World!";
    fb_write(msg, 16);
}