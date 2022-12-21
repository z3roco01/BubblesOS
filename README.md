# BubblesOS
A toy operating system

## Requirements
i686-elf binutils and gcc </br>
mtools </br>
make </br>
nasm </br>
qemu-system-x86_64 </br>

## Compiling
To compile run
`
make all
`

to test the os in qemu-system-x86_64 run
`
make run
`
after compiling.

## Features (implemented and planned)
- [X] FAT12
- [X] ATA driver
- [X] Memory allocater(only basic allocator)
- [X] VFS
- [ ] FAT12 VFS(half done)
- [X] Proper keyboard driver
- [ ] BASIC interpreter
- [ ] ELF executer
- [X] Real graphics(not VGA text mode)
