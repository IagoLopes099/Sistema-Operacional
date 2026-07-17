global load_idt
global enable_interrupts
global interrupt_handler_table

extern interrupt_handler   ; definida em interrupt.c

; load_idt - carrega a IDT no processador
; stack: [esp+4] endereço da struct idt_ptr
load_idt:
    mov eax, [esp + 4]
    lidt [eax]
    ret

; enable_interrupts - habilita interrupções (equivalente a sti em C)
enable_interrupts:
    sti
    ret

%macro no_error_code_interrupt_handler 1
global interrupt_handler_%1
interrupt_handler_%1:
    push dword 0             ; erro fake, pra manter o formato igual
    push dword %1            ; número da interrupção
    jmp  common_interrupt_handler
%endmacro

%macro error_code_interrupt_handler 1
global interrupt_handler_%1
interrupt_handler_%1:
    push dword %1             ; a CPU já empilhou o erro de verdade
    jmp  common_interrupt_handler
%endmacro

common_interrupt_handler:
    pusha                     ; salva eax,ecx,edx,ebx,esp,ebp,esi,edi
    push ds
    push es
    push fs
    push gs

    mov ax, 0x10              ; usa o kernel data segment enquanto trata a interrupção
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    ; a esta altura: pusha (32 bytes) + 4 segmentos (16 bytes) = 48 bytes
    ; empilhados depois do numero da interrupcao. Buscamos ele de volta:
    mov eax, [esp + 48]
    push eax
    call interrupt_handler
    add esp, 4                ; cdecl: quem chama limpa o argumento

    pop gs
    pop fs
    pop es
    pop ds
    popa
    add esp, 8                ; remove interrupt number + error code
    iret

; --- exceções da CPU (0 - 31) ---
no_error_code_interrupt_handler 0
no_error_code_interrupt_handler 1
no_error_code_interrupt_handler 2
no_error_code_interrupt_handler 3
no_error_code_interrupt_handler 4
no_error_code_interrupt_handler 5
no_error_code_interrupt_handler 6
no_error_code_interrupt_handler 7
error_code_interrupt_handler    8
no_error_code_interrupt_handler 9
error_code_interrupt_handler    10
error_code_interrupt_handler    11
error_code_interrupt_handler    12
error_code_interrupt_handler    13
error_code_interrupt_handler    14
no_error_code_interrupt_handler 15
no_error_code_interrupt_handler 16
error_code_interrupt_handler    17
no_error_code_interrupt_handler 18
no_error_code_interrupt_handler 19
no_error_code_interrupt_handler 20
no_error_code_interrupt_handler 21
no_error_code_interrupt_handler 22
no_error_code_interrupt_handler 23
no_error_code_interrupt_handler 24
no_error_code_interrupt_handler 25
no_error_code_interrupt_handler 26
no_error_code_interrupt_handler 27
no_error_code_interrupt_handler 28
no_error_code_interrupt_handler 29
no_error_code_interrupt_handler 30
no_error_code_interrupt_handler 31

; --- IRQs de hardware remapeadas (32 - 47), teclado = 33 (0x21) ---
no_error_code_interrupt_handler 32
no_error_code_interrupt_handler 33
no_error_code_interrupt_handler 34
no_error_code_interrupt_handler 35
no_error_code_interrupt_handler 36
no_error_code_interrupt_handler 37
no_error_code_interrupt_handler 38
no_error_code_interrupt_handler 39
no_error_code_interrupt_handler 40
no_error_code_interrupt_handler 41
no_error_code_interrupt_handler 42
no_error_code_interrupt_handler 43
no_error_code_interrupt_handler 44
no_error_code_interrupt_handler 45
no_error_code_interrupt_handler 46
no_error_code_interrupt_handler 47

section .data
align 4
interrupt_handler_table:
    dd interrupt_handler_0,  interrupt_handler_1,  interrupt_handler_2,  interrupt_handler_3
    dd interrupt_handler_4,  interrupt_handler_5,  interrupt_handler_6,  interrupt_handler_7
    dd interrupt_handler_8,  interrupt_handler_9,  interrupt_handler_10, interrupt_handler_11
    dd interrupt_handler_12, interrupt_handler_13, interrupt_handler_14, interrupt_handler_15
    dd interrupt_handler_16, interrupt_handler_17, interrupt_handler_18, interrupt_handler_19
    dd interrupt_handler_20, interrupt_handler_21, interrupt_handler_22, interrupt_handler_23
    dd interrupt_handler_24, interrupt_handler_25, interrupt_handler_26, interrupt_handler_27
    dd interrupt_handler_28, interrupt_handler_29, interrupt_handler_30, interrupt_handler_31
    dd interrupt_handler_32, interrupt_handler_33, interrupt_handler_34, interrupt_handler_35
    dd interrupt_handler_36, interrupt_handler_37, interrupt_handler_38, interrupt_handler_39
    dd interrupt_handler_40, interrupt_handler_41, interrupt_handler_42, interrupt_handler_43
    dd interrupt_handler_44, interrupt_handler_45, interrupt_handler_46, interrupt_handler_47