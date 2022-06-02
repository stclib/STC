/* MIT License
 *
 * Copyright (c) 2022 Tyge Løvset, NORCE, www.norceresearch.no
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
#ifndef CSVIEW_H_INCLUDED
#define CSVIEW_H_INCLUDED

#include "ccommon.h"
#include "forward.h"
#include "utf8.h"

#define             csview_null  c_sv("")
#define             csview_new(literal) c_sv(literal)
#define             csview_npos  (SIZE_MAX >> 1)

STC_API csview      csview_substr_ex(csview sv, intptr_t pos, size_t n);
STC_API csview      csview_slice_ex(csview sv, intptr_t p1, intptr_t p2);
STC_API csview      csview_token(csview sv, csview sep, size_t* start);

STC_INLINE csview   csview_init() { return csview_null; }
STC_INLINE csview   csview_from(const char* str)
                        { return c_make(csview){str, strlen(str)}; }
STC_INLINE csview   csview_from_n(const char* str, size_t n)
                        { return c_make(csview){str, n}; }
STC_INLINE void     csview_clear(csview* self) { *self = csview_null; }

STC_INLINE size_t   csview_size(csview sv) { return sv.size; }
STC_INLINE size_t   csview_length(csview sv) { return sv.size; }
STC_INLINE bool     csview_empty(csview sv) { return sv.size == 0; }

STC_INLINE bool csview_equals(csview sv, csview sv2)
    { return sv.size == sv2.size && !memcmp(sv.str, sv2.str, sv.size); }

STC_INLINE size_t csview_find(csview sv, csview needle) {
    char* res = c_strnstrn(sv.str, needle.str, sv.size, needle.size);
    return res ? res - sv.str : csview_npos;
}

STC_INLINE bool csview_contains(csview sv, csview needle)
    { return c_strnstrn(sv.str, needle.str, sv.size, needle.size) != NULL; }

STC_INLINE bool csview_starts_with(csview sv, csview sub) {
    if (sub.size > sv.size) return false;
    return !memcmp(sv.str, sub.str, sub.size);
}

STC_INLINE bool csview_ends_with(csview sv, csview sub) {
    if (sub.size > sv.size) return false;
    return !memcmp(sv.str + sv.size - sub.size, sub.str, sub.size);
}

STC_INLINE csview csview_substr(csview sv, size_t pos, size_t n) {
    if (pos + n > sv.size) n = sv.size - pos;
    sv.str += pos, sv.size = n;
    return sv;
}

STC_INLINE csview csview_slice(csview sv, size_t p1, size_t p2) {
    if (p2 > sv.size) p2 = sv.size;
    sv.str += p1, sv.size = p2 > p1 ? p2 - p1 : 0;
    return sv;
}

/* iterator */
STC_INLINE csview_iter csview_begin(const csview* self)
    { return c_make(csview_iter){.chr = {self->str, utf8_codep_size(self->str)}}; }

STC_INLINE csview_iter csview_end(const csview* self)
    { return c_make(csview_iter){self->str + self->size}; }

STC_INLINE void csview_next(csview_iter* it)
    { it->ref += it->chr.size; it->chr.size = utf8_codep_size(it->ref); }

/* utf8 */
STC_INLINE size_t csview_size_u8(csview sv)
    { return utf8_size_n(sv.str, sv.size); }

STC_INLINE csview csview_substr_u8(csview sv, size_t u8pos, size_t u8len) {
    sv.str = utf8_at(sv.str, u8pos);
    sv.size = utf8_pos(sv.str, u8len);
    return sv;
}

STC_INLINE bool csview_valid_u8(csview sv) // depends on src/utf8code.c
    { return utf8_valid_n(sv.str, sv.size); }


/* csview interaction with cstr: */
#ifdef CSTR_H_INCLUDED

STC_INLINE csview csview_from_s(const cstr* self)
    { return c_make(csview){cstr_str(self), cstr_size(*self)}; }

STC_INLINE cstr cstr_from_sv(csview sv)
    { return cstr_from_n(sv.str, sv.size); }

STC_INLINE csview cstr_substr(const cstr* self, size_t pos, size_t n)
    { return csview_substr(csview_from_s(self), pos, n); }

