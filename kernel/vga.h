/* vga.h - VGA text-mode (80x25) driver interface.
 *
 * The encoding helpers (vga_entry_color, vga_entry) are pure functions and are
 * unit-tested on the host. The driver functions touch the hardware text buffer
 * at 0xB8000 and only run inside the kernel.
 */
#ifndef MINIOS_VGA_H
#define MINIOS_VGA_H

#include <stdint.h>

enum vga_color {
    VGA_BLACK = 0,
    VGA_BLUE = 1,
    VGA_GREEN = 2,
    VGA_CYAN = 3,
    VGA_RED = 4,
    VGA_MAGENTA = 5,
    VGA_BROWN = 6,
    VGA_LIGHT_GREY = 7,
    VGA_DARK_GREY = 8,
    VGA_LIGHT_BLUE = 9,
    VGA_LIGHT_GREEN = 10,
    VGA_LIGHT_CYAN = 11,
    VGA_LIGHT_RED = 12,
    VGA_LIGHT_MAGENTA = 13,
    VGA_LIGHT_BROWN = 14,
    VGA_WHITE = 15,
};

#define VGA_WIDTH 80
#define VGA_HEIGHT 25

/* Pack a foreground and background colour into one attribute byte. */
static inline uint8_t vga_entry_color(enum vga_color fg, enum vga_color bg)
{
    return (uint8_t)(fg | (uint8_t)(bg << 4));
}

/* Pack a character and attribute into one 16-bit cell for the text buffer. */
static inline uint16_t vga_entry(unsigned char uc, uint8_t color)
{
    return (uint16_t)uc | (uint16_t)(color << 8);
}

/* Driver (kernel-only). */
void vga_init(void);
void vga_set_color(uint8_t color);
void vga_putc(char c);
void vga_puts(const char *s);

#endif /* MINIOS_VGA_H */
