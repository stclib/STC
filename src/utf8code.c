#include <ctype.h>
#define i_header
#include <stc/cstr.h>
#include "utf8tabs.inc"

const uint8_t utf8_dtab[] = {
   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
   1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,  9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,
   7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,  7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
   8,8,2,2,2,2,2,2,2,2,2,2,2,2,2,2,  2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
  10,3,3,3,3,3,3,3,3,3,3,3,3,4,3,3, 11,6,6,6,5,8,8,8,8,8,8,8,8,8,8,8,
   0,12,24,36,60,96,84,12,12,12,48,72, 12,12,12,12,12,12,12,12,12,12,12,12,
  12, 0,12,12,12,12,12, 0,12, 0,12,12, 12,24,12,12,12,12,12,24,12,24,12,12,
  12,12,12,12,12,12,12,24,12,12,12,12, 12,24,12,12,12,12,12,12,12,24,12,12,
  12,12,12,12,12,12,12,36,12,36,12,12, 12,36,12,12,12,12,12,36,12,36,12,12,
  12,36,12,12,12,12,12,12,12,12,12,12, 
};

unsigned utf8_encode(char *out, uint32_t c)
{
    if (c < 0x80U) {
        out[0] = (char) c;
        return 1;
    } else if (c < 0x0800U) {
        out[0] = (char) ((c>>6  & 0x1F) | 0xC0);
        out[1] = (char) ((c     & 0x3F) | 0x80);
        return 2;
    } else if (c < 0x010000U) {
        if ((c < 0xD800U) | (c >= 0xE000U)) {
            out[0] = (char) ((c>>12 & 0x0F) | 0xE0);
            out[1] = (char) ((c>>6  & 0x3F) | 0x80);
            out[2] = (char) ((c     & 0x3F) | 0x80);
            return 3;
        }
    } else if (c < 0x110000U) {
        out[0] = (char) ((c>>18 & 0x07) | 0xF0);
        out[1] = (char) ((c>>12 & 0x3F) | 0x80);
        out[2] = (char) ((c>>6  & 0x3F) | 0x80);
        out[3] = (char) ((c     & 0x3F) | 0x80);
        return 4;
    }
    return 0;
}

uint32_t utf8_peek(const char* s, int pos) {
    int inc = -1;
    if (pos > 0)
        pos = -pos, inc = 1;
    while (pos)
        pos += (*(s += inc) & 0xC0) != 0x80;
    utf8_decode_t d = {.state=0};
    do { utf8_decode(&d, (uint8_t)*s++); } while (d.state);
    return d.codep;
}

bool utf8_valid_n(const char* s, size_t nbytes) {
    utf8_decode_t d = {.state=0};
    while ((nbytes-- != 0) & (*s != 0))
        utf8_decode(&d, (uint8_t)*s++);
    return d.state == 0;
}

uint32_t utf8_casefold(uint32_t c) {
    for (size_t i=0; i < casefold_len; ++i) {
        const struct CaseMapping entry = casemappings[i];
        if (c <= entry.c2) {
            if (c < entry.c1) return c;
            int d = entry.m2 - entry.c2;
            if (d == 1) return c + ((entry.c2 & 1) == (c & 1));
            return c + d;
        }
    }
    return c;
}

uint32_t utf8_tolower(uint32_t c) {
    for (size_t i=0; i < sizeof upcase_ind/sizeof *upcase_ind; ++i) {
        const struct CaseMapping entry = casemappings[upcase_ind[i]];
        if (c <= entry.c2) {
            if (c < entry.c1) return c;
            int d = entry.m2 - entry.c2;
            if (d == 1) return c + ((entry.c2 & 1) == (c & 1));
            return c + d;
        }
    }
    return c;
}

