#ifndef UTF8_C_INCLUDED
#define UTF8_C_INCLUDED
#include <ctype.h>
#include <stc/utf8.h> // header only
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

uint32_t utf8_peek_off(const char* s, int pos) {
    int inc = -1;
    if (pos > 0) pos = -pos, inc = 1;
    while (pos) pos += (*(s += inc) & 0xC0) != 0x80;
    return utf8_peek(s);
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
            return (uint32_t)((int)c + d);
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
            return (uint32_t)((int)c + d);
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
            return (uint32_t)((int)c - d);
        }
    }
    return c;
}

int utf8_icmp_sv(const csview s1, const csview s2) {
    utf8_decode_t d1 = {.state=0}, d2 = {.state=0};
    size_t j1 = 0, j2 = 0;
    while ((j1 < s1.size) & (j2 < s2.size)) {
        do { utf8_decode(&d1, (uint8_t)s1.str[j1++]); } while (d1.state);
        do { utf8_decode(&d2, (uint8_t)s2.str[j2++]); } while (d2.state);
        int32_t c = (int32_t)utf8_casefold(d1.codep) - (int32_t)utf8_casefold(d2.codep);
        if (c || !s2.str[j2 - 1]) // OK if s1.size and s2.size are npos
            return (int)c;
    }
    return (int)(s1.size - s2.size);
}

typedef struct {
  uint16_t lo;
  uint16_t hi;
} URange16;

typedef struct {
  const char *name;
  const URange16 *r16;
  int nr16;
} UGroup;

static const UGroup unicode_groups[];
static const int num_unicode_groups;

bool utf8_isgroup(int group, uint32_t c) {
    for (int j=0; j<unicode_groups[group].nr16; ++j) {
        if (c < unicode_groups[group].r16[j].lo)
            return false;
        if (c <= unicode_groups[group].r16[j].hi)
            return true;
    }
    return false;
}

bool utf8_iscased(uint32_t c) {
    if (c < 128) return isalpha(c) != 0;
    return utf8_islower(c) || utf8_isupper(c) || utf8_isgroup(U8G_Lt, c);
}

bool utf8_isalnum(uint32_t c) {
    if (c < 128) return isalnum(c) != 0;
    return utf8_islower(c) || utf8_isupper(c) || utf8_isgroup(U8G_Lt, c) ||
           utf8_isgroup(U8G_Nd, c) || utf8_isgroup(U8G_Nl, c);
}

bool utf8_isblank(uint32_t c) {
    if (c < 128) return (c == ' ') | (c == '\t');
    return utf8_isgroup(U8G_Zs, c);
}

bool utf8_isspace(uint32_t c) {
    if (c < 128) return isspace(c) != 0;
    return ((c == 8232) | (c == 8233)) || utf8_isgroup(U8G_Zs, c);
}

bool utf8_isword(uint32_t c) {
    if (c < 128) return (isalnum(c) != 0) | (c == '_');
    return utf8_islower(c) || utf8_isupper(c) || utf8_isgroup(U8G_Lt, c) ||
           utf8_isgroup(U8G_Nd, c) || utf8_isgroup(U8G_Pc, c) || utf8_isgroup(U8G_Nl, c);
}

static const URange16 Cc_range16[] = { // Control
    { 0, 31 },
    { 127, 159 },
};

static const URange16 Lt_range16[] = { // Title case
    { 453, 453 },
    { 456, 456 },
    { 459, 459 },
    { 498, 498 },
    { 8072, 8079 },
    { 8088, 8095 },
    { 8104, 8111 },
    { 8124, 8124 },
    { 8140, 8140 },
    { 8188, 8188 },
};

static const URange16 Nd_range16[] = { // Decimal number
    { 48, 57 },
    { 1632, 1641 },
    { 1776, 1785 },
    { 1984, 1993 },
    { 2406, 2415 },
    { 2534, 2543 },
    { 2662, 2671 },
    { 2790, 2799 },
    { 2918, 2927 },
    { 3046, 3055 },
    { 3174, 3183 },
    { 3302, 3311 },
    { 3430, 3439 },
    { 3558, 3567 },
    { 3664, 3673 },
    { 3792, 3801 },
    { 3872, 3881 },
    { 4160, 4169 },
    { 4240, 4249 },
    { 6112, 6121 },
    { 6160, 6169 },
    { 6470, 6479 },
    { 6608, 6617 },
    { 6784, 6793 },
    { 6800, 6809 },
    { 6992, 7001 },
    { 7088, 7097 },
    { 7232, 7241 },
    { 7248, 7257 },
    { 42528, 42537 },
    { 43216, 43225 },
    { 43264, 43273 },
    { 43472, 43481 },
    { 43504, 43513 },
    { 43600, 43609 },
    { 44016, 44025 },
    { 65296, 65305 },
};

