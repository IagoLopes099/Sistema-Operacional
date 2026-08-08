#ifndef _INCLUDE_PAGING_H
#define _INCLUDE_PAGING_H

void paging_install(void);

/** paging_allow_user_access:
 *  Libera acesso em PL3 (modo usuario) para a regiao de 4MB do page
 *  directory que contem o endereco virtual dado. Ver cap. 11.2.
 *
 *  @param virtual_addr Endereco virtual dentro da regiao a liberar
 */
void paging_allow_user_access(unsigned int virtual_addr);

#endif