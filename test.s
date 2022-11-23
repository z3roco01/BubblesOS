[BITS 16]
[ORG 0]                 ; Tells the assembler to consider this as the base address
    jmp short start     ; jump over the data space to the actual code. executing data is generally a bad idea...
                        ; If this is to be a valid fat12 fs, the jump -MUST- be a short. effectively, this means
                        ; the code entry point needs to be the first routine listed.
    nop                 ; padding :) (hmmm...ignore what i said above and just remove this...)
; BIOS parameter block follows. This has all the bits and pieces describing
; the file system that the boot block supposes resides on the boot media

OEMid           db "MSWIN4.1"       ; OEM id. 8 bytes in size, basically a version string
BytesPerSector  dw 512              ; bytes making up each sector, 2 bytes
SecsPerCluster  db 1                ; sectors per cluster, 1 byte
ReserveCount    dw 1                ; reserved sectors before first FAT, 2 bytes
FATcount        db 2                ; number of FATs appearing on media
DirEntries      dw 224              ; directory entries max (root dir), 2 bytes
TotalSectors    dw 2880             ; Total sectors in file system (1.44MB Floppy)
MediaDescriptor db 0xF0             ; Media descriptor, 1 byte. F0 for 1.44M
SectorsPerFAT   dw 9                ; Sectors per FAT, 2 bytes
SectorsPerTrack dw 18               ; Sectors per track, 2 bytes.
HeadCount       dw 2                ; Number of drive heads (On a double sided diskette, there are 2 :))
HiddenSectors   dd 0                ; Hidden sectors before FAT. should be zero on an unpartitioned media
; In larger file systems, there would be a double word field here defining the
; total number of sectors. since we don't need this, we'll just pad it out
; so that we can write an Extended BPB below
                dd 0
DriveNumber     db 0                ; Physical drive number, 0 is removable. 80h for hdd
DirtyBit        db 1                ; WinNT uses this field to check if the fs is clean
extBootSig      db 0x29             ; This lets us know the EBPB exists
VolumeID        dd 77               ; Serial number for the volume
VolumeLabel     db "durOS     ",0    ; Volume label. pad out to 11 bytes with spaces
FSType          db "FAT12   "       ; File system type, pad with blanks to 8 bytes
; ********************** END BIOS PARAMETER BLOCK *************************

start:
    ; We first need to update the segment registers
    mov ax, 0x7c0       ; the boot code is loaded to 7C00
    mov ds, ax
    mov ax, 0x7c0       ; We're going to load data into conventional memory above the boot block.
    mov es, ax
    ; Now, we need a stack. We'll put this at the top of the conventional memory space.
    cli
    mov ax, 0x7000
    mov ss, ax
    mov sp, 0xFFFF
    sti

    ; Set up arguments to int 13h
    mov cx, WORD [SectorsPerFAT]   ; sectors to load from disk
    mov ax, WORD [ReserveCount]       ; sector offset from sector 0
    mov bx, 0x200                   ; We don't want to overwrite the boot block
                            ; after the reserved sectors (in our case, the boot sector)
    mov WORD [FAT0addr], bx      ; Save the address offset so we can use it later.
    call read_sectors       ; Make it so number 1.
    mov si, op_fat
    call print_str
    ; For now, we'll just load FAT0 to save code space in the boot block
    ; Now we'll load the root directory
    ; calculate the root directory size, store to cx
    mov ax, 0x20                ; size of each root dir entry is 32 bytes
    mul WORD [DirEntries]        ; DirEntries * 32 / BytesPerSector = RootSize
    div WORD [BytesPerSector]    ; ax now has root dir size in bytes.
    xchg ax, cx
    ; Now, calculate the root directory location on disk, and store to ax
    mov al, BYTE [FATcount]
    mul WORD [SectorsPerFAT]
    ; We'll compute the RAM location to store the root dir to here before we add the sector reserve
    mov bx, ax                  ; bx now has the number of sectors taken by the FATs
    mul WORD [BytesPerSector]    ; FATCount * SectorsPerFAT * BytesPerSector + ReserveCount = offset
    add ax, 0x200               ; offset the boot code
                                ; ax now has the offset to apply to RAM
    xchg bx, ax                    ; swap the two over and continue
    add ax, WORD [ReserveCount]    ; skip this many sectors
    mov WORD [rootaddr], bx        ; save to ram for later on
                                ; root directory will be loaded at 07c0:2600 (A200 physical)
    call read_sectors
    mov si, op_root
    call print_str
    ; OK, that should do it for the disk reading for now. we have copies of FAT0 and FAT1 in RAM
    ; Now, we want to find the kernel image so we can continue execution. We'll load this above the root directory
    ; The root directory occupies 14 sectors from 7E0:2400 -> 7E0:3E00, so this is where we'll load the image.
    ; We are guaranteed 465kb to be free here, this should be ample space to load our second stage image.
    mov si, op_k
    call print_str
    call load_kernel
    cmp dx, 1
    jz .opfail
    jmp 7c0h:4200h
    .opfail:
        mov si, op_fail
        call print_str
        call reboot

