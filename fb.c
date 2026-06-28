#include "io.h"

#define FB_ADDRESS       0x000B8000
#define FB_COMMAND_PORT  0x3D4
#define FB_DATA_PORT     0x3D5
#define FB_HIGH_BYTE_CMD 14
#define FB_LOW_BYTE_CMD  15

static char *fb = (char *) FB_ADDRESS;

void fb_write_cell(unsigned int i, char c, unsigned char fg, unsigned char bg) {
    fb[i * 2]     = c;
    fb[i * 2 + 1] = ((fg & 0x0F) << 4) | (bg & 0x0F);
}

void fb_move_cursor(unsigned short pos) {
    outb(FB_COMMAND_PORT, FB_HIGH_BYTE_CMD);
    outb(FB_DATA_PORT,    (pos >> 8) & 0x00FF);
    outb(FB_COMMAND_PORT, FB_LOW_BYTE_CMD);
    outb(FB_DATA_PORT,    pos & 0x00FF);
}

void fb_write(char *buf, unsigned int len) {
    static unsigned int pos = 0;
    for (unsigned int i = 0; i < len; i++) {
        fb_write_cell(pos, buf[i], 15, 0); // verde sobre preto
        pos++;
        fb_move_cursor(pos);
    }
}
