/* serial.c - 8250/16550 UART on COM1 (0x3F8), transmit only. */
#include "serial.h"

#include "io.h"
#include "util.h"

#define COM1 0x3F8

void serial_init(void)
{
    outb(COM1 + 1, 0x00); /* disable interrupts */
    outb(COM1 + 3, 0x80); /* enable DLAB to set the baud divisor */
    outb(COM1 + 0, 0x03); /* divisor low  (115200 / 3 = 38400 baud) */
    outb(COM1 + 1, 0x00); /* divisor high */
    outb(COM1 + 3, 0x03); /* 8 bits, no parity, one stop bit */
    outb(COM1 + 2, 0xC7); /* enable + clear FIFO, 14-byte threshold */
    outb(COM1 + 4, 0x0B); /* IRQs enabled, RTS/DSR set */
}

static int transmit_empty(void)
{
    return inb(COM1 + 5) & 0x20;
}

void serial_putc(char c)
{
    while (transmit_empty() == 0) {
        /* busy-wait for the transmit holding register */
    }
    outb(COM1, (unsigned char)c);
}

void serial_puts(const char *s)
{
    size_t len = k_strlen(s);
    for (size_t i = 0; i < len; i++) {
        if (s[i] == '\n') {
            serial_putc('\r');
        }
        serial_putc(s[i]);
    }
}
