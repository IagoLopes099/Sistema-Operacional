global load_gdt

; load_gdt - carrega a GDT e recarrega os registradores de segmento
; stack: [esp + 4] endereço da struct gdt_ptr (limit + base)
;        [esp    ] endereço de retorno
load_gdt:
    mov eax, [esp + 4]
    lgdt [eax]              ; carrega a GDT de verdade

    mov ax, 0x10             ; 0x10 = seletor do kernel data segment
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    jmp 0x08:.flush_cs       ; far jump pra recarregar cs (0x08 = kernel code segment)
.flush_cs:
    ret