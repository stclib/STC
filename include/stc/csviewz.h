/* MIT License
 *
 * Copyright (c) 2026 Tyge Løvset
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

// csviewz is a zero-terminated string view.

#ifndef STC_CSVIEWZ_H_INCLUDED
#define STC_CSVIEWZ_H_INCLUDED

#include "common.h"
#include "types.h"
#include "priv/utf8_prv.h"

#define             csviewz_init() c_zv("")
#define             csviewz_clone(zs) c_default_clone(zs)
#define             csviewz_drop(self) c_default_drop(self)
#define             csviewz_toraw(self) (self)->str

typedef             const char* csviewz_raw;
#define             csviewz_raw_cmp(x, y)      strcmp(*(x), *(y))
#define             csviewz_raw_eq(x, y)       (csviewz_raw_cmp(x, y) == 0)
#define             csviewz_raw_hash(vp)       c_hash_str(*(vp))

STC_INLINE csviewz  csviewz_from(const char* str)
                        { return c_literal(csviewz){str, c_strlen(str)}; }
STC_INLINE void     csviewz_clear(csviewz* self) { *self = c_zv(""); }
STC_INLINE csview   csviewz_sv(csviewz zs) { return c_sv_2(zs.str, zs.size); }

STC_INLINE isize_t  csviewz_size(csviewz zs) { return zs.size; }
STC_INLINE bool     csviewz_is_empty(csviewz zs) { return zs.size == 0; }

STC_INLINE bool csviewz_equals(csviewz zs, const char* str) {
    isize_t n = c_strlen(str);
    return zs.size == n && !c_memcmp(zs.str, str, n);
}

STC_INLINE isize_t csviewz_find(csviewz zs, const char* search) {
    const char* res = strstr(zs.str, search);
    return res ? (res - zs.str) : c_NPOS;
}

STC_INLINE bool csviewz_contains(csviewz zs, const char* str)
    { return csviewz_find(zs, str) != c_NPOS; }

STC_INLINE bool csviewz_starts_with(csviewz zs, const char* str) {
    isize_t n = c_strlen(str);
    return n <= zs.size && !c_memcmp(zs.str, str, n);
}

STC_INLINE bool csviewz_ends_with(csviewz zs, const char* str) {
    isize_t n = c_strlen(str);
    return n <= zs.size && !c_memcmp(zs.str + zs.size - n, str, n);
}

STC_INLINE csviewz csviewz_from_pos(csviewz zs, isize_t pos) {
    if (pos > zs.size) pos = zs.size;
    zs.str += pos; zs.size -= pos; return zs;
}

STC_INLINE csview csviewz_subview(csviewz zs, isize_t pos, isize_t len) {
    c_assert(((size_t)pos <= (size_t)zs.size) & (len >= 0));
    if (pos + len > zs.size) len = zs.size - pos;
    return c_literal(csview){zs.str + pos, len};
}

STC_INLINE csviewz csviewz_tail(csviewz zs, isize_t len) {
    if (len > zs.size) len = zs.size;
    zs.str += zs.size - len; zs.size = len;
    return zs;
}

/* utf8 */

STC_INLINE csviewz csviewz_u8_from_pos(csviewz zs, isize_t u8pos)
    { return csviewz_from_pos(zs, cutf8_to_index(zs.str, u8pos)); }

STC_INLINE csviewz csviewz_u8_tail(csviewz zs, isize_t u8len) {
    const char* p = &zs.str[zs.size];
    while (u8len && p != zs.str)
        u8len -= (*--p & 0xC0) != 0x80;
    zs.size -= p - zs.str, zs.str = p;
    return zs;
}

STC_INLINE csview csviewz_u8_subview(csviewz zs, isize_t u8pos, isize_t u8len)
    { return cutf8_subview(zs.str, u8pos, u8len); }

STC_INLINE csviewz_iter csviewz_u8_at(csviewz zs, isize_t u8pos) {
    csviewz_iter it = {.u8={{cutf8_at(zs.str, u8pos)}}};
    if (*it.ref == '\0') it.ref = NULL;
    else it.chr.size = cutf8_decode_codepoint(&it.u8.dec, it.ref, NULL);
    return it;
}

STC_INLINE isize_t csviewz_u8_size(csviewz zs)
    { return cutf8_count(zs.str); }

STC_INLINE bool csviewz_u8_valid(csviewz zs) // requires linking with utf8 symbols
    { return cutf8_valid_n(zs.str, zs.size); }

/* utf8 iterator */
STC_INLINE csviewz_iter csviewz_begin(const csviewz* self) {
    csviewz_iter it = {.u8={{self->str}}};
    if (*it.ref == '\0') it.ref = NULL;
    else it.chr.size = cutf8_decode_codepoint(&it.u8.dec, it.ref, NULL);
    return it;
}

STC_INLINE csviewz_iter csviewz_end(const csviewz* self) {
    (void)self; csviewz_iter it = {0}; return it;
}

STC_INLINE void csviewz_next(csviewz_iter* it) {
    it->ref += it->chr.size;
    if (*it->ref == '\0') it->ref = NULL;
    else it->chr.size = cutf8_decode_codepoint(&it->u8.dec, it->ref, NULL);
}

STC_INLINE csviewz_iter csviewz_advance(csviewz_iter it, isize_t u8pos) {
    it.ref = cutf8_offset(it.ref, u8pos);
    if (*it.ref == '\0') it.ref = NULL;
    else it.chr.size = cutf8_decode_codepoint(&it.u8.dec, it.ref, NULL);
    return it;
}

STC_INLINE uint32_t csviewz_codepoint(const csviewz_iter* it)
    { return it->u8.dec.codep; }

/* ---- Container helper functions ---- */

STC_INLINE size_t csviewz_hash(const csviewz *self)
    { return c_hash_str(self->str); }

STC_INLINE int csviewz_cmp(const csviewz* x, const csviewz* y)
    { return strcmp(x->str, y->str); }

STC_INLINE bool csviewz_eq(const csviewz* x, const csviewz* y)
    { return x->size == y->size && !c_memcmp(x->str, y->str, x->size); }

STC_INLINE int csviewz_icmp(const csviewz* x, const csviewz* y)
    { return cutf8_icmp(x->str, y->str); }

STC_INLINE bool csviewz_ieq(const csviewz* x, const csviewz* y)
    { return x->size == y->size && !cutf8_icmp(x->str, y->str); }

/* ---- case insensitive ---- */

STC_INLINE bool csviewz_iequals(csviewz zs, const char* str)
    { return c_strlen(str) == zs.size && !cutf8_icmp(zs.str, str); }

STC_INLINE bool csviewz_istarts_with(csviewz zs, const char* str)
    { return c_strlen(str) <= zs.size && !cutf8_icmp(zs.str, str); }

STC_INLINE bool csviewz_iends_with(csviewz zs, const char* str) {
    isize_t n = c_strlen(str);
    return n <= zs.size && !cutf8_icmp(zs.str + zs.size - n, str);
}

#endif // STC_CSVIEWZ_H_INCLUDED

#if defined i_import
#include "priv/utf8_prv.c"
#endif
