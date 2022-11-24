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

TARGET=BubblesOS.img

LINKSCRIPT=linker.ld

all: $(TARGET)

$(TARGET): linker.ld $(ASOBJS) $(COBJS)
	$(CC) $(COBJS) isr.o -o $(KERN_TARG) $(CFLAGS) -T $(LINKSCRIPT)
	nasm -fbin boot.s -o $(TARGET)
	mcopy -i $(TARGET) $(KERN_TARG) "::kernel.bin"
	#cat boot.o kernel.bin > $(TARGET)

$(COBJS): $(CSRC)
	$(CC) $(CFLAGS) -c $(CSRC)

$(ASOBJS): $(ASSRC)
	$(foreach asFile,$(ASSRC),$(AS) $(ASFLAGS) $(asFile) -o $(subst .s,.o,$(asFile));)

clean:
	rm -f boot.o $(COBJS) $(ASOBJS) $(TARGET) $(KERN_TARG)

run: $(TARGET)
	$(QEMU) -hda $(TARGET)
