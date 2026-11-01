/* test_util.c - host-side unit tests for the kernel's pure helpers.
 *
 * Compiled with the ordinary host compiler (not the freestanding kernel build)
 * and run as a normal program in CI. It exercises the string/number helpers in
 * util.c and the VGA cell-encoding helpers in vga.h, none of which touch
 * hardware, so they behave identically on the host and in the kernel.
 */
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "util.h"
#include "vga.h"

static void test_strlen(void)
{
    assert(k_strlen("") == 0);
    assert(k_strlen("abc") == 3);
    assert(k_strlen("mini-os") == 7);
}

static void test_memset(void)
{
    char buf[8];
    k_memset(buf, 'x', sizeof(buf));
    for (size_t i = 0; i < sizeof(buf); i++) {
        assert(buf[i] == 'x');
    }
    k_memset(buf, 0, sizeof(buf));
    assert(buf[0] == 0 && buf[7] == 0);
}

static void test_utoa(void)
{
    char buf[33];

    assert(strcmp(k_utoa(0, buf, 10), "0") == 0);
    assert(strcmp(k_utoa(2026, buf, 10), "2026") == 0);
    assert(strcmp(k_utoa(255, buf, 16), "ff") == 0);
    assert(strcmp(k_utoa(2026, buf, 16), "7ea") == 0);
    assert(strcmp(k_utoa(5, buf, 2), "101") == 0);
    assert(strcmp(k_utoa(4294967295u, buf, 10), "4294967295") == 0);

    /* invalid base yields an empty string rather than garbage */
    assert(strcmp(k_utoa(1, buf, 99), "") == 0);
}

static void test_vga_encoding(void)
{
    uint8_t color = vga_entry_color(VGA_LIGHT_GREEN, VGA_BLACK);
    assert(color == 0x0A);

    color = vga_entry_color(VGA_WHITE, VGA_BLUE);
    assert(color == 0x1F);

    uint16_t cell = vga_entry('A', vga_entry_color(VGA_LIGHT_GREY, VGA_BLACK));
    assert((cell & 0xFF) == 'A');
    assert((cell >> 8) == 0x07);
}

int main(void)
{
    test_strlen();
    test_memset();
    test_utoa();
    test_vga_encoding();
    printf("all mini-os unit tests passed\n");
    return 0;
}
