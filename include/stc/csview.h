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
#define _i_inc_utf8
#include "utf8.h"

#ifndef CSVIEW_H_INCLUDED
#define CSVIEW_H_INCLUDED

#define             csview_init() c_sv_1("")
#define             csview_drop(p) c_default_drop(p)
#define             csview_clone(sv) c_default_clone(sv)
#define             csview_from_n(str, n) c_sv_2(str, n)

STC_INLINE csview   csview_from(const char* str)
                        { return csview_from_n(str, c_strlen(str)); }
STC_INLINE void     csview_clear(csview* self) { *self = csview_init(); }
STC_INLINE csubstr  csview_ss(csview sv) { return c_ss_2(sv.str, sv.size); }

STC_INLINE intptr_t csview_size(csview sv) { return sv.size; }
STC_INLINE bool     csview_empty(csview sv) { return sv.size == 0; }

STC_INLINE bool csview_equals(csview sv, const char* str) { 
    intptr_t n = c_strlen(str); 
    return sv.size == n && !c_memcmp(sv.str, str, n);
}

STC_INLINE intptr_t csview_find_v(csview sv, csview search) {
    char* res = cstrnstrn(sv.str, search.str, sv.size, search.size);
    return res ? (res - sv.str) : c_NPOS;
}

STC_INLINE intptr_t csview_find(csview sv, const char* str)
    { return csview_find_v(sv, c_sv_2(str, c_strlen(str))); }

STC_INLINE bool csview_contains(csview sv, const char* str)
    { return csview_find(sv, str) != c_NPOS; }

STC_INLINE bool csview_starts_with(csview sv, const char* str) {
    intptr_t n = c_strlen(str);
    return n > sv.size ? false : !c_memcmp(sv.str, str, n);
}

STC_INLINE bool csview_ends_with(csview sv, const char* str) {
    intptr_t n = c_strlen(str);
    return n > sv.size ? false : !c_memcmp(sv.str + sv.size - n, str, n);
}

/* utf8 iterator */
STC_INLINE csview_iter csview_begin(const csview* self) { 
    if (!self->size) return c_LITERAL(csview_iter){.ref = NULL};
    return c_LITERAL(csview_iter){.u8 = {{self->str, utf8_chr_size(self->str)}}};
}
STC_INLINE csview_iter csview_end(const csview* self) {
    (void)self; return c_LITERAL(csview_iter){.ref = NULL};
}
STC_INLINE void csview_next(csview_iter* it) {
    it->ref += it->u8.chr.size;
    it->u8.chr.size = utf8_chr_size(it->ref);
    if (!*it->ref) it->ref = NULL;
}
STC_INLINE csview_iter csview_advance(csview_iter it, intptr_t pos) {
    int inc = -1;
    if (pos > 0) pos = -pos, inc = 1;
    while (pos && *it.ref) pos += (*(it.ref += inc) & 0xC0) != 0x80;
    it.u8.chr.size = utf8_chr_size(it.ref);
    if (!*it.ref) it.ref = NULL;
    return it;
}

/* utf8 size */
STC_INLINE intptr_t csview_u8_size(csview sv)
    { return utf8_size_n(sv.str, sv.size); }

/* utf8 validation: depends on src/utf8code.c */
STC_INLINE bool csview_valid_utf8(csview sv)
    { return utf8_valid_n(sv.str, sv.size); }

/* utf8 ignore case cmp: depends on src/utf8code.c */
STC_INLINE int csview_icmp(const csview* x, const csview* y)
    { return utf8_icmp_ss(c_ss_2(x->str, x->size), c_ss_2(y->str, y->size)); }


STC_INLINE int csview_cmp(const csview* x, const csview* y) {
    intptr_t n = x->size < y->size ? x->size : y->size;
    int c = c_memcmp(x->str, y->str, n);
    return c ? c : (int)(x->size - y->size);
}

STC_INLINE bool csview_eq(const csview* x, const csview* y)
    { return x->size == y->size && !c_memcmp(x->str, y->str, x->size); }

STC_INLINE uint64_t csview_hash(const csview *self)
    { return cfasthash(self->str, self->size); }

#endif // CSVIEW_H_INCLUDED
#undef i_static
#undef i_header
#undef i_implement
#undef i_import
#undef i_opt
