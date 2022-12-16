CC=i686-elf-gcc
CFLAGS=-std=gnu99 -ffreestanding -O2 -Wall -Wextra -nostdlib -lgcc
AS=nasm
ASFLAGS=-felf32

SRC=idt.c
OBJ=$(subst .c,.o,$(SRC))
AS_SRC=isr.s
AS_OBJ=$(subst .s,.o,$(AS_SRC))

all: $(OBJ) $(AS_OBJ)

$(AS_OBJ): $(AS_SRC)
	$(foreach asFile,$(AS_SRC),$(AS) $(ASFLAGS) $(asFile) -o $(subst .s,.o,$(asFile));)

$(OBJ): $(SRC)
	$(CC) -c $(SRC) $(CFLAGS)


clean:
	rm -f $(OBJ) $(AS_OBJ)
