global loader
extern kmain

    MAGIC_NUMBER  equ 0x1BADB002
    ALIGN_MODULES equ 0x00000001     
    FLAGS         equ ALIGN_MODULES 
    CHECKSUM      equ -(MAGIC_NUMBER + FLAGS)
    KERNEL_STACK_SIZE equ 4096

    section .bss
    align 4
    kernel_stack:
        resb KERNEL_STACK_SIZE

    section .text
    align 4
        dd MAGIC_NUMBER
        dd FLAGS
        dd CHECKSUM

    loader:
        mov esp, kernel_stack + KERNEL_STACK_SIZE
        push ebx              ; ebx aponta pra multiboot_info_t
        call kmain
    .loop:
        jmp .loop