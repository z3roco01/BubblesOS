CC=i686-elf-gcc
CFLAGS=-std=gnu99 -ffreestanding -O2 -Wall -Wextra -nostdlib -lgcc
AS=nasm
ASFLAGS=-felf32
QEMU=qemu-system-x86_64

BOOT_DIR=bootloader
KERN_DIR=kernel

# kernel.c NEEDS to be at the start of the list
CSRC=$(KERN_DIR)/kernel.c $(KERN_DIR)/idt.c $(KERN_DIR)/kbd.c $(KERN_DIR)/pic.c $(KERN_DIR)/ports.c $(KERN_DIR)/term.c $(KERN_DIR)/vga.c $(KERN_DIR)/ata.c
ASSRC=$(KERN_DIR)/isr.s
COBJS=$(subst $(KERN_DIR)/, ,$(subst .c,.o,$(CSRC)))
ASOBJS=$(subst .s,.o,$(ASSRC))

KERN_TARG=kernel.bin

TARGET=BubblesOS.img

LINKSCRIPT=linker.ld

all: $(TARGET)

$(TARGET): linker.ld $(ASOBJS) $(COBJS)
	$(CC) $(COBJS) $(ASOBJS) -o $(KERN_TARG) $(CFLAGS) -T $(LINKSCRIPT)
	nasm -fbin $(BOOT_DIR)/boot.s -o $(TARGET)
	mcopy -i $(TARGET) $(KERN_TARG) "::kernel.bin"
	mcopy -i $(TARGET) test "::test"

$(COBJS): $(CSRC)
	$(CC) $(CFLAGS) -c $(CSRC)

$(ASOBJS): $(ASSRC)
	$(foreach asFile,$(ASSRC),$(AS) $(ASFLAGS) $(asFile) -o $(subst .s,.o,$(asFile));)

clean:
	rm -f boot.o $(COBJS) $(ASOBJS) $(TARGET) $(KERN_TARG)

run: $(TARGET)
	$(QEMU) -drive file=$(TARGET),format=raw,index=0,media=disk
