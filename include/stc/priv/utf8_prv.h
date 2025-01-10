/* MIT License
 *
 * Copyright (c) 2025 Tyge Løvset
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
// IWYU pragma: private, include "stc/utf8.h"
#ifndef STC_UTF8_PRV_H_INCLUDED
#define STC_UTF8_PRV_H_INCLUDED

// The following functions assume valid utf8 strings:

/* number of bytes in the utf8 codepoint from s */
STC_INLINE int utf8_chr_size(const char *s) {
    unsigned b = (uint8_t)*s;
    if (b < 0x80) return 1;
    /*if (b < 0xC2) return 0;*/
    if (b < 0xE0) return 2;
    if (b < 0xF0) return 3;
    /*if (b < 0xF5)*/ return 4;
    /*return 0;*/
}

/* number of codepoints in the utf8 string s */
STC_INLINE isize utf8_count(const char *s) {
    isize size = 0;
    while (*s)
        size += (*++s & 0xC0) != 0x80;
    return size;
}

STC_INLINE isize utf8_count_n(const char *s, isize nbytes) {
    isize size = 0;
    while ((nbytes-- != 0) & (*s != 0)) {
        size += (*++s & 0xC0) != 0x80;
    }
    return size;
}

STC_INLINE const char* utf8_at(const char *s, isize u8pos) {
    while ((u8pos > 0) & (*s != 0))
        u8pos -= (*++s & 0xC0) != 0x80;
    return s;
}

STC_INLINE const char* utf8_offset(const char* s, isize u8pos) {
    int inc = 1;
    if (u8pos < 0) u8pos = -u8pos, inc = -1;
    while (u8pos && *s)
        u8pos -= (*(s += inc) & 0xC0) != 0x80;
    return s;
}

STC_INLINE isize utf8_to_index(const char* s, isize u8pos)
    { return utf8_at(s, u8pos) - s; }

STC_INLINE csview utf8_subview(const char *s, isize u8pos, isize u8len) {
    csview span;
    span.buf = utf8_at(s, u8pos);
    span.size = utf8_to_index(span.buf, u8len);
    return span;
}

// ------------------------------------------------------
// The following requires linking with utf8 symbols.
// To call them, either define i_import before including
// one of cstr, csview, zsview, or link with src/libstc.o.

extern bool     utf8_valid_n(const char* s, isize nbytes);
extern int      utf8_encode(char *out, uint32_t c);
extern int      utf8_icompare(const csview s1, const csview s2);
extern uint32_t utf8_peek_at(const char* s, isize u8offset);
extern uint32_t utf8_casefold(uint32_t c);
extern uint32_t utf8_tolower(uint32_t c);
extern uint32_t utf8_toupper(uint32_t c);

STC_INLINE bool utf8_isupper(uint32_t c)
    { return utf8_tolower(c) != c; }

STC_INLINE bool utf8_islower(uint32_t c)
    { return utf8_toupper(c) != c; }


/* decode next utf8 codepoint. https://bjoern.hoehrmann.de/utf-8/decoder/dfa */
typedef struct { uint32_t state, codep; } utf8_decode_t;
extern const uint8_t utf8_dtab[]; /* utf8code.c */

STC_INLINE uint32_t utf8_decode(utf8_decode_t* d, const uint32_t byte) {
    const uint32_t type = utf8_dtab[byte];
    d->codep = d->state ? (byte & 0x3fu) | (d->codep << 6)
                        : (0xffU >> type) & byte;
    return d->state = utf8_dtab[256 + d->state + type];
}

STC_INLINE uint32_t utf8_peek(const char* s) {
    utf8_decode_t d = {.state=0};
    do { utf8_decode(&d, (uint8_t)*s++); } while (d.state);
    return d.codep;
}

/* case-insensitive utf8 string comparison */
STC_INLINE int utf8_icmp(const char* s1, const char* s2) {
    return utf8_icompare(c_sv(s1, INTPTR_MAX), c_sv(s2, INTPTR_MAX));
}

STC_INLINE bool utf8_valid(const char* s) {
    return utf8_valid_n(s, INTPTR_MAX);
}

#endif // STC_UTF8_PRV_H_INCLUDED
