CC=i686-elf-gcc
CFLAGS=-std=gnu99 -ffreestanding -O2 -Wall -Wextra -nostdlib -lgcc

SRC=kbd.c
OBJ=$(subst .c,.o,$(SRC))

all: $(OBJ) $(AS_OBJ)

$(OBJ): $(SRC)
	$(CC) -c $(SRC) $(CFLAGS)

clean:
	rm -f $(OBJ)
