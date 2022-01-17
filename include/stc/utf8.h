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

enum utf8_state {
    utf8_ACCEPT = 0,
    utf8_REJECT = 12
};

/* number of codepoints in the utf8 string s, or SIZE_MAX if invalid utf8: */
STC_API size_t utf8_size(const char *s);
STC_API size_t utf8_size_n(const char *s, size_t n);
STC_API const char* utf8_at(const char *s, size_t index);

/* decode next utf8 codepoint. */
STC_API uint32_t utf8_decode(uint32_t *state, uint32_t *codep, const uint32_t byte);

STC_INLINE size_t utf8_pos(const char* s, size_t index) 
    { return utf8_at(s, index) - s; }

STC_INLINE bool utf8_valid(const char* s)
    { return utf8_size(s) != SIZE_MAX; }

STC_INLINE uint32_t utf8_peek(const char *s) {
    uint32_t state = 0, codepoint;
    utf8_decode(&state, &codepoint, (uint8_t)*s);
    return codepoint;
}

STC_INLINE size_t utf8_codepoint_size(const char* s) {
    uint8_t u = *(const uint8_t *)s;
    size_t ret = (u & 0xF0) == 0xE0;
    ret += ret << 1;                       // 3
    ret |= u < 0x80;                       // 1
    ret |= ((0xC1 < u) & (u < 0xE0)) << 1; // 2
    ret |= ((0xEF < u) & (u < 0xF5)) << 2; // 4
    return ret;
}

STC_INLINE const char *utf8_next(const char *s) {
    const char* t = s + utf8_codepoint_size(s);
    
    uintptr_t p = (uintptr_t)t;
    p &= (uintptr_t) -(*s != 0);
    return (const char *)p;
}

// --------------------------- IMPLEMENTATION ---------------------------------
// Copyright (c) 2008-2009 Bjoern Hoehrmann <bjoern@hoehrmann.de>
// See http://bjoern.hoehrmann.de/utf-8/decoder/dfa/ for details.
#ifdef _i_implement

static const uint8_t utf8_table[] = {
     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
     0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
     1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, 9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,
     7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7, 7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
     8,8,2,2,2,2,2,2,2,2,2,2,2,2,2,2, 2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
    10,3,3,3,3,3,3,3,3,3,3,3,3,4,3,3,11,6,6,6,5,8,8,8,8,8,8,8,8,8,8,8,
     0,12,24,36,60,96,84,12,12,12,48,72, 12,12,12,12,12,12,12,12,12,12,12,12,
    12, 0,12,12,12,12,12, 0,12, 0,12,12, 12,24,12,12,12,12,12,24,12,24,12,12,
    12,12,12,12,12,12,12,24,12,12,12,12, 12,24,12,12,12,12,12,12,12,24,12,12,
    12,12,12,12,12,12,12,36,12,36,12,12, 12,36,12,12,12,12,12,36,12,36,12,12,
    12,36,12,12,12,12,12,12,12,12,12,12,
};

STC_DEF uint32_t utf8_decode(uint32_t *state, uint32_t *codep,
                             const uint32_t byte)
{
    const uint32_t type = utf8_table[byte];
    const uint32_t x = (uint32_t) -(*state != 0);

    *codep = (x & ((byte & 0x3fu) | (*codep << 6)))  
           | (~x & ((0xff >> type) & byte));

    return *state = utf8_table[256 + *state + type];
}


STC_DEF size_t utf8_size(const char *s)
{
    uint32_t state = 0, codepoint;
    size_t size = 0;

    while (*s)
        size += !utf8_decode(&state, &codepoint, (uint8_t)*s++);
    return size | (size_t) -(state != 0);
}

STC_DEF size_t utf8_size_n(const char *s, size_t n)
{
    uint32_t state = 0, codepoint;
    size_t size = 0;

    while ((n-- != 0) & (*s != 0))
        size += !utf8_decode(&state, &codepoint, (uint8_t)*s++);
    return size | (size_t) -(state != 0);
}

STC_DEF const char* utf8_at(const char *s, size_t index)
{
    uint32_t state = 0, codepoint;

    for (size_t k = 0; (k < index) & (*s != 0); ++s)
        k += !utf8_decode(&state, &codepoint, (uint8_t)*s);
    return s;
}

#endif
#endif
#undef i_opt
