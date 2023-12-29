/* MIT License
 *
 * Copyright (c) 2023 Tyge Løvset
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
#ifndef STC_UTF8_PRV_H_INCLUDED
#define STC_UTF8_PRV_H_INCLUDED

#include <ctype.h>

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
STC_INLINE intptr_t utf8_size(const char *s) {
    intptr_t size = 0;
    while (*s)
        size += (*++s & 0xC0) != 0x80;
    return size;
}

STC_INLINE intptr_t utf8_size_n(const char *s, intptr_t nbytes) {
    intptr_t size = 0;
    while ((nbytes-- != 0) & (*s != 0)) {
        size += (*++s & 0xC0) != 0x80;
    }
    return size;
}

STC_INLINE const char* utf8_at(const char *s, intptr_t index) {
    while ((index > 0) & (*s != 0))
        index -= (*++s & 0xC0) != 0x80;
    return s;
}

STC_INLINE intptr_t utf8_pos(const char* s, intptr_t index)
    { return (intptr_t)(utf8_at(s, index) - s); }

// ------------------------------------------------------
// The following requires linking with utf8 symbols.
// To call them, either define i_import before including
// one of cstr, csview czview, or link with src/libstc.o.

enum {
    U8G_Cc, U8G_Lt, U8G_Nd, U8G_Nl,
    U8G_Pc, U8G_Pd, U8G_Pf, U8G_Pi,
    U8G_Sc, U8G_Zl, U8G_Zp, U8G_Zs,
    U8G_Arabic, U8G_Cyrillic,
    U8G_Devanagari, U8G_Greek,
    U8G_Han, U8G_Latin,
    U8G_SIZE
};

extern bool     utf8_isgroup(int group, uint32_t c);
extern bool     utf8_isalpha(uint32_t c);
extern uint32_t utf8_casefold(uint32_t c);
extern uint32_t utf8_tolower(uint32_t c);
extern uint32_t utf8_toupper(uint32_t c);
extern bool     utf8_iscased(uint32_t c);
extern bool     utf8_isword(uint32_t c);
extern bool     utf8_valid_n(const char* s, intptr_t nbytes);
extern int      utf8_icmp_sv(csview s1, csview s2);
extern int      utf8_encode(char *out, uint32_t c);
extern uint32_t utf8_peek_off(const char *s, int offset);

STC_INLINE bool utf8_isupper(uint32_t c)
    { return utf8_tolower(c) != c; }

STC_INLINE bool utf8_islower(uint32_t c)
    { return utf8_toupper(c) != c; }

STC_INLINE bool utf8_isalnum(uint32_t c) {
    if (c < 128) return isalnum((int)c) != 0;
    return utf8_isalpha(c) || utf8_isgroup(U8G_Nd, c);
}

STC_INLINE bool utf8_isblank(uint32_t c) {
    if (c < 128) return (c == ' ') | (c == '\t');
    return utf8_isgroup(U8G_Zs, c);
}

STC_INLINE bool utf8_isspace(uint32_t c) {
    if (c < 128) return isspace((int)c) != 0;
    return ((c == 8232) | (c == 8233)) || utf8_isgroup(U8G_Zs, c);
}

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
    return utf8_icmp_sv(c_sv(s1, INTPTR_MAX), c_sv(s2, INTPTR_MAX));
}

STC_INLINE bool utf8_valid(const char* s) {
    return utf8_valid_n(s, INTPTR_MAX);
}

#endif // STC_UTF8_PRV_H_INCLUDED
