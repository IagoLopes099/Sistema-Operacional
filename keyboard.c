#include "io.h"
#include "keyboard.h"

#define KBD_DATA_PORT 0x60
#define KBD_STATUS_PORT 0x64

#define LEFT_SHIFT_PRESS    0x2A
#define LEFT_SHIFT_RELEASE  0xAA
#define RIGHT_SHIFT_PRESS   0x36
#define RIGHT_SHIFT_RELEASE 0xB6
#define ABNT2_EXTRA_KEY     0x56

#define KBD_BUFFER_SIZE 128

static char buffer[KBD_BUFFER_SIZE];
static unsigned int read_pos = 0;
static unsigned int write_pos = 0;
static int shift_pressed = 0;

static const char scan_code_to_ascii[] = {
    0, 27, '1','2','3','4','5','6','7','8','9','0','-','=', '\b',
    '\t','q','w','e','r','t','y','u','i','o','p','[',']','\n',
    0,'a','s','d','f','g','h','j','k','l',';','\'','`',
    0,'\\','z','x','c','v','b','n','m',',','.','/',0,'*',0,' '
};

static const char scan_code_to_ascii_shift[] = {
    0, 27, '!','@','#','$','%','^','&','*','(',')','_','+', '\b',
    '\t','Q','W','E','R','T','Y','U','I','O','P','{','}','\n',
    0,'A','S','D','F','G','H','J','K','L',':','"','~',
    0,'|','Z','X','C','V','B','N','M','<','>','?',0,'*',0,' '
};

static void enqueue(char c) {
    unsigned int next = (write_pos + 1) % KBD_BUFFER_SIZE;
    if (next == read_pos) return; /* drop newest on overflow */
    buffer[write_pos] = c;
    write_pos = next;
}

void keyboard_handler(void) {
    unsigned char scancode = inb(KBD_DATA_PORT);

    if (scancode == LEFT_SHIFT_PRESS || scancode == RIGHT_SHIFT_PRESS) {
        shift_pressed = 1;
        return;
    }
    if (scancode == LEFT_SHIFT_RELEASE || scancode == RIGHT_SHIFT_RELEASE) {
        shift_pressed = 0;
        return;
    }
    if (scancode & 0x80) return;

    if (scancode == ABNT2_EXTRA_KEY) {
        enqueue(shift_pressed ? '|' : '\\');
        return;
    }

    if (scancode < sizeof(scan_code_to_ascii)) {
        char c = shift_pressed
                   ? scan_code_to_ascii_shift[scancode]
                   : scan_code_to_ascii[scancode];
        if (c) enqueue(c);
    }
}

int keyboard_has_char(void) {
    return read_pos != write_pos;
}

u8int keyboard_read(void) {
    char c;
    if (!keyboard_has_char()) return 0;
    c = buffer[read_pos];
    read_pos = (read_pos + 1) % KBD_BUFFER_SIZE;
    return (u8int)c;
}
