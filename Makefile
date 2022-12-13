CC=i686-elf-gcc
CFLAGS=-std=gnu99 -ffreestanding -O2 -Wall -Wextra -nostdlib -lgcc
AS=nasm
ASFLAGS=-felf32
QEMU=qemu-system-x86_64

BOOT_DIR=bootloader
KERN_DIR=kernel

# kernel.c NEEDS to be at the start of the list
CSRC=$(KERN_DIR)/kernel.c $(KERN_DIR)/idt.c $(KERN_DIR)/kbd.c $(KERN_DIR)/pic.c $(KERN_DIR)/ports.c $(KERN_DIR)/term.c $(KERN_DIR)/vga.c $(KERN_DIR)/ata.c $(KERN_DIR)/mm.c $(KERN_DIR)/vfs.c $(KERN_DIR)/fat12.c $(KERN_DIR)/dll.c
ASSRC=$(KERN_DIR)/isr.s
COBJS=$(subst $(KERN_DIR)/, ,$(subst .c,.o,$(CSRC)))
ASOBJS=$(subst .s,.o,$(ASSRC))

KERN_TARG=kernel.bin

IMAGE=BubblesOS.img

LINKSCRIPT=linker.ld

all: boot kern

boot: $(BOOT_DIR)/boot.s
	nasm -fbin $(BOOT_DIR)/boot.s -o $(IMAGE)

kern: linker.ld $(ASOBJS) $(COBJS)
	$(CC) $(COBJS) $(ASOBJS) -o $(KERN_TARG) $(CFLAGS) -T $(LINKSCRIPT)
	mcopy -i $(IMAGE) $(KERN_TARG) "::kernel.bin" -o
	mcopy -i $(IMAGE) test "::test" -o
	mcopy -i $(IMAGE) test2 "::test2" -o

$(COBJS): $(CSRC)
	$(CC) $(CFLAGS) -c $(CSRC)

$(ASOBJS): $(ASSRC)
	$(foreach asFile,$(ASSRC),$(AS) $(ASFLAGS) $(asFile) -o $(subst .s,.o,$(asFile));)

clean:
	rm -f boot.o $(COBJS) $(ASOBJS) $(IMAGE) $(KERN_TARG)

run:
	$(QEMU) -drive file=$(IMAGE),format=raw,index=0,media=disk