;---------------------------------- DATA AREA -------------------------------------

op_fat          db " FAT",13,10,0
op_root         db " Root",13,10,0
op_fail         db "Err",13,10,0
op_k            db " OK. Booting Kernel",13,10,0
op_progress        db ".",0
kernel          db "KERNEL",0


FAT0addr        resb 2
rootaddr        resb 2
sectorAbs       resb 1
headAbs         resb 1
trackAbs        resb 1

;--------------------------------- SUBROUTINES ------------------------------------
; print_str - outputs a string to the terminal. expects the starting address in si
print_str:
    lodsb
    or al, al
    jz .done
    mov ah, 0xE
    mov bx, 7
    int 10h
    jmp print_str
    .done:
        ret


; reboot - wait for keypress and then reboot the machine
reboot:
    call get_key
    db 0xEA         ; Machine code for jmp instruction
    dw 0x0000       ; Offset
    dw 0xFFFF       ; segment

; get_key - wait for a keypress and return
get_key:
    mov ah, 0
    int 16h
    ret


; read_sectors - load sectors off the floppy disk into ram at ES:BX
; Assumes that:
; - cx contains the number of sectors to read off the disk. These will be read one at a time
; - ax contains the LBA sector offset. This will be converted to a CHS value before the read

read_sectors:
.main:
    mov di, 5                ; Retry on err
    ; First things first, save the values in the register while we work out the CHS value
.sectorloop:
    push ax
    push bx
    push cx
    call lbachs    ; return values will be saved below.
    mov ah, 2    ; function parameter for int 13h
    mov al, 0x01; read 1 sector
    mov ch, BYTE [trackAbs] ; Cylinder address
    mov cl, BYTE [sectorAbs]; Sector address
    mov dh, BYTE [headAbs]    ; Head address
    mov dl, 0                ; Drive number
    int 13h                    ; Make it so, Number 1
    jnc .readsuccess
    ; Read failed. decrement retry count, reset and go again, or fail
    xor ax, ax
    int 13h                    ; reset drive
    dec di
    pop cx
    pop bx
    pop ax
    jnz .sectorloop
    mov si, op_fail
    call print_str
    call reboot
.readsuccess:
    mov si, op_progress
    call print_str
    pop cx
    pop bx
    pop ax
    add bx, WORD [BytesPerSector]    ; increment the buffer so we don't overwrite the last sector
    inc ax                            ; Increment LBA sector offset
    loop .main                        ; decrement cx and start again.
    ret                                ; all sectors read



