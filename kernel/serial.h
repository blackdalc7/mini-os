/* serial.h - minimal COM1 serial output.
 *
 * Writing to the serial port lets the kernel be tested head-less: QEMU can be
 * told to forward COM1 to stdout, so CI can boot the image and grep for a
 * known line without a display.
 */
#ifndef MINIOS_SERIAL_H
#define MINIOS_SERIAL_H

void serial_init(void);
void serial_putc(char c);
void serial_puts(const char *s);

#endif /* MINIOS_SERIAL_H */
