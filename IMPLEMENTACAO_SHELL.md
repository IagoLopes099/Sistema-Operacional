# MyOS2 - Mini-shell e processos cooperativos

Implementação baseada no estado do projeto até o capítulo 11.

## Recursos adicionados

- Mini-shell interativo no framebuffer.
- Buffer de teclado alimentado pela IRQ1.
- PCB com PID, PPID, estado, stack e contador de execução.
- Criação de processos filhos.
- Escalonamento round-robin cooperativo.
- Troca de contexto por `process_switch.s`.
- `worker` em foreground/background.
- Sintaxe `comando &`.
- `exit` e estado `ZOMBIE`.
- Reap de filhos pelo processo pai.
- `ps`.
- `top` com atualização periódica baseada no PIT e saída por `q`.
- PIT em 100 Hz para ticks de sistema e medição de CPU por processo.
- Inicialização correta do page-frame allocator antes das stacks dos processos.

## Build

No ambiente do projeto:

    make clean
    make
    make run

O build requer as mesmas ferramentas do projeto original, incluindo NASM, GCC multilib, GNU ld, genisoimage e QEMU.

## Arquivos alterados

- Makefile
- kmain.c
- process.c
- process.h
- shell.c
- shell.h
- keyboard.c
- fb.h
- iso/boot/grub/menu.lst

## Arquivo novo

- process_switch.s
- timer.c / timer.h

Os arquivos de user mode do capítulo 11 foram preservados, mas o mini-shell/process manager desta etapa utiliza processos cooperativos no kernel; eles não são incluídos no Makefile desta versão.
