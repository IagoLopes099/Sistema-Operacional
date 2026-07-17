#include "io.h"
#include "fb.h"

#define FB_ADDRESS              0x000B8000
#define FB_COMMAND_PORT         0x3D4
#define FB_DATA_PORT            0x3D5
#define FB_HIGH_BYTE_COMMAND    14
#define FB_LOW_BYTE_COMMAND     15

#define FB_BACKGROUND     0 // fundo preto
#define FB_LETTER 15 // letra 

char *fb = (char *) FB_ADDRESS;

void fb_move_cursor(unsigned short pos)
{
	outb(FB_COMMAND_PORT, FB_HIGH_BYTE_COMMAND);
	outb(FB_DATA_PORT,    ((pos >> 8) & 0x00FF));
	outb(FB_COMMAND_PORT, FB_LOW_BYTE_COMMAND);
	outb(FB_DATA_PORT,    pos & 0x00FF);
}


void fb_write_cell(unsigned int i, char c, unsigned char fg, unsigned char bg)
{
	fb[i] = c;
	fb[i + 1] = ((fg & 0x0F) << 4) | (bg & 0x0F);
}


#define FB_COLUMNS 80
#define FB_ROWS    25

/* Guarda a posição (em células) de onde a próxima escrita deve começar.
 * Sem isso, cada chamada de write() reescreveria a partir da célula 0. */
static unsigned int fb_position = 0;

int fb_write(char *buf, unsigned int len) // funcao para escrever na tela
{
    unsigned int i;
    for (i = 0; i < len; i++) {
        if (buf[i] == '\n') {
            fb_position = (fb_position / FB_COLUMNS + 1) * FB_COLUMNS;
            continue;
        }
        if (buf[i] == '\b') {
            if (fb_position > 0) {
                fb_position--;
                fb_write_cell(fb_position * 2, ' ', FB_BACKGROUND, FB_LETTER);
            }
            continue;
        }

        fb_write_cell(fb_position * 2, buf[i], FB_BACKGROUND, FB_LETTER);
        fb_position++;
    }

    if (fb_position >= FB_COLUMNS * FB_ROWS) {
        fb_position = 0; // TODO: implementar scroll de verdade no lugar disso
    }

    fb_move_cursor(fb_position);
    return i;
}