STC_INLINE csview cstr_slice(const cstr* self, size_t p1, size_t p2)
    { return csview_slice(csview_from_s(self), p1, p2); }

STC_INLINE csview cstr_substr_ex(const cstr* self, intptr_t pos, size_t n)
    { return csview_substr_ex(csview_from_s(self), pos, n); }

STC_INLINE csview cstr_slice_ex(const cstr* self, intptr_t p1, intptr_t p2)
    { return csview_slice_ex(csview_from_s(self), p1, p2); }

STC_INLINE csview cstr_assign_sv(cstr* self, csview sv)
    { return c_make(csview){cstr_assign_n(self, sv.str, sv.size), sv.size}; }

STC_INLINE void cstr_append_sv(cstr* self, csview sv)
    { cstr_append_n(self, sv.str, sv.size); }

STC_INLINE void cstr_insert_sv(cstr* self, size_t pos, csview sv)
    { cstr_replace_n(self, pos, 0, sv.str, sv.size); }

STC_INLINE void cstr_replace_sv(cstr* self, csview sub, csview with)
    { cstr_replace_n(self, sub.str - cstr_str(self), sub.size, with.str, with.size); }

STC_INLINE bool cstr_equals_sv(cstr s, csview sv)
    { return sv.size == cstr_size(s) && !memcmp(cstr_str(&s), sv.str, sv.size); }

STC_INLINE size_t cstr_find_sv(cstr s, csview needle) { 
    char* res = c_strnstrn(cstr_str(&s), needle.str, cstr_size(s), needle.size);
    return res ? res - cstr_str(&s) : cstr_npos;
}

STC_INLINE bool cstr_contains_sv(cstr s, csview needle)
    { return c_strnstrn(cstr_str(&s), needle.str, cstr_size(s), needle.size) != NULL; }

STC_INLINE bool cstr_starts_with_sv(cstr s, csview sub) {
    if (sub.size > cstr_size(s)) return false;
    return !memcmp(cstr_str(&s), sub.str, sub.size);
}

STC_INLINE bool cstr_ends_with_sv(cstr s, csview sub) {
    if (sub.size > cstr_size(s)) return false;
    return !memcmp(cstr_str(&s) + cstr_size(s) - sub.size, sub.str, sub.size);
}
#endif
/* ---- Container helper functions ---- */

STC_INLINE int csview_cmp(const csview* x, const csview* y) 
    { return strcmp(x->str, y->str); }

STC_INLINE int csview_icmp(const csview* x, const csview* y)
    { return utf8_icmp_n(~(size_t)0, x->str, x->size, y->str, y->size); }

STC_INLINE bool csview_eq(const csview* x, const csview* y)
    { return x->size == y->size && !memcmp(x->str, y->str, x->size); }

STC_INLINE uint64_t csview_hash(const csview *self)
    { return c_fasthash(self->str, self->size); }

/* -------------------------- IMPLEMENTATION ------------------------- */
#if defined(i_implement) || defined(i_extern)

STC_DEF csview
csview_substr_ex(csview sv, intptr_t pos, size_t n) {
    if (pos < 0) { 
        pos += sv.size;
        if (pos < 0) pos = 0;
    }
    if (pos > (intptr_t)sv.size) pos = sv.size; 
    if (pos + n > sv.size) n = sv.size - pos;
    sv.str += pos, sv.size = n;
    return sv;
}

STC_DEF csview
csview_slice_ex(csview sv, intptr_t p1, intptr_t p2) {
    if (p1 < 0) { 
        p1 += sv.size;
        if (p1 < 0) p1 = 0;
    }
    if (p2 < 0) p2 += sv.size;
    if (p2 > (intptr_t)sv.size) p2 = sv.size;
    sv.str += p1, sv.size = p2 > p1 ? p2 - p1 : 0;
    return sv;
}

STC_DEF csview
csview_token(csview sv, csview sep, size_t* start) {
    csview slice = {sv.str + *start, sv.size - *start};
    const char* res = c_strnstrn(slice.str, sep.str, slice.size, sep.size);
    csview tok = {slice.str, res ? res - slice.str : (sep.size = 0, slice.size)};
    *start += tok.size + sep.size;
    return tok;
}

#endif
#endif
#undef i_opt
#undef i_header
#undef i_implement
#undef i_static
#undef i_extern
