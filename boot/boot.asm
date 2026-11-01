; boot.asm - a 512-byte BIOS boot sector (stage 1).
;
; The BIOS loads this single sector to physical address 0x7C00 and jumps to it
; in 16-bit real mode. Our job here is small but fiddly:
;   1. print a short "loading" message with BIOS teletype (int 10h),
;   2. read the kernel off the boot disk into memory (int 13h),
;   3. enable the A20 line and switch the CPU into 32-bit protected mode,
;   4. jump to the kernel.
;
; Assemble with:  nasm -f bin boot/boot.asm -o build/boot.bin

bits 16
org 0x7C00

KERNEL_OFFSET equ 0x1000        ; where we load the kernel in memory
KERNEL_SECTORS equ 32           ; how many 512-byte sectors to read (16 KiB)

start:
    cli
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7C00              ; stack grows down from the boot sector
    sti

    mov [boot_drive], dl       ; BIOS leaves the boot drive number in DL

    mov si, msg_loading
    call print_string

    call load_kernel
    call enable_a20
    call enter_protected_mode  ; does not return

; --- 16-bit helpers ---------------------------------------------------------

; print_string: write the NUL-terminated string at DS:SI via BIOS teletype.
print_string:
    pusha
.next:
    lodsb
    test al, al
    jz .done
    mov ah, 0x0E
    mov bh, 0x00
    int 0x10
    jmp .next
.done:
    popa
    ret

; load_kernel: read KERNEL_SECTORS sectors starting at LBA 2 (the sector right
; after this boot sector) into ES:BX = 0x0000:KERNEL_OFFSET.
load_kernel:
    pusha
    mov bx, KERNEL_OFFSET
    mov ah, 0x02               ; BIOS read sectors
    mov al, KERNEL_SECTORS
    mov ch, 0x00              ; cylinder 0
    mov cl, 0x02              ; start at sector 2 (1-based)
    mov dh, 0x00              ; head 0
    mov dl, [boot_drive]
    int 0x13
    jc .error
    popa
    ret
.error:
    mov si, msg_disk_error
    call print_string
    jmp halt16

enable_a20:
    ; "Fast A20" via the system control port. Good enough for QEMU/Bochs.
    in al, 0x92
    or al, 0x02
    out 0x92, al
    ret

enter_protected_mode:
    cli
    lgdt [gdt_descriptor]
    mov eax, cr0
    or eax, 0x1                ; set PE (protection enable)
    mov cr0, eax
    jmp CODE_SEG:init_pm       ; far jump flushes the pipeline into 32-bit code

halt16:
    hlt
    jmp halt16

; --- 32-bit entry -----------------------------------------------------------

bits 32
init_pm:
    mov ax, DATA_SEG
    mov ds, ax
    mov ss, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov esp, 0x90000           ; a roomy stack below 1 MiB
    jmp KERNEL_OFFSET          ; hand control to the C kernel

; --- Global Descriptor Table ------------------------------------------------

gdt_start:
    dq 0x0000000000000000      ; null descriptor
gdt_code:
    dw 0xFFFF                  ; limit 0:15
    dw 0x0000                  ; base 0:15
    db 0x00                    ; base 16:23
    db 10011010b               ; access: present, ring0, code, exec/read
    db 11001111b               ; flags + limit 16:19 (4 KiB granularity, 32-bit)
    db 0x00                    ; base 24:31
gdt_data:
    dw 0xFFFF
    dw 0x0000
    db 0x00
    db 10010010b               ; access: present, ring0, data, read/write
    db 11001111b
    db 0x00
gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dd gdt_start

CODE_SEG equ gdt_code - gdt_start
DATA_SEG equ gdt_data - gdt_start

; --- data -------------------------------------------------------------------

boot_drive      db 0
msg_loading     db "mini-os: loading kernel...", 0x0D, 0x0A, 0
msg_disk_error  db "mini-os: disk read error", 0x0D, 0x0A, 0

; pad to 510 bytes, then the 0xAA55 boot signature.
times 510 - ($ - $$) db 0
dw 0xAA55
