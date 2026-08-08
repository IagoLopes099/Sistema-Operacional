#include "fb.h"
#include "serial.h"
#include "gdt.h"
#include "idt.h"
#include "pic.h"
#include "interrupt.h"
#include "paging.h"
#include "multiboot.h"
#include "pfa.h"
#include "kheap.h"
#include "user.h"

#define SERIAL_COM1_BASE 0x3F8

/* implementadas mais abaixo - so servem pra transformar numeros em texto
 * pra mandar pela porta serial, nao tem nada de especial do cap. 10 */
static void serial_write_string(const char *s);
static void serial_write_uint(unsigned int n);

/** teste_page_frame_allocator:
 *  Exercita o allocator de frames (cap. 10.1/10.2) e o heap do kernel
 *  (cap. 10.3): mostra quanta memoria tem disponivel, aloca alguns
 *  blocos, escreve neles, libera um e aloca de novo pra provar que o
 *  espaco foi reaproveitado.
 */
static void teste_page_frame_allocator(void)
{
    void *a, *b, *c;

    serial_write_string("PFA: frames livres = ");
    serial_write_uint(pfa_free_frame_count());
    serial_write_string(" / ");
    serial_write_uint(pfa_total_frame_count());
    serial_write_string("\n");

    a = kmalloc(64);
    b = kmalloc(128);
    serial_write_string("kmalloc(64)  = 0x");
    serial_write_uint((unsigned int) a);
    serial_write_string("\nkmalloc(128) = 0x");
    serial_write_uint((unsigned int) b);
    serial_write_string("\n");

    if (a != 0) {
        *(char *) a = 'K'; /* escreve no bloco, prova que o ponteiro e valido */
    }

    kfree(a);
    c = kmalloc(32);
    serial_write_string("kfree(a) e kmalloc(32) reaproveitou o espaco? ");
    serial_write_string((c == a) ? "sim\n" : "nao (tudo bem, tambem e valido)\n");

    kfree(b);
    kfree(c);

    serial_write_string("PFA: frames livres depois dos testes = ");
    serial_write_uint(pfa_free_frame_count());
    serial_write_string("\n");
}

void kmain(unsigned int ebx)
{
    multiboot_info_t *mbinfo = (multiboot_info_t *) ebx;

    gdt_install();
    idt_install();
    paging_install();
    pic_remap();

    serial_configure_baud_rate(SERIAL_COM1_BASE, 2);
    serial_configure_line(SERIAL_COM1_BASE);
    serial_configure_buffer(SERIAL_COM1_BASE);
    serial_configure_modem(SERIAL_COM1_BASE);

    serial_write("Kernel inicializado (GDT, IDT, paginacao, PIC ok)\n", 51);

    pfa_init(mbinfo);
    teste_page_frame_allocator();

    fb_write("Hello world! Digite algo:\n", 27);

    enable_interrupts();

    /* cap. 11: prepara o processo de usuario (a partir do modulo do GRUB,
     * infraestrutura do cap. 7) e salta pra modo usuario (anel 3). Se der
     * certo essa chamada NUNCA retorna - o resto do kernel (o loop de
     * hlt abaixo, que mantem o teclado responsivo) vira uma rede de
     * seguranca caso o modulo nao seja encontrado. */
    serial_write("Cap 11: preparando processo de modo usuario...\n", 48);
    if (user_mode_start(mbinfo) != 0) {
        serial_write("Cap 11: nao foi possivel entrar em modo usuario.\n", 50);
    }

    while (1) {
        __asm__ volatile ("hlt"); /* dorme ate a proxima interrupcao chegar */
    }
}

/* --- so utilitarios pra imprimir o resultado dos testes acima --- */

static void serial_write_string(const char *s)
{
    unsigned int len = 0;
    while (s[len] != '\0') {
        len++;
    }
    serial_write((char *) s, len);
}

static void serial_write_uint(unsigned int n)
{
    /* 8 digitos hexadecimais + \0 - imprime em hex pra ficar do tamanho
     * de um endereco/ponteiro de 32 bits */
    char buf[9];
    int i;
    const char *hex_digits = "0123456789ABCDEF";

    for (i = 7; i >= 0; i--) {
        buf[i] = hex_digits[n & 0xF];
        n >>= 4;
    }
    buf[8] = '\0';

    serial_write_string(buf);
}