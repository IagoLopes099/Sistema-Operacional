global enter_user_mode

; enter_user_mode - monta a pilha exatamente como o processador monta
; quando uma interrupcao inter-privilegio ocorre, e entao executa iret
; pra saltar pro modo usuario (cap. 11.3 do little book).
;
; stack: [esp + 16] eip do codigo de usuario (onde comecar a executar)
;        [esp + 12] esp da pilha de usuario (topo da pilha em PL3)
;        [esp +  8] seletor do codigo de usuario (ja com RPL = 3)
;        [esp +  4] seletor dos dados de usuario (ja com RPL = 3)
;        [esp     ] endereco de retorno
enter_user_mode:
    mov eax, [esp + 4]      ; data selector
    mov ecx, [esp + 8]      ; code selector
    mov edx, [esp + 12]     ; user esp
    mov ebx, [esp + 16]     ; user eip

    ; ds/es/fs/gs usam o data segment de usuario a partir de agora
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    ; monta a pilha que o iret vai consumir:
    ; [esp+16] ss  [esp+12] esp  [esp+8] eflags  [esp+4] cs  [esp+0] eip
    push eax                 ; ss = data selector
    push edx                 ; esp = topo da pilha de usuario

    pushfd
    pop eax
    and eax, 0xFFFFFDFF      ; desliga a flag IF (bit 9): interrupcoes
                              ; ficam desligadas em modo usuario por
                              ; enquanto - reabilitar isso direito exige
                              ; uma TSS (cap. 13, System Calls)
    push eax                 ; eflags

    push ecx                 ; cs = code selector
    push ebx                 ; eip = ponto de entrada do programa

    iret                     ; salta pra PL3 - nunca retorna daqui
