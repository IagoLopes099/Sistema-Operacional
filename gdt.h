#ifndef _INCLUDE_GDT_H_
#define _INCLUDE_GDT_H_

/** gdt_install:
 *  Monta a GDT (null, kernel code, kernel data) e a carrega no processador.
 */
void gdt_install(void);

#endif