uint32_t utf8_toupper(uint32_t c) {
    for (size_t i=0; i < sizeof lowcase_ind/sizeof *lowcase_ind; ++i) {
        const struct CaseMapping entry = casemappings[lowcase_ind[i]];
        if (c <= entry.m2) {
            int d = entry.m2 - entry.c2;
            if (c < (uint32_t)(entry.c1 + d)) return c;
            if (d == 1) return c - ((entry.m2 & 1) == (c & 1));
            return c - d;
        }
    }
    return c;
}
/*
int utf8_icmp(const char* s1, const char* s2) {
    utf8_decode_t d1 = {.state=0}, d2 = {.state=0};
    for (;;) {
        do { utf8_decode(&d1, (uint8_t)*s1++); } while (d1.state);
        do { utf8_decode(&d2, (uint8_t)*s2++); } while (d2.state);
        int c = utf8_casefold(d1.codep) - utf8_casefold(d2.codep);
        if (c || !*s2)
            return c;
    }
}
*/
int utf8_icmp_n(size_t u8max, const char* s1, const size_t n1,
                              const char* s2, const size_t n2) {
    utf8_decode_t d1 = {.state=0}, d2 = {.state=0};
    size_t j1 = 0, j2 = 0;
    while (u8max-- && ((j1 < n1) & (j2 < n2))) {
        do { utf8_decode(&d1, (uint8_t)s1[j1++]); } while (d1.state);
        do { utf8_decode(&d2, (uint8_t)s2[j2++]); } while (d2.state);
        int c = utf8_casefold(d1.codep) - utf8_casefold(d2.codep);
        if (c || !s2[j2])
            return c;
    }
    return (j2 < n2) - (j1 < n1);
}

bool utf8_isupper(uint32_t c) {
    return utf8_tolower(c) != c;
}

bool utf8_islower(uint32_t c) {
    return utf8_toupper(c) != c;
}

bool utf8_isspace(uint32_t c) {
    static uint16_t t[] = {0x20, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x85, 0xA0,
                           0x1680, 0x2028, 0x2029, 0x202F, 0x205F, 0x3000};
    for (size_t i=0; i<sizeof t/sizeof *t; ++i)
        if (c == t[i]) return true;
    return (c >= 0x2000) & (c <= 0x200A);
}

bool utf8_isdigit(uint32_t c) {
    return ((c >= '0') & (c <= '9')) ||
           ((c >= 0xFF10) & (c <= 0xFF19));
}

bool utf8_isxdigit(uint32_t c) {
    static uint16_t t[] = {0x30, 0x39, 0x41, 0x46, 0x61, 0x66, 0xFF10,
                           0xFF19, 0xFF21, 0xFF26, 0xFF41, 0xFF46};
    for (size_t i=1; i<sizeof t/sizeof *t; i += 2)
        if (c <= t[i]) return c >= t[i - 1];
    return false;
}

bool utf8_isalnum(uint32_t c) {
    if (c < 128) return isalnum(c) != 0;
    if ((c >= 0xFF10) & (c <= 0xFF19)) return true;
    return utf8_islower(c) || utf8_isupper(c);
}

bool utf8_isalpha(uint32_t c) {
    if (c < 128) return isalpha(c) != 0;
    return utf8_islower(c) || utf8_isupper(c);
}

static struct fncase {
    int      (*conv_asc)(int);
    uint32_t (*conv_utf)(uint32_t);
}
fn_tofold = {tolower, utf8_casefold},
fn_tolower = {tolower, utf8_tolower},
fn_toupper = {toupper, utf8_toupper};


static cstr cstr_tocase(const cstr* self, struct fncase fn) {
    csview sv = cstr_sv(self);
    cstr out = cstr_null;
    char *buf = cstr_reserve(&out, sv.size*3/2);
    uint32_t cp; size_t sz = 0;
    utf8_decode_t d = {.state=0};

    while (*sv.str) {
        do { utf8_decode(&d, (uint8_t)*sv.str++); } while (d.state);
        if (d.codep < 128)
            buf[sz++] = (char)fn.conv_asc(d.codep);
        else {
            cp = fn.conv_utf(d.codep);
            sz += utf8_encode(buf + sz, cp);
        }
    }
    _cstr_set_size(&out, sz);
    cstr_shrink_to_fit(&out);
    return out;
}

cstr cstr_tofold(const cstr* self) {
    return cstr_tocase(self, fn_tofold);
}

cstr cstr_tolower(const cstr* self) {
    return cstr_tocase(self, fn_tolower);
}

cstr cstr_toupper(const cstr* self) {
    return cstr_tocase(self, fn_toupper);
}

void cstr_casefold(cstr* self) {
    cstr_take(self, cstr_tocase(self, fn_tofold));
}

void cstr_lowercase(cstr* self) {
    cstr_take(self, cstr_tocase(self, fn_tolower));
}

void cstr_uppercase(cstr* self) {
    cstr_take(self, cstr_tocase(self, fn_toupper));
}
