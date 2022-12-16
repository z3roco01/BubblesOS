org 0x7C00
bits 16
    jmp short start                             ; MS-DOS/Windows checks for this jump
    nop
bsOemName               DB      "Bubbles "      ; 0x03

;; BPB starts here

bpbBytesPerSector       DW      512             ; 0x0B
bpbSectorsPerCluster    DB      1               ; 0x0D
bpbReservedSectors      DW      1               ; 0x0E
bpbNumFats              DB      2               ; 0x10
bpbRootEntries          DW      224             ; 0x11
bpbTotalSectors         DW      2880            ; 0x13
bpbMedia                DB      0F0h            ; 0x15
bpbSectorsPerFat        DW      9               ; 0x16
bpbSectorsPerTrack      DW      18              ; 0x18
bpbHeadsPerCylinder     DW      2               ; 0x1A
bpbHiddenSectors        DD      0               ; 0x1C
bpbTotalSectorsBig      DD      0               ; 0x20

;; BPB ends here

bsDriveNum              DB      0               ; 0x24
bsUnused                DB      0               ; 0x25
bsExtBootSignature      DB      29H             ; 0x26
bsSerialNumber          DD      11223344h       ; 0x27
bsVolumeLabel           DB      "BUBBLES OS "   ; 0x2B
bsFileSystem            DB      "FAT12   "      ; 0x36

start:
    ; Enable A20
    mov ax, 0x2401
    int 0x15

    ; Set VGA text mode to mode 2
    mov ax, 0x13
    int 0x10

    ; store drive number in the FAT BS
    mov [bsDriveNum], dl

    mov ax, 0x7000
    mov ss, ax
    mov sp, 0xFFFF

    ; read drive params and store them in the fat bpb
    push es
    mov ah, 0x08
    int 0x13
    pop es

    and cl, 0x3F
    xor ch, ch
    mov [bpbSectorsPerTrack], cx

    inc dh
    mov [bpbHeadsPerCylinder], dh

    ; calculate lba of root dir = reserved + fats * sectPerFat
    ; could be hardcoded
    mov ax, [bpbSectorsPerFat]
    mov bl, [bpbNumFats]
    xor bh, bh
    mul bx                          ; ax = fats * sectPerFat
    add ax, [bpbReservedSectors]    ; ax = lba of root dir
    push ax

    ; calculate size of root dir = (32 * numEntries) / bytesPerSect
    mov ax, [bpbSectorsPerFat]
    shl ax, 5                       ; shift left by 5 = * 32
    xor dx, dx
    div word [bpbBytesPerSector]

    test dx, dx                     ; if dx != 0, dx++
    jz _rootDirSizeAfter
    inc ax

_rootDirSizeAfter:
    ; read root dir
    mov cl, al
    pop ax
    mov dl, [bsDriveNum]
    mov bx, buf             ; es:bx = buffer
    call diskRead

    ; find kernel.bin
    xor bx, bx
    mov di, buf

.findKernel:
    mov si, kernelBinName
    mov cx, 11

    push di
    repe cmpsb                  ; compare 11 bytes to kernelBinName
    pop di

    je .foundKernel

    add di, 32
    inc bx
    cmp bx, [bpbRootEntries]
    jl .findKernel

    jmp .kernelNotFound
.foundKernel:
    ; di should contain the addr to the entry
    mov ax, [di + 26]
    mov [kernClust], ax

    mov ax, [bpbReservedSectors]
    mov bx, buf
    mov cl, [bpbSectorsPerFat]
    mov dl, [bsDriveNum]
    call diskRead

    ; Read kernel and process Fat chain
    mov bx, KERNEL_LOAD_SEG
    mov es, bx
    mov bx, KERNEL_LOAD_OFF

.loadKernelLoop:
    mov ax, [kernClust]

    add ax, 31

    mov cl, 1
    mov dl, [bsDriveNum]
    call diskRead

    add bx, [bpbBytesPerSector]

    mov ax, [kernClust]
    mov cx, 3
    mul cx
    mov cx, 2
    div cx                      ; ax = index of entry in fat, dx = cluster mod 2

    mov si, buf
    add si, ax
    mov ax, [ds:si]

    or dx, dx
    jz .even

.odd:
    shr ax, 4
    jmp .nextClustAfter
.even:
    and ax, 0x0FFF

.nextClustAfter:
    cmp ax, 0x0FF8              ; end of chain
    jae .readFinish

    mov [kernClust], ax
    jmp .loadKernelLoop

.readFinish:
    mov dl, [bsDriveNum]

    cli
    ; Load the GDT
    lgdt [gdt.pointer]

    ; Set PE bit of CR0 enabling pmode
    mov eax, cr0
    or eax, 1
    mov cr0, eax

    jmp 0x08:boot2

.kernelNotFound:
    cli
    hlt
    jmp $

; Converts an LBA address to a CHS address
; ax: LBA address
; Returns:
; cx [bits 0-5]: sector number
; cx [bits 6-15]: cylinder
; dh: head
lba2Chs:

    push ax
    push dx

    xor dx, dx                          ; dx = 0
    div word [bpbSectorsPerTrack]       ; ax = LBA / SectorsPerTrack
                                        ; dx = LBA % SectorsPerTrack

    inc dx                              ; dx = (LBA % SectorsPerTrack + 1) = sector
    mov cx, dx                          ; cx = sector

    xor dx, dx                          ; dx = 0
    div word [bpbHeadsPerCylinder]      ; ax = (LBA / SectorsPerTrack) / Heads = cylinder
                                        ; dx = (LBA / SectorsPerTrack) % Heads = head
    mov dh, dl                          ; dh = head
    mov ch, al                          ; ch = cylinder (lower 8 bits)
    shl ah, 6
    or cl, ah                           ; put upper 2 bits of cylinder in CL

    pop ax
    mov dl, al                          ; restore DL
    pop ax
    ret

; Reads sectors from a disk
; ax: LBA address
; cl: number of sectors to read (up to 128)
; dl: drive number
; es:bx: memory address where to store read data
diskRead:

    push ax                             ; save registers we will modify
    push bx
    push cx
    push dx
    push di

    push cx                             ; temporarily save CL (number of sectors to read)
    call lba2Chs                     ; compute CHS
    pop ax                              ; AL = number of sectors to read

    mov ah, 02h
    mov di, 3                           ; retry count

.retry:
    pusha                               ; save all registers, we don't know what bios modifies
    stc                                 ; set carry flag, some BIOS'es don't set it
    int 13h                             ; carry flag cleared = success
    jnc .done                           ; jump if carry not set

    ; read failed
    popa
    call diskReset

    dec di
    test di, di
    jnz .retry

.fail:
    ; all attempts are exhausted
    cli
    hlt

.done:
    popa

    pop di
    pop dx
    pop cx
    pop bx
    pop ax                             ; restore registers modified
    ret

diskReset:
    pusha
    mov ah, 0
    stc
    int 13h
    popa
    ret

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

    mov esp,0x4000

    call 0x20000
halt:
    cli
    hlt
    jmp halt

kernelBinName:  db 'KERNEL  BIN'
kernClust:      dw 0

KERNEL_LOAD_SEG equ 0x2000
KERNEL_LOAD_OFF equ 0x00

times 510 - ($-$$) db 0 ; pad remaining 510 bytes with zeroes
dw 0xAA55 ; magic bootloader magic - marks this 512 byte sector bootable!bits 32
buf:
times 1440000 - ($-$$) db 0

;section .bss
;align 4
;kernel_stack_bottom: equ $
;    resb 16384 ; 16 KB
;kernel_stack_top:
