/* kernel.c - the C entry point, reached from entry.asm in 32-bit mode.
 *
 * It clears the screen, prints a banner to both the VGA text buffer and the
 * COM1 serial port, shows a few computed values (to prove the helpers work),
 * then halts. The serial output is what the head-less CI boot test checks for.
 */
#include <stdint.h>

#include "serial.h"
#include "util.h"
#include "vga.h"

/* Print the same line to the screen and to the serial port. */
static void kprint(const char *s)
{
    vga_puts(s);
    serial_puts(s);
}

static void kprint_uint(uint32_t value, int base)
{
    char buf[33];
    k_utoa(value, buf, base);
    kprint(buf);
}

void kernel_main(void)
{
    vga_init();
    serial_init();

    vga_set_color(vga_entry_color(VGA_LIGHT_GREEN, VGA_BLACK));
    kprint("mini-os kernel: booted in 32-bit protected mode\n");

    vga_set_color(vga_entry_color(VGA_LIGHT_GREY, VGA_BLACK));
    kprint("hello from C, running on bare metal\n");

    kprint("decimal 2026 in hex is 0x");
    kprint_uint(2026, 16);
    kprint(", in binary is ");
    kprint_uint(2026, 2);
    kprint("\n");

    kprint("mini-os: halted.\n");

    /* entry.asm halts the CPU after this returns. */
}
