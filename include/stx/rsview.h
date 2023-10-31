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
#include "../stc/priv/linkage.h"

// rsview is a null-terminated "raw string view". It replaces crawstr.

#ifndef RSVIEW_H_INCLUDED
#define RSVIEW_H_INCLUDED

#include "../stc/ccommon.h"
#include "../stc/forward.h"
#include "../stc/priv/utf8_hdr.h"

#define             rsview_init() c_rs("")
#define             rsview_clone(rs) c_default_clone(rs)
#define             rsview_drop(self) c_default_drop(self)
#define             rsview_toraw(self) (self)->str

STC_INLINE rsview   rsview_from(const char* str)
                        { return c_rs_2(str, c_strlen(str)); }
STC_INLINE void     rsview_clear(rsview* self) { *self = c_rs(""); }
STC_INLINE csview   rsview_sv(rsview rs) { return c_sv_2(rs.str, rs.size); }

STC_INLINE intptr_t rsview_size(rsview rs) { return rs.size; }
STC_INLINE bool     rsview_empty(rsview rs) { return rs.size == 0; }

STC_INLINE bool rsview_equals(rsview rs, const char* str) { 
    intptr_t n = c_strlen(str); 
    return rs.size == n && !c_memcmp(rs.str, str, n);
}

STC_INLINE intptr_t rsview_find(rsview rs, const char* search) {
    char* res = strstr(rs.str, search);
    return res ? (res - rs.str) : c_NPOS;
}

STC_INLINE bool rsview_contains(rsview rs, const char* str)
    { return rsview_find(rs, str) != c_NPOS; }

STC_INLINE bool rsview_starts_with(rsview rs, const char* str) {
    intptr_t n = c_strlen(str);
    return n > rs.size ? false : !c_memcmp(rs.str, str, n);
}

STC_INLINE bool rsview_ends_with(rsview rs, const char* str) {
    intptr_t n = c_strlen(str);
    return n > rs.size ? false : !c_memcmp(rs.str + rs.size - n, str, n);
}

STC_INLINE rsview rsview_substr(rsview rs, intptr_t pos) {
    if (pos < rs.size) { rs.str += pos; rs.size -= pos; }
    return rs;
}

STC_INLINE rsview rsview_last(rsview rs, intptr_t count)
    { return rsview_substr(rs, rs.size - count); }

/* utf8 */
STC_INLINE intptr_t rsview_u8_size(rsview rs)
    { return utf8_size(rs.str); }

STC_INLINE const char* rsview_u8_at(rsview rs, intptr_t u8idx)
    { return utf8_at(rs.str, u8idx); }

STC_INLINE rsview rsview_u8_last(rsview rs, intptr_t u8len) {
    const char* p = rs.str + rs.size;
    while (u8len && p != rs.str) u8len -= (*--p & 0xC0) != 0x80;
    return rsview_substr(rs, p - rs.str);
}

STC_INLINE bool rsview_u8_valid(rsview rs) // depends on src/utf8code.c
    { return utf8_valid_n(rs.str, rs.size); }

/* utf8 iterator */
STC_INLINE rsview_iter rsview_begin(const rsview* self) { 
    if (!self->size) return c_LITERAL(rsview_iter){.ref = NULL};
    return c_LITERAL(rsview_iter){.chr = {self->str, utf8_chr_size(self->str)}};
}

STC_INLINE rsview_iter rsview_end(const rsview* self) {
    (void)self; return c_LITERAL(rsview_iter){.ref = NULL};
}

STC_INLINE void rsview_next(rsview_iter* it) {
    it->ref += it->chr.size;
    it->chr.size = utf8_chr_size(it->ref);
    if (!*it->ref) it->ref = NULL;
}

STC_INLINE rsview_iter rsview_advance(rsview_iter it, intptr_t pos) {
    int inc = -1;
    if (pos > 0) pos = -pos, inc = 1;
    while (pos && *it.ref) pos += (*(it.ref += inc) & 0xC0) != 0x80;
    it.chr.size = utf8_chr_size(it.ref);
    if (!*it.ref) it.ref = NULL;
    return it;
}

/* utf8 ignore case cmp: depends on src/utf8code.c */
STC_INLINE int rsview_icmp(const rsview* x, const rsview* y)
    { return utf8_icmp_sv(c_sv_2(x->str, x->size), c_sv_2(y->str, y->size)); }

STC_INLINE int rsview_cmp(const rsview* x, const rsview* y) {
    intptr_t n = x->size < y->size ? x->size : y->size;
    int c = c_memcmp(x->str, y->str, n);
    return c ? c : (int)(x->size - y->size);
}

STC_INLINE bool rsview_eq(const rsview* x, const rsview* y)
    { return x->size == y->size && !c_memcmp(x->str, y->str, x->size); }

STC_INLINE uint64_t rsview_hash(const rsview *self)
    { return stc_hash(self->str, self->size); }

#endif // RSVIEW_H_INCLUDED

#if defined i_import
  #include "../../src/utf8code.c"
#endif
#include "../stc/priv/linkage2.h"
