# MyOS

## Descrição do Projeto

O MyOS é um sistema operacional educacional desenvolvido como parte da disciplina de Sistemas Operacionais. O objetivo do projeto é compreender os conceitos fundamentais do funcionamento de um sistema operacional por meio da implementação prática de seus componentes básicos.

O desenvolvimento utiliza como principal referência o livro *The Little Book About OS Development*, que serve como base teórica e prática para a construção do sistema.

O projeto é desenvolvido em linguagem C, com trechos em Assembly quando necessário, sendo executado em ambiente virtual para testes e validação.

## Objetivos

* Compreender o processo de inicialização de um sistema operacional.
* Desenvolver um kernel simples.
* Estudar a interação entre hardware e software em baixo nível.
* Implementar mecanismos fundamentais de um sistema operacional.
* Aplicar os conceitos estudados na disciplina de Sistemas Operacionais.

---

## Imagem da Interface

O sistema atualmente possui uma interface em modo texto executada durante a inicialização do sistema operacional.

> Inserir aqui uma captura de tela do sistema em execução.

```markdown
![Interface do MyOS](images/myos.png)
```

---

## Dependências

Para compilar e executar o projeto são necessários:

* GCC
* NASM
* GRUB
* xorriso
* QEMU

No Ubuntu ou Debian:

```bash
sudo apt update
sudo apt install build-essential nasm grub-pc-bin xorriso qemu-system-x86
```

---

## Compilação

Para compilar o projeto:

```bash
make
```

Caso o projeto não utilize Makefile, os comandos podem variar conforme a estrutura dos arquivos.

---

## Execução

Para gerar a imagem ISO:

```bash
grub-mkrescue -o os.iso iso
```

Para executar no QEMU:

```bash
qemu-system-i386 -cdrom os.iso
```

---

## Instruções de Uso

1. Compile o projeto.
2. Gere a imagem ISO.
3. Execute o sistema utilizando o QEMU.
4. Aguarde o carregamento do kernel.
5. Observe e teste as funcionalidades implementadas durante o desenvolvimento.

---

## Divisão das Atividades

| Integrante | Responsabilidades                                                                                                                                   |
| ---------- | --------------------------------------------------------------------------------------------------------------------------------------------------- |
| Aluno 1    | Configuração do ambiente, implementação do código, compilação e execução dos testes do sistema.                                                     |
| Aluno 2    | Pesquisa de soluções para erros encontrados, auxílio na depuração, validação das etapas do desenvolvimento e elaboração da documentação do projeto. |

---

## Referências

* The Little Book About OS Development.
* Material da disciplina de Sistemas Operacionais.
* Documentação do GCC, NASM, GRUB e QEMU.
