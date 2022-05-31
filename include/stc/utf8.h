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

        c_foreach (i, cstr, s1)
            printf("%" c_PRIsv ",", c_ARGsv(i.chr));
    }
}
// Output:
// hell😀 w😀rld
// hell😀 w🐨rld
// h,e,l,l,😀, ,w,🐨,r,l,d,
*/
#include "ccommon.h"
#include <ctype.h>

// utf8 methods defined in src/utf8code.c:
bool        utf8_islower(uint32_t c);
bool        utf8_isupper(uint32_t c);
bool        utf8_isspace(uint32_t c);
bool        utf8_isdigit(uint32_t c);
bool        utf8_isxdigit(uint32_t c);
bool        utf8_isalpha(uint32_t c);
bool        utf8_isalnum(uint32_t c);
uint32_t    utf8_tolower(uint32_t c);
uint32_t    utf8_toupper(uint32_t c);

bool        utf8_valid(const char* s);
bool        utf8_valid_n(const char* s, size_t n);

int         utf8_icmp_n(size_t u8max, const char* s1, size_t n1,
                                      const char* s2, size_t n2);
/* encode/decode next utf8 codepoint. */
enum { UTF8_OK = 0, UTF8_ERROR = 4 };
typedef struct { uint32_t state, codep, size; } utf8_decode_t;

void        utf8_peek(utf8_decode_t* d, const char *s);
unsigned    utf8_encode(char *out, uint32_t c);
void        utf8_decode(utf8_decode_t *d, const uint8_t b);

/* case-insensitive utf8 string comparison */
STC_INLINE int utf8_icmp(const char* s1, const char* s2) {
    return utf8_icmp_n(~(size_t)0, s1, ~(size_t)0, s2, ~(size_t)0);
}

/* number of characters in the utf8 codepoint from s */
STC_INLINE unsigned utf8_codep_size(const char *s) {
    unsigned b = (uint8_t)*s;
    if (b < 0x80) return 1;
    if (b < 0xC2) return 0;
    if (b < 0xE0) return 2;
    if (b < 0xF0) return 3;
    if (b < 0xF5) return 4;
    return 0;
}

/* number of codepoints in the utf8 string s */
STC_INLINE size_t utf8_size(const char *s) {
    size_t size = 0;
    while (*s)
        size += (*s++ & 0xC0) != 0x80;
    return size;
}

STC_INLINE size_t utf8_size_n(const char *s, size_t n) {
    size_t size = 0;
    while ((n-- != 0) & (*s != 0))
        size += (*s++ & 0xC0) != 0x80;
    return size;
}

STC_INLINE const char* utf8_at(const char *s, size_t index) {
    while ((index > 0) & (*s != 0))
        index -= (*++s & 0xC0) != 0x80;
    return s;
}

STC_INLINE size_t utf8_pos(const char*   s, size_t index)
    { return utf8_at(s, index) - s; }

#endif
