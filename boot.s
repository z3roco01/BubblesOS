org 0x7C00
section .boot
bits 16
global boot
boot:
    ; Enable A20
    mov ax, 0x2401
    int 0x15

    ; Set VGA text mode to mode 3
    mov ax, 0x3
    int 0x10

    mov [disk], dl


    mov ax, 0x1000
    mov es, ax
    mov bx, 0    ; target pointer

    mov ah, 2           ; read sects
    mov al, 32           ; sectors to read
    mov ch, 0           ; cylinder idx
    mov dh, 0           ; head idx
    mov cl, 2           ; sect idx
    mov dl, [disk]      ; disk idx
    int 0x13

    cli
    ; Load the GDT
    lgdt [gdt.pointer]

    ; Set PE bit of CR0 enabling pmode
    mov eax, cr0
    or eax, 1
    mov cr0, eax

    jmp 0x08:boot2

gdt:
.null:
    dq 0
.kcode:
    dw 0xFFFF       ; First 16 limit bits
    dw 0x0000       ; First 16 base bits
    db 0x00         ; Middle 8 base bits
    db 0b10011010   ; Access bits
    db 0b11001111   ; 4 flag bits and high 4 limit bits
    db 0x00         ; High 8 base bits
.kdata:
    dw 0xFFFF
    dw 0x0000
    db 0x00
    db 0b10010010
    db 0b11001111
    db 0x00
.pointer:
    dw gdt.pointer - gdt
    dd gdt

disk:
    db 0x00

bits 32
boot2:
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    mov esp,kernel_stack_top

    call 0x10000
halt:
    cli
    hlt
    jmp halt

times 510 - ($-$$) db 0 ; pad remaining 510 bytes with zeroes
dw 0xAA55 ; magic bootloader magic - marks this 512 byte sector bootable!bits 32

section .bss
align 4
kernel_stack_bottom: equ $
    resb 16384 ; 16 KB
kernel_stack_top:
