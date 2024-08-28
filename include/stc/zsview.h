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
#define i_header // external linkage by default. override with i_static.
#include "priv/linkage.h"

// zsview is a zero-terminated string view.

#ifndef STC_ZSVIEW_H_INCLUDED
#define STC_ZSVIEW_H_INCLUDED

#include "common.h"
#include "types.h"
#include "priv/utf8_prv.h"

#define             zsview_init() c_zv("")
#define             zsview_clone(zs) c_default_clone(zs)
#define             zsview_drop(self) c_default_drop(self)
#define             zsview_toraw(self) (self)->str

STC_INLINE zsview   zsview_from(const char* str)
                        { return c_literal(zsview){str, c_strlen(str)}; }
STC_INLINE void     zsview_clear(zsview* self) { *self = c_zv(""); }
STC_INLINE csview   zsview_sv(zsview zs) { return c_sv_2(zs.str, zs.size); }

STC_INLINE isize zsview_size(zsview zs) { return zs.size; }
STC_INLINE bool     zsview_is_empty(zsview zs) { return zs.size == 0; }

STC_INLINE bool zsview_equals(zsview zs, const char* str) {
    isize n = c_strlen(str);
    return zs.size == n && !c_memcmp(zs.str, str, n);
}

STC_INLINE isize zsview_find(zsview zs, const char* search) {
    char* res = strstr(zs.str, search);
    return res ? (res - zs.str) : c_NPOS;
}

STC_INLINE bool zsview_contains(zsview zs, const char* str)
    { return zsview_find(zs, str) != c_NPOS; }

STC_INLINE bool zsview_starts_with(zsview zs, const char* str) {
    isize n = c_strlen(str);
    return n > zs.size ? false : !c_memcmp(zs.str, str, n);
}

STC_INLINE bool zsview_ends_with(zsview zs, const char* str) {
    isize n = c_strlen(str);
    return n > zs.size ? false : !c_memcmp(zs.str + zs.size - n, str, n);
}

STC_INLINE zsview zsview_from_pos(zsview zs, isize pos) {
    if (pos < zs.size) { zs.str += pos; zs.size -= pos; }
    return zs;
}

STC_INLINE zsview zsview_last(zsview zs, isize count)
    { return zsview_from_pos(zs, zs.size - count); }

/* utf8 */
STC_INLINE isize zsview_u8_size(zsview zs)
    { return utf8_size(zs.str); }

STC_INLINE const char* zsview_u8_at(zsview zs, isize u8idx)
    { return utf8_at(zs.str, u8idx); }

STC_INLINE zsview zsview_u8_from_pos(zsview zs, isize u8idx)
    { return zsview_from_pos(zs, utf8_pos(zs.str, u8idx)); }

STC_INLINE zsview zsview_u8_last(zsview zs, isize u8len) {
    const char* p = zs.str + zs.size;
    while (u8len && p != zs.str) u8len -= (*--p & 0xC0) != 0x80;
    return zsview_from_pos(zs, p - zs.str);
}

STC_INLINE bool zsview_u8_valid(zsview zs) // requires linking with utf8 symbols
    { return utf8_valid_n(zs.str, zs.size); }

/* utf8 iterator */
STC_INLINE zsview_iter zsview_begin(const zsview* self) {
    return c_literal(zsview_iter){.chr = {self->str, utf8_chr_size(self->str)}};
}

STC_INLINE zsview_iter zsview_end(const zsview* self) {
    (void)self; return c_literal(zsview_iter){.ref = NULL};
}

STC_INLINE void zsview_next(zsview_iter* it) {
    it->ref += it->chr.size;
    it->chr.size = utf8_chr_size(it->ref);
    if (!*it->ref) it->ref = NULL;
}

STC_INLINE zsview_iter zsview_advance(zsview_iter it, isize u8pos) {
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
STC_INLINE int zsview_icmp(const zsview* x, const zsview* y)
    { return utf8_icmp_sv(c_sv_2(x->str, x->size), c_sv_2(y->str, y->size)); }

STC_INLINE int zsview_cmp(const zsview* x, const zsview* y) {
    isize n = x->size < y->size ? x->size : y->size;
    int c = c_memcmp(x->str, y->str, n);
    return c ? c : (int)(x->size - y->size);
}

STC_INLINE bool zsview_eq(const zsview* x, const zsview* y)
    { return x->size == y->size && !c_memcmp(x->str, y->str, x->size); }

STC_INLINE uint64_t zsview_hash(const zsview *self)
    { return chash_n(self->str, self->size); }

#endif // STC_ZSVIEW_H_INCLUDED

#if defined i_import
  #include "priv/utf8_prv.c"
#endif
#include "priv/linkage2.h"
