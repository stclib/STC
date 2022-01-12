#ifndef UTF8_H_INCLUDED
#define UTF8_H_INCLUDED

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

STC_INLINE bool utf8_valid(const char* str)
    { return utf8_size(str) != SIZE_MAX; }

STC_INLINE uint32_t utf8_peek(const char *s)
{
    uint32_t state = 0, codepoint;
    utf8_decode(&state, &codepoint, (uint8_t)s[0]);
    return codepoint;
}

STC_INLINE int utf8_codepoint_width(uint8_t c)
{
    int ret = (c & 0xF0) == 0xE0;
    ret += ret << 1;                       // 3
    ret |= c < 0x80;                       // 1
    ret |= ((0xC1 < c) & (c < 0xE0)) << 1; // 2
    ret |= ((0xEF < c) & (c < 0xF5)) << 2; // 4
    return ret;
}

STC_INLINE const char *utf8_next(const char *s)
{
    const char* t = s + utf8_codepoint_width((uint8_t)s[0]);
    
    uintptr_t p = (uintptr_t)t;
    p &= (uintptr_t) -(*s != 0);
    return (const char *)p;
}

#ifdef CSVIEW_H_INCLUDED
STC_INLINE size_t csview_size_utf8(csview sv)
    { return utf8_size(sv.str); }

STC_INLINE bool csview_valid_utf8(csview sv)
    { return utf8_valid(sv.str); }

STC_INLINE csview csview_from_utf8cp(const char* str)
    { return c_make(csview){str, utf8_codepoint_width((uint8_t)str[0])}; }

STC_INLINE csview csview_at_utf8(const char* str, size_t idx)
    { return csview_from_utf8cp(utf8_at(str, idx)); }
#endif

#ifdef CSTR_H_INCLUDED
STC_INLINE size_t cstr_size_utf8(cstr s)
    { return utf8_size(cstr_str(&s)); }

STC_INLINE bool cstr_valid_utf8(cstr s)
    { return utf8_valid(cstr_str(&s)); }

#endif

// --------------------------- IMPLEMENTATION ---------------------------------
// Copyright (c) 2008-2009 Bjoern Hoehrmann <bjoern@hoehrmann.de>
// See http://bjoern.hoehrmann.de/utf-8/decoder/dfa/ for details.

#ifdef _i_implement
#ifdef _i_static
static
#endif
const uint8_t utf8_table[] = {
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
   
    for (size_t size = 0; (size < index) & (*s != 0); ++s)
        size += !utf8_decode(&state, &codepoint, (uint8_t)*s);
    return s;
}


#endif
#endif
#undef i_opt
