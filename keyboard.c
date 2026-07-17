#include "io.h"
#include "fb.h"
#include "keyboard.h"

#define KBD_DATA_PORT 0x60

#define LEFT_SHIFT_PRESS    0x2A
#define LEFT_SHIFT_RELEASE  0xAA
#define RIGHT_SHIFT_PRESS   0x36
#define RIGHT_SHIFT_RELEASE 0xB6

/* Tecla extra do teclado ABNT2 (o "102nd key" do layout ISO),
 * fica entre o Shift esquerdo e o Z. No ABNT2 produz "\" / "|".
 * Se a tecla do "/" "?" "°" te der um scan code diferente disso,
 * troque o valor abaixo (veja a dica de debug no final do arquivo).
 */
#define ABNT2_EXTRA_KEY 0x56

static int shift_pressed = 0;

/* Tabela sem Shift (scan code -> ASCII) */
static char scan_code_to_ascii[] = {
    0,    27,  '1', '2', '3', '4', '5', '6', '7', '8',
    '9',  '0', '-', '=', '\b',
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0,    'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
    0,    '\\','z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0,
    '*',  0,   ' '
};

/* Tabela com Shift pressionado */
static char scan_code_to_ascii_shift[] = {
    0,    27,  '!', '@', '#', '$', '%', '\174', '&', '*',
    '(',  ')', '_', '+', '\b',
    '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',
    0,    'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~',
    0,    '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0,
    '*',  0,   ' '
};

static unsigned char read_scan_code(void)
{
    return inb(KBD_DATA_PORT);
}

/** keyboard_handler:
 *  Chamada pela interrupt_handler toda vez que o teclado gera uma IRQ.
 */
void keyboard_handler(void)
{
    unsigned char scan_code = read_scan_code();

    /* teclas modificadoras: atualiza o estado, mas nao imprime nada */
    switch (scan_code) {
        case LEFT_SHIFT_PRESS:
        case RIGHT_SHIFT_PRESS:
            shift_pressed = 1;
            return;
        case LEFT_SHIFT_RELEASE:
        case RIGHT_SHIFT_RELEASE:
            shift_pressed = 0;
            return;
        default:
            break;
    }

    /* bit 7 = 1 significa "tecla solta" (break code); ignoramos por ora */
    if (scan_code & 0x80) {
        return;
    }

    /* tecla extra do ABNT2 (fora do range da tabela padrao de 58 posicoes) */
    if (scan_code == ABNT2_EXTRA_KEY) {
        char c = shift_pressed ? '|' : '\\';
        fb_write(&c, 1);
        return;
    }

    if (scan_code < sizeof(scan_code_to_ascii)) {
        char c = shift_pressed
                     ? scan_code_to_ascii_shift[scan_code]
                     : scan_code_to_ascii[scan_code];
        if (c != 0) {
            fb_write(&c, 1);
        }
    }
}