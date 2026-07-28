# MyOS

## Descrição do Projeto

O MyOS é um sistema operacional educacional desenvolvido como parte da disciplina de Sistemas Operacionais da Universidade Federal da Paraíba (UFPB). O projeto tem como objetivo compreender os conceitos fundamentais do funcionamento de um sistema operacional por meio da implementação prática de seus componentes básicos.

O desenvolvimento utiliza como principal referência o livro *The Little Book About OS Development*, que serve como base teórica e prática para a construção do sistema. Ao longo do desenvolvimento, foram realizadas adaptações e pesquisas para solucionar incompatibilidades e erros encontrados em relação ao ambiente utilizado.

O projeto é desenvolvido em linguagem C, com trechos em Assembly quando necessário, sendo compilado e executado por meio do emulador QEMU.

---

## Objetivos

- Compreender o processo de inicialização de um sistema operacional.
- Desenvolver um kernel simples.
- Estudar a interação entre hardware e software em baixo nível.
- Implementar mecanismos fundamentais de um sistema operacional.
- Aplicar, na prática, os conceitos estudados na disciplina de Sistemas Operacionais.

---

## Interface

O sistema possui uma interface em modo texto, exibida durante a inicialização e execução do kernel.

> **Adicionar aqui uma captura de tela do sistema em execução.**

Exemplo:

```text
docs/images/myos.png
```

---

## Dependências

Para compilar e executar o projeto, é necessário possuir:

- GCC
- NASM
- GRUB
- Make
- QEMU

### Instalação (Ubuntu/Debian)

```bash
sudo apt update
sudo apt install build-essential nasm grub-pc-bin qemu-system-x86 make
```

---

## Como Compilar

Na pasta do projeto, execute:

```bash
make
```

Ao final da compilação, será gerada a imagem ISO do sistema operacional.

---

## Como Executar

Para iniciar o sistema no QEMU, execute:

```bash
make run
```

---

## Instruções de Uso

1. Compile o projeto utilizando o comando `make`.
2. Execute o sistema utilizando o comando `make run`.
3. Aguarde a inicialização do kernel.
4. Observe e teste as funcionalidades implementadas na versão atual do sistema.

---

## Estrutura do Projeto

```text
.
├── include/
├── iso/
│   └── boot/
├── kernel/
├── Makefile
└── README.md
```

*A estrutura acima pode variar conforme a organização adotada durante o desenvolvimento.*

---

## Divisão das Atividades

| Matrícula | Integrante | Responsabilidades |
|-----------|------------|-------------------|
| 20240036909 | Iago Vitor Lopes das Chagas | Configuração do ambiente de desenvolvimento, implementação do código, compilação e execução dos testes do sistema. |
| 20230157858 | Kezia da Silva Pessoa | Pesquisa e análise de problemas encontrados, auxílio na depuração, testes, documentação e acompanhamento do desenvolvimento do sistema. |

---

## Referências

- *The Little Book About OS Development*.
- Material da disciplina de Sistemas Operacionais.
- Documentação oficial do GCC, NASM, GRUB e QEMU.