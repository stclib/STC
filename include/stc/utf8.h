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
        cstr_replace_sv(&s1, utf8_substr(s1.str, 7, 1), c_sv("x"));
        printf("%s\n", s1.str);

        csview sv = csview_from_s(s1);
        c_foreach (i, csview, sv)
            printf(c_PRIsv ",", c_ARGsv(i.cp));
    }
}
// Output:
// hell😀 wxrld
// h,e,l,l,😀, ,w,x,r,l,d,
*/
#include "ccommon.h"
#include <ctype.h>

/* number of codepoints in the utf8 string s, or SIZE_MAX if invalid utf8: */
enum { UTF8_OK = 0, UTF8_ERROR = 4 };
typedef struct { uint32_t state, codep, len; } utf8_decode_t;

/* decode next utf8 codepoint. */
STC_API uint32_t utf8_decode(utf8_decode_t *c, const uint8_t b);
STC_API const uint8_t* utf8_nextc(utf8_decode_t *c, const uint8_t* s);
STC_API size_t utf8_size(const char *s);
STC_API size_t utf8_size_n(const char *s, size_t n);
STC_API const char* utf8_at(const char *s, size_t index);

STC_INLINE size_t utf8_pos(const char* s, size_t index) 
    { return utf8_at(s, index) - s; }

STC_INLINE bool utf8_valid(const char* s)
    { return utf8_size(s) != SIZE_MAX; }

STC_INLINE uint32_t utf8_peek(const char *s) {
    utf8_decode_t ctx = {UTF8_OK, 0};
    utf8_nextc(&ctx, (const uint8_t*)s);
    return ctx.codep;
}

STC_INLINE size_t utf8_codep_size(const char *s) {
    utf8_decode_t ctx = {UTF8_OK, 0};
    utf8_nextc(&ctx, (const uint8_t*)s);
    return ctx.len;
}

STC_INLINE const char* utf8_next(const char *s) {
    if (!*s) return NULL;
    utf8_decode_t ctx = {UTF8_OK, 0};
    return (const char*) utf8_nextc(&ctx, (const uint8_t*)s);
}

// --------------------------- IMPLEMENTATION ---------------------------------
#ifdef _i_implement
// https://news.ycombinator.com/item?id=15423674
// https://gist.github.com/s4y/344a355f8c1f99c6a4cb2347ec4323cc

STC_DEF uint32_t utf8_decode(utf8_decode_t *c, const uint8_t b)
{
    switch (c->state) {
    case UTF8_OK:
        if      (b < 0x80) c->codep = b, c->len = 1;
        else if (b < 0xc2) c->state = UTF8_ERROR;
        else if (b < 0xe0) c->state = 1, c->codep = b & 0x1f, c->len = 2;
        else if (b < 0xf0) c->state = 2, c->codep = b & 0xf, c->len = 3;
        else if (b < 0xf5) c->state = 3, c->codep = b & 0x7, c->len = 4;
        else c->state = UTF8_ERROR;
        break;
    case 1: case 2: case 3:
        if ((b & 0xc0) == 0x80) {
            c->state -= 1;
            c->codep = (c->codep << 6) | (b & 0x3f);
        } else
            c->state = UTF8_ERROR;
    }
    return c->state;
}

STC_DEF const uint8_t* utf8_nextc(utf8_decode_t *c, const uint8_t* s) {
    utf8_decode(c, *s++);
    switch (c->len) {
        case 4: utf8_decode(c, *s++);
        case 3: utf8_decode(c, *s++);
        case 2: utf8_decode(c, *s++);
    }
    return s;
}

STC_DEF size_t utf8_size(const char *s)
{
    utf8_decode_t ctx = {UTF8_OK, 0};
    size_t size = 0;
    while (*s)
        size += !utf8_decode(&ctx, (uint8_t)*s++);
    return !ctx.state ? size : SIZE_MAX;
}

STC_DEF size_t utf8_size_n(const char *s, size_t n)
{
    utf8_decode_t ctx = {UTF8_OK, 0};
    size_t size = 0;
    while ((n-- != 0) & (*s != 0))
        size += !utf8_decode(&ctx, (uint8_t)*s++);
    return !ctx.state ? size : SIZE_MAX;
}

STC_DEF const char* utf8_at(const char *s, size_t index)
{
    utf8_decode_t ctx = {UTF8_OK, 0};
    for (size_t i = 0; (i < index) & (*s != 0); ++s)
        i += !utf8_decode(&ctx, (uint8_t)*s);
    return s;
}

#endif
#endif
#undef i_opt
