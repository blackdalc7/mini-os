/* util.h - small freestanding helpers.
 *
 * These have no dependency on a C library, so they compile both into the
 * freestanding kernel and into the host test program, where they are checked
 * with assertions.
 */
#ifndef MINIOS_UTIL_H
#define MINIOS_UTIL_H

#include <stddef.h>
#include <stdint.h>

/* Length of a NUL-terminated string. */
size_t k_strlen(const char *s);

/* Fill n bytes at dst with byte value c; returns dst. */
void *k_memset(void *dst, int c, size_t n);

/* Convert an unsigned 32-bit value to a string in the given base (2-16).
 * buf must hold at least 33 bytes. Returns buf. */
char *k_utoa(uint32_t value, char *buf, int base);

#endif /* MINIOS_UTIL_H */
