#ifndef UTF8_H_INCLUDED
#define UTF8_H_INCLUDED
/*
// Example:
#include <stc/cstr.h>
#include <stc/csview.h>

int main()
{
    c_auto (cstr, s1) {
        s1 = cstr_new("hell😀 w😀rld");
        printf("%s\n", cstr_str(&s1));
        cstr_replace_sv(&s1, utf8_substr(cstr_str(&s1), 7, 1), c_sv("🐨"));
        printf("%s\n", cstr_str(&s1));

        csview sv = csview_from_s(s1);
        c_foreach (i, csview, sv)
            printf(c_PRIsv ",", c_ARGsv(i.codep));
    }
}
// Output:
// hell😀 w😀rld
// hell😀 w🐨rld
// h,e,l,l,😀, ,w,🐨,r,l,d,
*/
#include "ccommon.h"
#include <ctype.h>

/* number of codepoints in the utf8 string s, or SIZE_MAX if invalid utf8: */
enum { UTF8_OK = 0, UTF8_ERROR = 4 };
typedef struct { uint32_t state, codep, size; } utf8_decode_t;

/* decode next utf8 codepoint. */
STC_API size_t          utf8_encode(char *out, uint32_t c);
STC_API uint32_t        utf8_decode(utf8_decode_t *d, const uint8_t b);
STC_API const uint8_t*  utf8_next(utf8_decode_t *d, const uint8_t* u);
STC_API size_t          utf8_size(const char *s);
STC_API size_t          utf8_size_n(const char *s, size_t n);
STC_API const char*     utf8_at(const char *s, size_t index);

STC_INLINE size_t utf8_pos(const char* s, size_t index) 
    { return utf8_at(s, index) - s; }

STC_INLINE bool utf8_valid(const char* s)
    { return utf8_size(s) != SIZE_MAX; }

STC_INLINE uint32_t utf8_peek(const char *s) {
    utf8_decode_t d = {UTF8_OK, 0};
    utf8_next(&d, (const uint8_t*)s);
    return d.codep;
}

STC_INLINE size_t utf8_codep_size(const char *s) {
    utf8_decode_t d = {UTF8_OK, 0};
    utf8_next(&d, (const uint8_t*)s);
    return d.size;
}

// --------------------------- IMPLEMENTATION ---------------------------------
#ifdef _i_implement
// https://news.ycombinator.com/item?id=15423674
// https://gist.github.com/s4y/344a355f8c1f99c6a4cb2347ec4323cc

STC_DEF uint32_t utf8_decode(utf8_decode_t *d, const uint8_t b)
{
    switch (d->state) {
    case UTF8_OK:
        if      (b < 0x80) d->codep = b, d->size = 1;
        else if (b < 0xC2) d->state = UTF8_ERROR, d->size = 0;
        else if (b < 0xE0) d->state = 1, d->codep = b & 0x1F, d->size = 2;
        else if (b < 0xF0) d->state = 2, d->codep = b & 0x0F, d->size = 3;
        else if (b < 0xF5) d->state = 3, d->codep = b & 0x07, d->size = 4;
        else d->state = UTF8_ERROR, d->size = 0;
        break;
    case 1: case 2: case 3:
        if ((b & 0xC0) == 0x80) {
            d->state -= 1;
            d->codep = (d->codep << 6) | (b & 0x3F);
        } else
            d->state = UTF8_ERROR, d->size = 0;
    }
    return d->state;
}

STC_DEF size_t utf8_encode(char *out, uint32_t c)
{
    char* p = out;
    if (c < 0x80U) {
        *p++ = (char) c;
    } else if (c < 0x0800U) {
        *p++ = (char) ((c>>6  & 0x1F) | 0xC0);
        *p++ = (char) ((c     & 0x3F) | 0x80);
    } else if (c < 0x010000U) {
        if (c < 0xD800U || c >= 0xE000U) {
            *p++ = (char) ((c>>12 & 0x0F) | 0xE0);
            *p++ = (char) ((c>>6  & 0x3F) | 0x80);
            *p++ = (char) ((c     & 0x3F) | 0x80);
        }
    } else if (c < 0x110000U) {
        *p++ = (char) ((c>>18 & 0x07) | 0xF0);
        *p++ = (char) ((c>>12 & 0x3F) | 0x80);
        *p++ = (char) ((c>>6  & 0x3F) | 0x80);
        *p++ = (char) ((c     & 0x3F) | 0x80);
    }
    return p - out;
}

STC_DEF const uint8_t* utf8_next(utf8_decode_t *d, const uint8_t* u) {
    utf8_decode(d, *u++);
    switch (d->size) {
        case 4: utf8_decode(d, *u++);
        case 3: utf8_decode(d, *u++);
        case 2: utf8_decode(d, *u++);
    }
    return u;
}

STC_DEF size_t utf8_size(const char *s)
{
    utf8_decode_t d = {UTF8_OK, 0};
    size_t size = 0;
    while (*s)
        size += !utf8_decode(&d, (uint8_t)*s++);
    return d.state ? SIZE_MAX : size;
}

STC_DEF size_t utf8_size_n(const char *s, size_t n)
{
    utf8_decode_t d = {UTF8_OK, 0};
    size_t size = 0;
    while ((n-- != 0) & (*s != 0))
        size += !utf8_decode(&d, (uint8_t)*s++);
    return !d.state ? size : SIZE_MAX;
}

STC_DEF const char* utf8_at(const char *s, size_t index)
{
    utf8_decode_t d = {UTF8_OK, 0};
    for (size_t i = 0; (i < index) & (*s != 0); ++s)
        i += !utf8_decode(&d, (uint8_t)*s);
    return s;
}

#endif
#endif
#undef i_opt
