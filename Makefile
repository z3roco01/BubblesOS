CC=i686-elf-gcc
CFLAGS=-std=gnu99 -ffreestanding -O2 -Wall -Wextra -nostdlib -lgcc
AS=nasm
ASFLAGS=-felf32
QEMU=qemu-system-x86_64

# kernel.c NEEDS to be at the start of the list
CSRC=kernel.c idt.c kbd.c pic.c ports.c term.c vga.c
ASSRC=isr.s
COBJS=$(subst .c,.o,$(CSRC))
ASOBJS=$(subst .s,.o,$(ASSRC))

KERN_TARG=kernel.bin

TARGET=deezOS.img
IMG_SIZE=32000000

LINKSCRIPT=linker.ld

all: $(TARGET)

$(TARGET): linker.ld $(ASOBJS) $(COBJS)
	$(CC) $(COBJS) isr.o -o $(KERN_TARG) $(CFLAGS) -T $(LINKSCRIPT)
	nasm -fbin boot.s -o boot.o
	dd if=/dev/null of=./$(TARGET) seek=$(IMG_SIZE) count=1 bs=1 status=none
	mkfs.fat -F 16 ./$(TARGET) >/dev/null
	mcopy -i $(TARGET) $(KERN_TARG) ::/
	dd if=./boot.o of=./$(TARGET) bs=448 count=1 seek=62 status=none conv=notrunc
	cat boot.o kernel.bin > $(TARGET)

$(COBJS): $(CSRC)
	$(CC) $(CFLAGS) -c $(CSRC)

$(ASOBJS): $(ASSRC)
	$(foreach asFile,$(ASSRC),$(AS) $(ASFLAGS) $(asFile) -o $(subst .s,.o,$(asFile));)

clean:
	rm -f boot.o $(COBJS) $(ASOBJS) $(TARGET) $(KERN_TARG)

run: $(TARGET)
	$(QEMU) -hda $(TARGET)
