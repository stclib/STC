#ifndef UTF8_H_INCLUDED
#define UTF8_H_INCLUDED

#include "ccommon.h"
#include <ctype.h>

// utf8 methods defined in src/utf8code.c:

extern bool     utf8_isspace(uint32_t c);
extern bool     utf8_isdigit(uint32_t c);
extern bool     utf8_isxdigit(uint32_t c);
extern bool     utf8_isalpha(uint32_t c);
extern bool     utf8_isalnum(uint32_t c);
extern uint32_t utf8_casefold(uint32_t c);
extern uint32_t utf8_tolower(uint32_t c);
extern uint32_t utf8_toupper(uint32_t c);
extern bool     utf8_valid_n(const char* s, size_t nbytes);
extern int      utf8_icmp_sv(csview s1, csview s2);
extern unsigned utf8_encode(char *out, uint32_t c);
extern uint32_t utf8_peek_off(const char *s, int offset);

/* following functions uses src/utf8code.c */

STC_INLINE bool utf8_isupper(uint32_t c) { return utf8_tolower(c) != c; }
STC_INLINE bool utf8_islower(uint32_t c) { return utf8_toupper(c) != c; }

/* decode next utf8 codepoint. https://bjoern.hoehrmann.de/utf-8/decoder/dfa */
typedef struct { uint32_t state, codep; } utf8_decode_t;

STC_INLINE uint32_t utf8_decode(utf8_decode_t* d, const uint32_t byte) {
    extern const uint8_t utf8_dtab[]; /* utf8code.c */
    const uint32_t type = utf8_dtab[byte];
    d->codep = d->state ? (byte & 0x3fu) | (d->codep << 6)
                        : (0xff >> type) & byte;
    return d->state = utf8_dtab[256 + d->state + type];
}

STC_INLINE uint32_t utf8_peek(const char* s) {
    utf8_decode_t d = {.state=0};
    do { utf8_decode(&d, (uint8_t)*s++); } while (d.state);
    return d.codep;
}

/* case-insensitive utf8 string comparison */
STC_INLINE int utf8_icmp(const char* s1, const char* s2) {
    return utf8_icmp_sv(c_sv(s1, ~(size_t)0), c_sv(s2, ~(size_t)0));
}

STC_INLINE bool utf8_valid(const char* s) {
    return utf8_valid_n(s, ~(size_t)0);
}

/* following functions are independent but assume valid utf8 strings: */

/* number of bytes in the utf8 codepoint from s */
STC_INLINE unsigned utf8_chr_size(const char *s) {
    unsigned b = (uint8_t)*s;
    if (b < 0x80) return 1;
    /*if (b < 0xC2) return 0;*/
    if (b < 0xE0) return 2;
    if (b < 0xF0) return 3;
    /*if (b < 0xF5)*/ return 4;
    /*return 0;*/
}

/* number of codepoints in the utf8 string s */
STC_INLINE size_t utf8_size(const char *s) {
    size_t size = 0;
    while (*s)
        size += (*++s & 0xC0) != 0x80;
    return size;
}

STC_INLINE size_t utf8_size_n(const char *s, size_t nbytes) {
    size_t size = 0;
    while ((nbytes-- != 0) & (*s != 0)) {
        size += (*++s & 0xC0) != 0x80;
    }
    return size;
}

STC_INLINE const char* utf8_at(const char *s, size_t index) {
    while ((index > 0) & (*s != 0))
        index -= (*++s & 0xC0) != 0x80;
    return s;
}

STC_INLINE size_t utf8_pos(const char* s, size_t index)
    { return utf8_at(s, index) - s; }

#endif
