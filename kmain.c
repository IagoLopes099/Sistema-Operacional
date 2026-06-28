#include "fb.h"


void clear_screen() {
    char *fb = (char *)0xB8000;

    for (int i = 0; i < 80 * 25; i++) {
        fb[i * 2] = ' ';
        fb[i * 2 + 1] = 0x0F;
    }
}

void kmain() {
    clear_screen();
    char *msg = "Hello, OS World!";
    fb_write(msg, 16);
}