static const URange16 Nl_range16[] = { // Number letter
    { 5870, 5872 },
    { 8544, 8578 },
    { 8581, 8584 },
    { 12295, 12295 },
    { 12321, 12329 },
    { 12344, 12346 },
    { 42726, 42735 },
};

static const URange16 Pc_range16[] = { // Connector punctuation
    { 95, 95 },
    { 8255, 8256 },
    { 8276, 8276 },
    { 65075, 65076 },
    { 65101, 65103 },
    { 65343, 65343 },
};

static const URange16 Pd_range16[] = { // Dash punctuation
    { 45, 45 },
    { 1418, 1418 },
    { 1470, 1470 },
    { 5120, 5120 },
    { 6150, 6150 },
    { 8208, 8213 },
    { 11799, 11799 },
    { 11802, 11802 },
    { 11834, 11835 },
    { 11840, 11840 },
    { 11869, 11869 },
    { 12316, 12316 },
    { 12336, 12336 },
    { 12448, 12448 },
    { 65073, 65074 },
    { 65112, 65112 },
    { 65123, 65123 },
    { 65293, 65293 },
};

static const URange16 Pf_range16[] = { // Final punctuation
    { 187, 187 },
    { 8217, 8217 },
    { 8221, 8221 },
    { 8250, 8250 },
    { 11779, 11779 },
    { 11781, 11781 },
    { 11786, 11786 },
    { 11789, 11789 },
    { 11805, 11805 },
    { 11809, 11809 },
};

static const URange16 Pi_range16[] = { // Initial punctuation
    { 171, 171 },
    { 8216, 8216 },
    { 8219, 8220 },
    { 8223, 8223 },
    { 8249, 8249 },
    { 11778, 11778 },
    { 11780, 11780 },
    { 11785, 11785 },
    { 11788, 11788 },
    { 11804, 11804 },
    { 11808, 11808 },
};

static const URange16 Sc_range16[] = { // Currency symbol
    { 36, 36 },
    { 162, 165 },
    { 1423, 1423 },
    { 1547, 1547 },
    { 2046, 2047 },
    { 2546, 2547 },
    { 2555, 2555 },
    { 2801, 2801 },
    { 3065, 3065 },
    { 3647, 3647 },
    { 6107, 6107 },
    { 8352, 8384 },
    { 43064, 43064 },
    { 65020, 65020 },
    { 65129, 65129 },
    { 65284, 65284 },
    { 65504, 65505 },
    { 65509, 65510 },
};

static const URange16 Zl_range16[] = { // Line separator
    { 8232, 8232 },
};

static const URange16 Zp_range16[] = { // Paragraph separator
    { 8233, 8233 },
};

static const URange16 Zs_range16[] = { // Space separator
    { 32, 32 },
    { 160, 160 },
    { 5760, 5760 },
    { 8192, 8202 },
    { 8239, 8239 },
    { 8287, 8287 },
    { 12288, 12288 },
};

#define UNI_ENTRY(Code) \
    { #Code, Code##_range16, sizeof(Code##_range16)/(2*2) }

static const UGroup unicode_groups[] = {
    [U8G_Cc] = UNI_ENTRY(Cc),
    [U8G_Lt] = UNI_ENTRY(Lt),
    [U8G_Nd] = UNI_ENTRY(Nd),
    [U8G_Nl] = UNI_ENTRY(Nl),
    [U8G_Pc] = UNI_ENTRY(Pc),
    [U8G_Pd] = UNI_ENTRY(Pd),
    [U8G_Pf] = UNI_ENTRY(Pf),
    [U8G_Pi] = UNI_ENTRY(Pi),
    [U8G_Sc] = UNI_ENTRY(Sc),
    [U8G_Zl] = UNI_ENTRY(Zl),
    [U8G_Zp] = UNI_ENTRY(Zp),
    [U8G_Zs] = UNI_ENTRY(Zs),
};

static const int num_unicode_groups = sizeof unicode_groups / sizeof unicode_groups[0];

#endif
