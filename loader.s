global loader
extern kmain

    MAGIC_NUMBER        equ 0x1BADB002
    ALIGN_MODULES       equ 0x00000001
    FLAGS               equ ALIGN_MODULES
    CHECKSUM            equ -(MAGIC_NUMBER + FLAGS)
    KERNEL_STACK_SIZE   equ 4096

    ; onde o kernel enxerga a si mesmo na memoria virtual (higher half) -
    ; tem que bater com o valor usado em link.ld e em paging.c
    KERNEL_VIRTUAL_BASE equ 0xC0000000
    KERNEL_PDE_INDEX    equ (KERNEL_VIRTUAL_BASE >> 22)   ; = 768

; ---------------------------------------------------------------------------
; Estagio de boot: NAO relocado (roda com paginacao desligada, entao os
; enderecos usados aqui tem que ser os mesmos enderecos fisicos reais).
; ---------------------------------------------------------------------------
section .text.boot
align 4
    dd MAGIC_NUMBER
    dd FLAGS
    dd CHECKSUM

; Page directory temporario: serve so pra ligar a paginacao e conseguir
; pular pro codigo do kernel ja rodando na higher half. Usa paginas de
; 4MB (PSE), do mesmo jeito que paging.c ja fazia no identity paging do
; cap. 9.1/9.2 - assim nao precisamos de page tables separadas aqui.
align 0x1000
boot_page_directory:
    ; entrada 0: identity map do primeiro 4MB fisico. Precisa disso porque,
    ; ate o "jmp" pra higher_half, o eip ainda esta em endereco baixo.
    dd 0x00000083                        ; present | rw | 4MB, fisico 0x00000000
    times (KERNEL_PDE_INDEX - 1) dd 0
    ; entrada 768 (indice de 0xC0000000): mapeia a higher half pro MESMO
    ; frame fisico de 4MB onde o GRUB carregou o kernel.
    dd 0x00000083                        ; present | rw | 4MB, fisico 0x00000000
    times (1024 - KERNEL_PDE_INDEX - 1) dd 0

loader:
    mov ecx, boot_page_directory   ; endereco fisico (esta secao nao e relocada)
    mov cr3, ecx

    mov ecx, cr4
    or  ecx, 0x00000010            ; bit 4 = PSE (paginas de 4MB)
    mov cr4, ecx

    mov ecx, cr0
    or  ecx, 0x80000000            ; bit 31 = PG (liga a paginacao)
    mov cr0, ecx

    ; a partir de agora enderecos >= 0xC0000000 ja sao traduzidos
    ; corretamente pela MMU, entao podemos pular pro codigo relocado
    lea ecx, [higher_half]
    jmp ecx

; ---------------------------------------------------------------------------
; A partir daqui, codigo relocado (linkado em 0xC0100000+, a higher half)
; ---------------------------------------------------------------------------
section .text
align 4
higher_half:
    mov esp, kernel_stack + KERNEL_STACK_SIZE
    push ebx              ; ebx aponta pra multiboot_info_t
    call kmain
.loop:
    jmp .loop

section .bss
align 4
kernel_stack:
    resb KERNEL_STACK_SIZE
