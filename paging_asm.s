global load_page_directory
global enable_paging

; load_page_directory - carrega o endereco fisico do page directory em cr3
; stack: [esp + 4] endereco do page directory
;        [esp    ] endereco de retorno
load_page_directory:
    mov eax, [esp + 4]
    mov cr3, eax
    ret

; enable_paging - liga PSE (paginas de 4MB) em cr4 e PG (paginacao) em cr0
enable_paging:
    mov eax, cr4
    or  eax, 0x00000010    ; bit 4 = PSE
    mov cr4, eax

    mov eax, cr0
    or  eax, 0x80000000    ; bit 31 = PG
    mov cr0, eax

    ret