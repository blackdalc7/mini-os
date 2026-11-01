; entry.asm - 32-bit kernel entry stub.
;
; The boot sector jumps to the start of the loaded kernel in protected mode.
; The linker script places this stub first, so execution begins at _start.
; We set up a stack and call the C entry point, then halt forever.
;
; Assemble with:  nasm -f elf32 kernel/entry.asm -o build/entry.o

bits 32

global _start
extern kernel_main

section .text
_start:
    mov esp, stack_top
    call kernel_main
.hang:
    cli
    hlt
    jmp .hang

section .bss
align 16
stack_bottom:
    resb 16384            ; 16 KiB kernel stack
stack_top:

; Mark the stack non-executable to keep the linker quiet; the bit is advisory
; for a bare-metal image but silences a spurious warning.
section .note.GNU-stack noalloc noexec nowrite progbits
