/* MIT License
 *
 * Copyright (c) 2024 Tyge Løvset
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
#ifndef STC_UTF8_PRV_C_INCLUDED
#define STC_UTF8_PRV_C_INCLUDED

#include "utf8_tab.c"

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

int utf8_encode(char *out, uint32_t c)
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

bool utf8_valid_n(const char* s, intptr_t nbytes) {
    utf8_decode_t d = {.state=0};
    while ((nbytes-- != 0) & (*s != 0))
        utf8_decode(&d, (uint8_t)*s++);
    return d.state == 0;
}

uint32_t utf8_casefold(uint32_t c) {
    for (int i=0; i < casefold_len; ++i) {
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
    for (int i=0; i < (int)(sizeof upcase_ind/sizeof *upcase_ind); ++i) {
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
    for (int i=0; i < (int)(sizeof lowcase_ind/sizeof *lowcase_ind); ++i) {
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
    intptr_t j1 = 0, j2 = 0;
    while ((j1 < s1.size) & (j2 < s2.size)) {
        do { utf8_decode(&d1, (uint8_t)s1.buf[j1++]); } while (d1.state);
        do { utf8_decode(&d2, (uint8_t)s2.buf[j2++]); } while (d2.state);
        int32_t c = (int32_t)utf8_casefold(d1.codep) - (int32_t)utf8_casefold(d2.codep);
        if (c || !s2.buf[j2 - 1]) // OK if s1.size and s2.size are npos
            return (int)c;
    }
    return (int)(s1.size - s2.size);
}

typedef struct {
  uint16_t lo;
  uint16_t hi;
} URange16;

typedef struct {
  const URange16 *r16;
  int nr16;
} UGroup;

#ifndef __cplusplus
static
#else
extern
#endif
const UGroup _utf8_unicode_groups[U8G_SIZE];

bool utf8_isgroup(int group, uint32_t c) {
    for (int j=0; j<_utf8_unicode_groups[group].nr16; ++j) {
        if (c < _utf8_unicode_groups[group].r16[j].lo)
            return false;
        if (c <= _utf8_unicode_groups[group].r16[j].hi)
            return true;
    }
    return false;
}

bool utf8_isalpha(uint32_t c) {
    static int16_t groups[] = {U8G_Latin, U8G_Nl, U8G_Greek, U8G_Cyrillic,
                               U8G_Han, U8G_Devanagari, U8G_Arabic};
    if (c < 128) return isalpha((int)c) != 0;
    for (int j=0; j < c_arraylen(groups); ++j)
        if (utf8_isgroup(groups[j], c))
            return true;
    return false;
}

bool utf8_iscased(uint32_t c) {
    if (c < 128) return isalpha((int)c) != 0;
    return utf8_islower(c) || utf8_isupper(c) ||
           utf8_isgroup(U8G_Lt, c);
}

bool utf8_isword(uint32_t c) {
    if (c < 128) return (isalnum((int)c) != 0) | (c == '_');
    return utf8_isalpha(c) || utf8_isgroup(U8G_Nd, c) ||
           utf8_isgroup(U8G_Pc, c);
}

/* The tables below are extracted from the RE2 library */

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

static const URange16 Arabic_range16[] = {
    { 1536, 1540 },
    { 1542, 1547 },
    { 1549, 1562 },
    { 1564, 1566 },
    { 1568, 1599 },
    { 1601, 1610 },
    { 1622, 1647 },
    { 1649, 1756 },
    { 1758, 1791 },
    { 1872, 1919 },
    { 2160, 2190 },
    { 2192, 2193 },
    { 2200, 2273 },
    { 2275, 2303 },
    { 64336, 64450 },
    { 64467, 64829 },
    { 64832, 64911 },
    { 64914, 64967 },
    { 64975, 64975 },
    { 65008, 65023 },
    { 65136, 65140 },
    { 65142, 65276 },
};

static const URange16 Cyrillic_range16[] = {
    { 1024, 1156 },
    { 1159, 1327 },
    { 7296, 7304 },
    { 7467, 7467 },
    { 7544, 7544 },
    { 11744, 11775 },
    { 42560, 42655 },
    { 65070, 65071 },
};

static const URange16 Devanagari_range16[] = {
    { 2304, 2384 },
    { 2389, 2403 },
    { 2406, 2431 },
    { 43232, 43263 },
};

