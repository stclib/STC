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

#ifndef CRAWSTR_H_INCLUDED
#define CRAWSTR_H_INCLUDED

#include "ccommon.h"
#include "forward.h"
#include "priv/utf8_hdr.h"

#define             crawstr_init() c_rs("")
#define             crawstr_clone(rs) c_default_clone(rs)
#define             crawstr_drop(self) c_default_drop(self)
#define             crawstr_toraw(self) (self)->str

STC_INLINE crawstr  crawstr_from(const char* str)
                        { return c_rs_2(str, c_strlen(str)); }
STC_INLINE void     crawstr_clear(crawstr* self) { *self = c_rs(""); }
STC_INLINE csview   crawstr_sv(crawstr rs) { return c_sv_2(rs.str, rs.size); }

STC_INLINE intptr_t crawstr_size(crawstr rs) { return rs.size; }
STC_INLINE bool     crawstr_empty(crawstr rs) { return rs.size == 0; }

STC_INLINE bool crawstr_equals(crawstr rs, const char* str) {
    intptr_t n = c_strlen(str);
    return rs.size == n && !c_memcmp(rs.str, str, n);
}

STC_INLINE intptr_t crawstr_find(crawstr rs, const char* search) {
    char* res = strstr(rs.str, search);
    return res ? (res - rs.str) : c_NPOS;
}

STC_INLINE bool crawstr_contains(crawstr rs, const char* str)
    { return crawstr_find(rs, str) != c_NPOS; }

STC_INLINE bool crawstr_starts_with(crawstr rs, const char* str) {
    intptr_t n = c_strlen(str);
    return n > rs.size ? false : !c_memcmp(rs.str, str, n);
}

STC_INLINE bool crawstr_ends_with(crawstr rs, const char* str) {
    intptr_t n = c_strlen(str);
    return n > rs.size ? false : !c_memcmp(rs.str + rs.size - n, str, n);
}

/* utf8 iterator */
STC_INLINE crawstr_iter crawstr_begin(const crawstr* self) {
    if (!self->size) return c_LITERAL(crawstr_iter){.ref = NULL};
    return c_LITERAL(crawstr_iter){.chr = {self->str, utf8_chr_size(self->str)}};
}
STC_INLINE crawstr_iter crawstr_end(const crawstr* self) {
    (void)self; return c_LITERAL(crawstr_iter){.ref = NULL};
}
STC_INLINE void crawstr_next(crawstr_iter* it) {
    it->ref += it->chr.size;
    it->chr.size = utf8_chr_size(it->ref);
    if (!*it->ref) it->ref = NULL;
}
STC_INLINE crawstr_iter crawstr_advance(crawstr_iter it, intptr_t pos) {
    int inc = -1;
    if (pos > 0) pos = -pos, inc = 1;
    while (pos && *it.ref) pos += (*(it.ref += inc) & 0xC0) != 0x80;
    it.chr.size = utf8_chr_size(it.ref);
    if (!*it.ref) it.ref = NULL;
    return it;
}

/* utf8 ignore case cmp: depends on src/utf8code.c */
STC_INLINE int crawstr_icmp(const crawstr* x, const crawstr* y)
    { return utf8_icmp_sv(c_sv_2(x->str, x->size), c_sv_2(y->str, y->size)); }

STC_INLINE int crawstr_cmp(const crawstr* x, const crawstr* y) {
    intptr_t n = x->size < y->size ? x->size : y->size;
    int c = c_memcmp(x->str, y->str, n);
    return c ? c : (int)(x->size - y->size);
}

STC_INLINE bool crawstr_eq(const crawstr* x, const crawstr* y)
    { return x->size == y->size && !c_memcmp(x->str, y->str, x->size); }

STC_INLINE uint64_t crawstr_hash(const crawstr *self)
    { return stc_hash(self->str, self->size); }

#endif // CRAWSTR_H_INCLUDED

#if defined i_import
  #include "../../src/utf8code.c"
#endif
#include "priv/linkage2.h"
