#ifndef STC_UTF8_INCLUDED
#define STC_UTF8_INCLUDED

#include "ccommon.h"
#include <ctype.h>

enum utf8_state {
    utf8_ACCEPT = 0,
    utf8_REJECT = 12
};

STC_API uint32_t utf8_decode(uint32_t *state, uint32_t *codep, const uint32_t byte);
STC_API bool utf8_valid_codepoints(const uint8_t *s, size_t *count);

STC_INLINE bool utf8_is_valid(const char *s)
{
    size_t count;
    return utf8_valid_codepoints((const uint8_t *)s, &count);
}

STC_INLINE uint32_t utf8_peek(const char *s)
{
    uint32_t state = utf8_ACCEPT, codepoint;
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

// assumes input is valid utf8! Use utf8_valid_codepoints() if unsure.
STC_INLINE size_t utf8_size(const char *s)
{
    size_t count = 0;
    while (*s)
        s += utf8_codepoint_width((uint8_t)*s), ++count;
    return count;
}


// --------------------------- IMPLEMENTATION ---------------------------------
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
    const uint32_t x = (uint32_t) -(*state != utf8_ACCEPT);

    *codep = (x & ((byte & 0x3fu) | (*codep << 6)))  
           | (~x & ((0xff >> type) & byte));

    return *state = utf8_table[256 + *state + type];
}

STC_DEF bool utf8_valid_codepoints(const uint8_t *s, size_t *count)
{
    uint32_t state = utf8_ACCEPT, codepoint;
    
    for (*count = 0; *s; ++s)
        *count += utf8_decode(&state, &codepoint, *s) == utf8_ACCEPT;
    return state == utf8_ACCEPT;
}

#endif
#endif
#undef i_opt