static const URange16 Greek_range16[] = {
    { 880, 883 },
    { 885, 887 },
    { 890, 893 },
    { 895, 895 },
    { 900, 900 },
    { 902, 902 },
    { 904, 906 },
    { 908, 908 },
    { 910, 929 },
    { 931, 993 },
    { 1008, 1023 },
    { 7462, 7466 },
    { 7517, 7521 },
    { 7526, 7530 },
    { 7615, 7615 },
    { 7936, 7957 },
    { 7960, 7965 },
    { 7968, 8005 },
    { 8008, 8013 },
    { 8016, 8023 },
    { 8025, 8025 },
    { 8027, 8027 },
    { 8029, 8029 },
    { 8031, 8061 },
    { 8064, 8116 },
    { 8118, 8132 },
    { 8134, 8147 },
    { 8150, 8155 },
    { 8157, 8175 },
    { 8178, 8180 },
    { 8182, 8190 },
    { 8486, 8486 },
    { 43877, 43877 },
};

static const URange16 Han_range16[] = {
    { 11904, 11929 },
    { 11931, 12019 },
    { 12032, 12245 },
    { 12293, 12293 },
    { 12295, 12295 },
    { 12321, 12329 },
    { 12344, 12347 },
    { 13312, 19903 },
    { 19968, 40959 },
    { 63744, 64109 },
    { 64112, 64217 },
};

static const URange16 Latin_range16[] = {
    { 65, 90 },
    { 97, 122 },
    { 170, 170 },
    { 186, 186 },
    { 192, 214 },
    { 216, 246 },
    { 248, 696 },
    { 736, 740 },
    { 7424, 7461 },
    { 7468, 7516 },
    { 7522, 7525 },
    { 7531, 7543 },
    { 7545, 7614 },
    { 7680, 7935 },
    { 8305, 8305 },
    { 8319, 8319 },
    { 8336, 8348 },
    { 8490, 8491 },
    { 8498, 8498 },
    { 8526, 8526 },
    { 8544, 8584 },
    { 11360, 11391 },
    { 42786, 42887 },
    { 42891, 42954 },
    { 42960, 42961 },
    { 42963, 42963 },
    { 42965, 42969 },
    { 42994, 43007 },
    { 43824, 43866 },
    { 43868, 43876 },
    { 43878, 43881 },
    { 64256, 64262 },
    { 65313, 65338 },
    { 65345, 65370 },
};

#define UNI_ENTRY(Code) \
    { Code##_range16, sizeof(Code##_range16)/sizeof(URange16) }
#ifdef __cplusplus
    #define _e_arg(k, v) v
#else
    #define _e_arg(k, v) [k] = v
    static
#endif
const UGroup _utf8_unicode_groups[U8G_SIZE] = {
    _e_arg(U8G_Cc, UNI_ENTRY(Cc)),
    _e_arg(U8G_Lt, UNI_ENTRY(Lt)),
    _e_arg(U8G_Nd, UNI_ENTRY(Nd)),
    _e_arg(U8G_Nl, UNI_ENTRY(Nl)),
    _e_arg(U8G_Pc, UNI_ENTRY(Pc)),
    _e_arg(U8G_Pd, UNI_ENTRY(Pd)),
    _e_arg(U8G_Pf, UNI_ENTRY(Pf)),
    _e_arg(U8G_Pi, UNI_ENTRY(Pi)),
    _e_arg(U8G_Sc, UNI_ENTRY(Sc)),
    _e_arg(U8G_Zl, UNI_ENTRY(Zl)),
    _e_arg(U8G_Zp, UNI_ENTRY(Zp)),
    _e_arg(U8G_Zs, UNI_ENTRY(Zs)),
    _e_arg(U8G_Arabic, UNI_ENTRY(Arabic)),
    _e_arg(U8G_Cyrillic, UNI_ENTRY(Cyrillic)),
    _e_arg(U8G_Devanagari, UNI_ENTRY(Devanagari)),
    _e_arg(U8G_Greek, UNI_ENTRY(Greek)),
    _e_arg(U8G_Han, UNI_ENTRY(Han)),
    _e_arg(U8G_Latin, UNI_ENTRY(Latin)),
};

#endif // STC_UTF8_PRV_C_INCLUDED
