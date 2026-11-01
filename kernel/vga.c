/* vga.c - VGA text-mode driver: cursor, scrolling, colour. */
#include "vga.h"

#include "util.h"

static volatile uint16_t *const VGA_BUFFER = (volatile uint16_t *)0xB8000;

static size_t vga_row;
static size_t vga_col;
static uint8_t vga_color;

void vga_init(void)
{
    vga_row = 0;
    vga_col = 0;
    vga_color = vga_entry_color(VGA_LIGHT_GREY, VGA_BLACK);
    uint16_t blank = vga_entry(' ', vga_color);
    for (size_t i = 0; i < (size_t)VGA_WIDTH * VGA_HEIGHT; i++) {
        VGA_BUFFER[i] = blank;
    }
}

void vga_set_color(uint8_t color)
{
    vga_color = color;
}

static void vga_scroll(void)
{
    /* Move every row up by one and clear the last row. */
    for (size_t y = 1; y < VGA_HEIGHT; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            VGA_BUFFER[(y - 1) * VGA_WIDTH + x] = VGA_BUFFER[y * VGA_WIDTH + x];
        }
    }
    uint16_t blank = vga_entry(' ', vga_color);
    for (size_t x = 0; x < VGA_WIDTH; x++) {
        VGA_BUFFER[(VGA_HEIGHT - 1) * VGA_WIDTH + x] = blank;
    }
    vga_row = VGA_HEIGHT - 1;
}

static void vga_newline(void)
{
    vga_col = 0;
    if (++vga_row >= VGA_HEIGHT) {
        vga_scroll();
    }
}

void vga_putc(char c)
{
    if (c == '\n') {
        vga_newline();
        return;
    }
    VGA_BUFFER[vga_row * VGA_WIDTH + vga_col] = vga_entry((unsigned char)c, vga_color);
    if (++vga_col >= VGA_WIDTH) {
        vga_newline();
    }
}

void vga_puts(const char *s)
{
    size_t len = k_strlen(s);
    for (size_t i = 0; i < len; i++) {
        vga_putc(s[i]);
    }
}
