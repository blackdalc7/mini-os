/* linker.ld - lay out the flat kernel binary.
 *
 * The boot sector loads the kernel to physical 0x1000 and jumps there, so we
 * link at 0x1000 with entry.o first (its _start is the entry point). objcopy
 * then strips the ELF wrapper to leave a raw binary the boot sector can run.
 */
ENTRY(_start)

SECTIONS
{
    . = 0x1000;

    .text : {
        *(.text._start)
        *(.text*)
    }

    .rodata : { *(.rodata*) }
    .data   : { *(.data*) }
    .bss    : {
        *(COMMON)
        *(.bss*)
    }

    /DISCARD/ : { *(.comment) *(.note*) *(.eh_frame*) }
}
