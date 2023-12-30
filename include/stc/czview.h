/* MIT License
 *
 * Copyright (c) 2023 Tyge Løvset
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
#define i_header // external linkage by default. override with i_static.
#include "priv/linkage.h"

// czview is a "zero-terminated string view". It replaces czview.

#ifndef STC_CZVIEW_H_INCLUDED
#define STC_CZVIEW_H_INCLUDED

#include "common.h"
#include "types.h"
#include "priv/utf8_prv.h"

#define             czview_init() c_zv("")
#define             czview_clone(rs) c_default_clone(rs)
#define             czview_drop(self) c_default_drop(self)
#define             czview_toraw(self) (self)->str

STC_INLINE czview   czview_from(const char* str)
                        { return c_LITERAL(czview){str, c_strlen(str)}; }
STC_INLINE void     czview_clear(czview* self) { *self = c_zv(""); }
STC_INLINE csview   czview_sv(czview rs) { return c_sv_2(rs.str, rs.size); }

STC_INLINE intptr_t czview_size(czview rs) { return rs.size; }
STC_INLINE bool     czview_empty(czview rs) { return rs.size == 0; }

STC_INLINE bool czview_equals(czview rs, const char* str) {
    intptr_t n = c_strlen(str);
    return rs.size == n && !c_memcmp(rs.str, str, n);
}

STC_INLINE intptr_t czview_find(czview rs, const char* search) {
    char* res = strstr(rs.str, search);
    return res ? (res - rs.str) : c_NPOS;
}

STC_INLINE bool czview_contains(czview rs, const char* str)
    { return czview_find(rs, str) != c_NPOS; }

STC_INLINE bool czview_starts_with(czview rs, const char* str) {
    intptr_t n = c_strlen(str);
    return n > rs.size ? false : !c_memcmp(rs.str, str, n);
}

STC_INLINE bool czview_ends_with(czview rs, const char* str) {
    intptr_t n = c_strlen(str);
    return n > rs.size ? false : !c_memcmp(rs.str + rs.size - n, str, n);
}

STC_INLINE czview czview_from_pos(czview rs, intptr_t pos) {
    if (pos < rs.size) { rs.str += pos; rs.size -= pos; }
    return rs;
}

STC_INLINE czview czview_last(czview rs, intptr_t count)
    { return czview_from_pos(rs, rs.size - count); }

/* utf8 */
STC_INLINE intptr_t czview_u8_size(czview rs)
    { return utf8_size(rs.str); }

STC_INLINE const char* czview_u8_at(czview rs, intptr_t u8idx)
    { return utf8_at(rs.str, u8idx); }

STC_INLINE czview czview_u8_from_pos(czview rs, intptr_t u8idx)
    { return czview_from_pos(rs, utf8_pos(rs.str, u8idx)); }

STC_INLINE czview czview_u8_last(czview rs, intptr_t u8len) {
    const char* p = rs.str + rs.size;
    while (u8len && p != rs.str) u8len -= (*--p & 0xC0) != 0x80;
    return czview_from_pos(rs, p - rs.str);
}

STC_INLINE bool czview_u8_valid(czview rs) // requires linking with utf8 symbols
    { return utf8_valid_n(rs.str, rs.size); }

/* utf8 iterator */
STC_INLINE czview_iter czview_begin(const czview* self) {
    return c_LITERAL(czview_iter){.chr = {self->str, utf8_chr_size(self->str)}};
}

STC_INLINE czview_iter czview_end(const czview* self) {
    (void)self; return c_LITERAL(czview_iter){.ref = NULL};
}

STC_INLINE void czview_next(czview_iter* it) {
    it->ref += it->chr.size;
    it->chr.size = utf8_chr_size(it->ref);
    if (!*it->ref) it->ref = NULL;
}

STC_INLINE czview_iter czview_advance(czview_iter it, intptr_t u8pos) {
    int inc = -1;
    if (u8pos > 0)
        u8pos = -u8pos, inc = 1;
    while (u8pos && *it.ref)
        u8pos += (*(it.ref += inc) & 0xC0) != 0x80;
    it.chr.size = utf8_chr_size(it.ref);
    if (!*it.ref) it.ref = NULL;
    return it;
}

/* utf8 ignore case cmp: requires linking with utf8 symbols */
STC_INLINE int czview_icmp(const czview* x, const czview* y)
    { return utf8_icmp_sv(c_sv_2(x->str, x->size), c_sv_2(y->str, y->size)); }

STC_INLINE int czview_cmp(const czview* x, const czview* y) {
    intptr_t n = x->size < y->size ? x->size : y->size;
    int c = c_memcmp(x->str, y->str, n);
    return c ? c : (int)(x->size - y->size);
}

STC_INLINE bool czview_eq(const czview* x, const czview* y)
    { return x->size == y->size && !c_memcmp(x->str, y->str, x->size); }

STC_INLINE uint64_t czview_hash(const czview *self)
    { return c_hash_n(self->str, self->size); }

#endif // STC_CZVIEW_H_INCLUDED

#if defined i_import
  #include "priv/utf8_prv.c"
#endif
#include "priv/linkage2.h"
