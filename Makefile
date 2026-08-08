OBJECTS = loader.o kmain.o fb.o io.o serial.o gdt.o gdt_asm.o idt.o isr.o interrupt.o pic.o keyboard.o paging.o paging_asm.o pfa.o kheap.o user.o user_asm.o
CC = gcc
CFLAGS = -m32 -nostdlib -nostdinc -fno-builtin -fno-stack-protector \
         -nostartfiles -nodefaultlibs -Wall -Wextra -Werror -c
LDFLAGS = -T link.ld -melf_i386
AS = nasm
ASFLAGS = -f elf32

all: kernel.elf

kernel.elf: $(OBJECTS)
	ld $(LDFLAGS) $(OBJECTS) -o kernel.elf

os.iso: kernel.elf program
	mkdir -p iso/modules
	cp kernel.elf iso/boot/kernel.elf
	cp program iso/modules/program
	genisoimage -R                              \
	          -b boot/grub/stage2_eltorito    \
                -no-emul-boot                   \
                -boot-load-size 4               \
                -A os                           \
                -input-charset utf8             \
                -quiet                          \
                -boot-info-table                \
                -o os.iso                       \
                iso

program: program.s
	nasm -f bin program.s -o program

run: os.iso
	qemu-system-i386 -cdrom os.iso -serial file:com1.out -k pt-br -no-reboot -no-shutdown -d int,cpu_reset -D qemu.log

%.o: %.c
	$(CC) $(CFLAGS)  $< -o $@

%.o: %.s
	$(AS) $(ASFLAGS) $< -o $@

clean:
	rm -rf *.o kernel.elf os.iso
