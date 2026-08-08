#ifndef _INCLUDE_USER_H_
#define _INCLUDE_USER_H_

#include "multiboot.h"

/** user_mode_start:
 *  Implementa o cap. 11 (User Mode) do little book: copia o programa que
 *  o GRUB carregou como modulo (infraestrutura do cap. 7) para uma regiao
 *  de memoria acessivel em PL3, monta uma pilha de usuario e salta pra
 *  esse programa em modo usuario (anel 3) via iret (cap. 11.3).
 *
 *  Interrupcoes ficam desligadas no modo usuario por enquanto - habilitar
 *  interrupcoes inter-privilegio de volta pro kernel exige uma TSS, que so
 *  e configurada no cap. 13 (System Calls).
 *
 *  @param  mbinfo A struct multiboot recebida em kmain
 *  @return -1 se nao foi possivel preparar o processo de usuario (module
 *          do GRUB ausente ou grande demais); em caso de sucesso essa
 *          funcao NUNCA RETORNA.
 */
int user_mode_start(multiboot_info_t *mbinfo);

#endif