; lbachs - convert LBA address to CHS address
; assumes ax is an LBA sector address
; returns:
; sectorAbs = (sector % SectorsPerTrack) + 1
; headAbs = sector / SectorsPerTrack % HeadCount
; trackAbs = sector / (SectorsPerTrack * HeadCount)
lbachs:
    push bx
    push ax                        ; save a copy of ax for later
    xor dx, dx
    ; do sectors first
    div WORD [SectorsPerTrack]     ; dx now contains the remainder
    inc dl                           ; add 1
    mov BYTE [sectorAbs], dl    ; Save the result
    xor dx, dx
    ; now Heads
    ; ax should have the quotient of sector / SPT from the previous op
    div WORD [HeadCount]        ; remainder in dl again
    mov BYTE [headAbs], dl
    ; Finally, do cylinders
    mov ax, WORD [HeadCount]
    mul WORD [SectorsPerTrack]
    xor dx, dx
    xchg bx, ax
    pop ax
    div bx                        ; ax now contains the cylinder address
    mov BYTE [trackAbs], al
    xor dx, dx
    pop bx
    ret


; load_kernel - load the kernel image into RAM
; assumes address is loaded to es:bx, kernel image name is assumed to be "kernel"
load_kernel:
    xor cx, cx
    mov bx, [rootaddr]
    ; Now, we go through one at a time, and find the root dir entry that we want
    .next:
        ; first, we'll check for an empty entry, then a null
        mov al, [bx]
        cmp al, 0xE5
        je .nomatch
        cmp al, 0x0
        je .error
        mov si, kernel
        mov di, bx
        mov cx, 6
        rep cmpsb
        jne .nomatch
        .match:             ; we have a winner.
            call fetch_kernel
            ret
        .nomatch:
            add bx, 32
            jmp .next
        .error:
            mov dx, 1
            ret

; fetch_kernel - fetches kernel sectors off disk
; assumes es:bx points to a valid root directory entry for the kernel file
fetch_kernel:
    mov ax, WORD [bx + 26]       ; This is the first cluster address.
    mov bx, 0x4200                  ; load our memory offset,
    .fetch:
    push ax                         ; ax has the first cluster address, but we need to do stuff to it atm
    xor cx, cx                      ; zero out cx so we don't get issues here
    add ax, 31                      ; Physical sector number = FAT entry + ReserveCount + (FATcount * SectorsPerFAT) - 2
                                    ; We subtract 2 because the first 2 sectors of the data area are reserved.
    mov cl, BYTE [SecsPerCluster]   ; number of clusters to get from disk
    call read_sectors               ; grab the cluster from disk
    pop ax
    push bx                         ; save the offset address as we need to load stuff from FAT
    ; now, we need to examine the FAT and determine the next sector to load. we repeat this until all sectors are loaded into RAM
    ; first, we need to know which side of the byte to load the additional 4 bits from
    ; If the cluster address (n) is even:
    ; low 4 bits from bits from 1+(3*n)/2
    ; high 8 bits from (3*n)/2
    ; if the cluster address is odd:
    ; high 4 bits from (3*n)/2
    ; low 4 bits from 1+(3*n)/2
    .getnext:
        ; ax has cluster address
        mov cx, ax
        mov dx, ax
        shr dx, 1                       ; divide by 2
        add cx, dx                      ; 1.5 bytes
        mov bx, [FAT0addr]
        add bx, cx                      ; calculate the offset in the FAT
        mov dx, WORD [bx]               ; grab 16 bits from FAT
        test ax, 1                      ; check if ax is odd or even
        jnz .odd
        .even:
            and dx, 0xFFF               ; use the low 12 bits
            jmp .check
        .odd:
            shr dx, 0x4                 ; use high 12 bits
        .check:                         ; at this point, ax contains the cluster address of the next block
            mov ax, dx                  ; mov the address into ax
            ; first we examine it for state info.
            cmp ax, 0xFF7
            jz .error
            cmp ax, 0xFF8
            jge .done                ; all clusters loaded.
            ; If we have anything other than these two values, we can assume that the next cluster belongs to kernel
            ; NOTE: there are other status codes that we are not checking that will result in error, but i want to conserve space here
            pop bx
            jmp .fetch
        .error:
            pop bx
            mov dx, 1
        .done:
            pop bx
            ret

times 510-($-$$) db 0
db 0x55
db 0xAA
times 1440000-($-$$) db 0
