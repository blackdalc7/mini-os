/* util.c - implementation of the freestanding helpers (see util.h). */
#include "util.h"

size_t k_strlen(const char *s)
{
    size_t n = 0;
    while (s[n] != '\0') {
        n++;
    }
    return n;
}

void *k_memset(void *dst, int c, size_t n)
{
    unsigned char *p = (unsigned char *)dst;
    for (size_t i = 0; i < n; i++) {
        p[i] = (unsigned char)c;
    }
    return dst;
}

char *k_utoa(uint32_t value, char *buf, int base)
{
    static const char digits[] = "0123456789abcdef";

    if (base < 2 || base > 16) {
        buf[0] = '\0';
        return buf;
    }

    /* Write digits least-significant first, then reverse. */
    char tmp[33];
    int i = 0;
    do {
        tmp[i++] = digits[value % (uint32_t)base];
        value /= (uint32_t)base;
    } while (value != 0);

    int j = 0;
    while (i > 0) {
        buf[j++] = tmp[--i];
    }
    buf[j] = '\0';
    return buf;
}
