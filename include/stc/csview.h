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

typedef                 struct csview { const char* str; size_t size; } csview;
typedef                 struct csview_iter { const char *ref; } csview_iter;
typedef                 char csview_value;

#define                 csview_null  c_make(csview){"", 0}
#define                 csview_npos  (SIZE_MAX >> 1)
#define                 c_PRIsv      "%.*s"
#define                 c_ARGsv(sv)  (int)(sv).size, (sv).str
#define                 c_sv(literal) csview_new(literal)

STC_API csview          csview_substr(csview sv, intptr_t pos, size_t n);
STC_API csview          csview_slice(csview sv, intptr_t p1, intptr_t p2);
STC_API csview          csview_token(csview sv, csview sep, size_t* start);

#define                 csview_new(literal) \
                            c_make(csview){literal, sizeof c_make(c_strlit){literal} - 1}
STC_INLINE csview       csview_init() { return csview_null; }
STC_INLINE csview       csview_from(const char* str)
                            { return c_make(csview){str, strlen(str)}; }
STC_INLINE csview       csview_from_n(const char* str, size_t n)
                            { return c_make(csview){str, n}; }
STC_INLINE size_t       csview_size(csview sv) { return sv.size; }
STC_INLINE size_t       csview_length(csview sv) { return sv.size; }
STC_INLINE bool         csview_empty(csview sv) { return sv.size == 0; }
STC_INLINE char         csview_front(csview sv) { return sv.str[0]; }
STC_INLINE char         csview_back(csview sv) { return sv.str[sv.size - 1]; }

STC_INLINE void         csview_clear(csview* self) { *self = csview_null; }

STC_INLINE bool         csview_equals(csview sv, csview sv2)
                            { return sv.size == sv2.size && !memcmp(sv.str, sv2.str, sv.size); }
STC_INLINE size_t       csview_find(csview sv, csview needle)
                            { char* res = c_strnstrn(sv.str, needle.str, sv.size, needle.size);
                              return res ? res - sv.str : csview_npos; }
STC_INLINE bool         csview_contains(csview sv, csview needle)
                            { return c_strnstrn(sv.str, needle.str, sv.size, needle.size) != NULL; }
STC_INLINE bool         csview_starts_with(csview sv, csview sub)
                            { if (sub.size > sv.size) return false;
                              return !memcmp(sv.str, sub.str, sub.size); }
STC_INLINE bool         csview_ends_with(csview sv, csview sub)
                            { if (sub.size > sv.size) return false;
                              return !memcmp(sv.str + sv.size - sub.size, sub.str, sub.size); }
STC_INLINE csview_iter  csview_begin(const csview* self)
                            { return c_make(csview_iter){self->str}; }
STC_INLINE csview_iter  csview_end(const csview* self)
                            { return c_make(csview_iter){self->str + self->size}; }
STC_INLINE void         csview_next(csview_iter* it) { ++it->ref; }

/* ---- Container helper functions ---- */

STC_INLINE int          csview_cmp(const csview* x, const csview* y) { 
                            const size_t m = x->size < y->size ? x->size : y->size; 
                            const int c = memcmp(x->str, y->str, m);
                            return c ? c : x->size - y->size;
                        }
#define                 csview_hash(xp, dummy) c_default_hash((xp)->str, (xp)->size)
#define                 csview_eq(xp, yp) (!csview_cmp(xp, yp))

/* -------------------------- IMPLEMENTATION ------------------------- */
#if defined(_i_implement)

STC_DEF csview
csview_substr(csview sv, intptr_t pos, size_t n) {
    if (pos < 0) { 
        pos += sv.size;
        if (pos < 0) pos = 0;
    }
    if (pos > sv.size) pos = sv.size; 
    if (pos + n > sv.size) n = sv.size - pos;
    sv.str += pos, sv.size = n;
    return sv;
}

STC_DEF csview
csview_slice(csview sv, intptr_t p1, intptr_t p2) {
    if (p1 < 0) { 
        p1 += sv.size;
        if (p1 < 0) p1 = 0;
    }
    if (p2 < 0) p2 += sv.size;
    if (p2 > sv.size) p2 = sv.size;
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
