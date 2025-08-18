/* MIT License
 *
 * Copyright (c) 2025 Tyge Løvset
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

int utf8_encode(char *out, uint32_t c) {
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

uint32_t utf8_peek_at(const char* s, isize offset) {
    return utf8_peek(utf8_offset(s, offset));
}

bool utf8_valid(const char* s) {
    utf8_decode_t d = {.state=0};
    while ((utf8_decode(&d, (uint8_t)*s) != utf8_REJECT) & (*s != '\0'))
        ++s;
    return d.state == utf8_ACCEPT;
}

bool utf8_valid_n(const char* s, isize nbytes) {
    utf8_decode_t d = {.state=0};
    for (; nbytes-- != 0; ++s)
        if ((utf8_decode(&d, (uint8_t)*s) == utf8_REJECT) | (*s == '\0'))
            break;
    return d.state == utf8_ACCEPT;
}

uint32_t utf8_casefold(uint32_t c) {
    #define _at_fold(idx) &casemappings[idx].c2
    int i;
    c_lowerbound(uint32_t, c, _at_fold, c_default_less, casefold_len, &i);
    if (i < casefold_len && casemappings[i].c1 <= c) {
        const struct CaseMapping entry = casemappings[i];
        int d = entry.m2 - entry.c2;
        if (d == 1) return c + ((entry.c2 & 1U) == (c & 1U));
        return (uint32_t)((int)c + d);
    }
    return c;
}

uint32_t utf8_tolower(uint32_t c) {
    #define _at_upper(idx) &casemappings[upcase_ind[idx]].c2
    int i, n = c_countof(upcase_ind);
    c_lowerbound(uint32_t, c, _at_upper, c_default_less, n, &i);
    if (i < n) {
        const struct CaseMapping entry = casemappings[upcase_ind[i]];
        if (entry.c1 <= c) {
            int d = entry.m2 - entry.c2;
            if (d == 1) return c + ((entry.c2 & 1U) == (c & 1U));
            return (uint32_t)((int)c + d);
        }
    }
    return c;
}

uint32_t utf8_toupper(uint32_t c) {
    #define _at_lower(idx) &casemappings[lowcase_ind[idx]].m2
    int i, n = c_countof(lowcase_ind);
    c_lowerbound(uint32_t, c, _at_lower, c_default_less, n, &i);
    if (i < n) {
        const struct CaseMapping entry = casemappings[lowcase_ind[i]];
        int d = entry.m2 - entry.c2;
        if (entry.c1 + (uint32_t)d <= c) {
            if (d == 1) return c - ((entry.m2 & 1U) == (c & 1U));
            return (uint32_t)((int)c - d);
        }
    }
    return c;
}

int utf8_decode_codepoint(utf8_decode_t* d, const char* s, const char* end) { // s < end
    const char* start = s;
    do switch (utf8_decode(d, (uint8_t)*s++)) {
        case utf8_ACCEPT: return (int)(s - start);
        case utf8_REJECT: goto recover;
    } while (s != end);

    recover: // non-complete utf8 is also treated as utf8_REJECT
    d->state = utf8_ACCEPT;
    d->codep = 0xFFFD;
    //return 1;
    int n = (int)(s - start);
    return n > 2 ? n - 1 : 1;
}

int utf8_icompare(const csview s1, const csview s2) {
    utf8_decode_t d1 = {.state=0}, d2 = {.state=0};
    const char *e1 = s1.buf + s1.size, *e2 = s2.buf + s2.size;
    isize j1 = 0, j2 = 0;
    while ((j1 < s1.size) & (j2 < s2.size)) {
        if (s2.buf[j2] == '\0') return s1.buf[j1];

        j1 += utf8_decode_codepoint(&d1, s1.buf + j1, e1);
        j2 += utf8_decode_codepoint(&d2, s2.buf + j2, e2);

        int32_t c = (int32_t)utf8_casefold(d1.codep) - (int32_t)utf8_casefold(d2.codep);
        if (c != 0) return (int)c;
    }
    return (int)(s1.size - s2.size);
}

#endif // STC_UTF8_PRV_C_INCLUDED
