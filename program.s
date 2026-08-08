[BITS 32]
    ; cap. 11: este programa roda em modo usuario (anel 3). Coloca um
    ; valor bem distinguivel em eax pra confirmar, via "info registers"
    ; no monitor do QEMU, que o codigo realmente executou.
    mov eax, 0xFACEFEED

    ; entra em loop infinito - sem chamadas de sistema (cap. 13) ainda
    ; nao ha como voltar pro kernel, entao e aqui que o teste termina.
    jmp $
