CC=i686-elf-gcc
CFLAGS=-std=gnu99 -ffreestanding -O2 -Wall -Wextra -nostdlib -lgcc
AS=nasm
ASFLAGS=-felf32
QEMU=qemu-system-x86_64

BOOT_DIR=bootloader
KERN_DIR=kernel

DRV_DIR=drivers
ATA_DIR=ata
KBD_DIR=kbd
PIC_DIR=pic
PORTS_DIR=ports
TERM_DIR=term
VIDEO_DIR=video

IDT_DIR=idt

MISC_DIR=misc

MM_DIR=mm

VFS_DIR=vfs

KERNDRV_DIR=$(KERN_DIR)/$(DRV_DIR)

C_DIRS= $(KERNDRV_DIR)/$(ATA_DIR) $(KERNDRV_DIR)/$(KBD_DIR) $(KERNDRV_DIR)/$(PIC_DIR) $(KERNDRV_DIR)/$(PORTS_DIR) $(KERNDRV_DIR)/$(TERM_DIR) $(KERNDRV_DIR)/$(VIDEO_DIR) $(KERN_DIR)/$(VFS_DIR) $(KERN_DIR)/$(MISC_DIR) $(KERN_DIR)/$(IDT_DIR) $(KERN_DIR)/$(MM_DIR)
OBJS= $(KERN_DIR)/kernel.c $(wildcard $(KERNDRV_DIR)/$(ATA_DIR)/*.o) $(wildcard $(KERNDRV_DIR)/$(KBD_DIR)/*.o) $(wildcard $(KERNDRV_DIR)/$(PIC_DIR)/*.o) $($(KERNDRV_DIR)/$(PORTS_DIR)/*.o) $(wildcard $(KERNDRV_DIR)/$(TERM_DIR)/*.o ) $(wildcard $(KERNDRV_DIR)/$(VIDEO_DIR)/*.o) $(wildcard $(KERN_DIR)/$(VFS_DIR)/*.o) $(wildcard $(KERN_DIR)/$(MISC_DIR)/*.o) $(wildcard $(KERN_DIR)/$(IDT_DIR)/*.o) $(wildcard $(KERN_DIR)/$(MM_DIR)/*.o)

KERN_TARG=kernel.bin

IMAGE=BubblesOS.img

LINKSCRIPT=linker.ld

all: boot kern

boot: $(BOOT_DIR)/boot.s
	nasm -fbin $(BOOT_DIR)/boot.s -o $(IMAGE)
kern: $(C_DIRS)
	$(foreach makeFile,$(C_DIRS),$(MAKE) --directory=$(makeFile);)
	$(CC) -c $(KERN_DIR)/kernel.c $(CFLAGS)

	$(CC) $(OBJS) -o $(KERN_TARG) $(CFLAGS) -T $(LINKSCRIPT)
	mcopy -i $(IMAGE) $(KERN_TARG) "::kernel.bin" -o
	mcopy -i $(IMAGE) test "::test" -o
	mcopy -i $(IMAGE) test2 "::test2" -o


clean: $(C_DIRS)
	$(foreach makeFile,$(C_DIRS),$(MAKE) --directory=$(makeFile) clean;)

run:
	$(QEMU) -drive file=$(IMAGE),format=raw,index=0,media=disk
