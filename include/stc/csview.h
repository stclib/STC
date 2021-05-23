/* MIT License
 *
 * Copyright (c) 2021 Tyge Løvset, NORCE, www.norceresearch.no
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

#include "cstr.h"

typedef                 struct { const char* str; size_t size; } csview;
typedef                 struct { const char *ref; } csview_iter_t;
typedef                 char csview_value_t;
#define                 csview_null  c_make(csview){"", 0}
#define                 csview_ARG(sv)  (int)(sv).size, (sv).str

#define                 c_lit(literal) csview_lit(literal)
#define                 c_sv(s) csview_from_s(s)

#define                 csview_lit(literal) \
                            c_make(csview){literal, sizeof c_make(strlit_t){literal} - 1}
STC_INLINE csview       csview_from(const char* str)
                            { return c_make(csview){str, strlen(str)}; }
STC_INLINE csview       csview_from_n(const char* str, size_t n)
                            { return c_make(csview){str, n}; }
STC_INLINE csview       csview_from_s(cstr s)
                            { return c_make(csview){s.str, _cstr_rep(&s)->size}; }
STC_INLINE csview       csview_substr(csview sv, size_t pos, size_t n)
                            { sv.str += pos, sv.size = n; return sv; }
STC_INLINE csview       csview_trimmed(csview sv, size_t left, size_t right)
                            { sv.str += left, sv.size -= left + right; return sv; }

STC_INLINE size_t       csview_size(csview sv) { return sv.size; }
STC_INLINE size_t       csview_length(csview sv) { return sv.size; }
STC_INLINE bool         csview_empty(csview sv) { return sv.size == 0; }
STC_INLINE void         csview_clear(csview* self) { *self = csview_null; }
STC_INLINE const char*  csview_front(const csview* self) { return self->str; }
STC_INLINE const char*  csview_back(const csview* self) { return self->str + self->size - 1; }

#define                 csview_hash(sv)  c_default_hash((sv).str, (sv).size)
STC_INLINE bool         csview_equals(csview sv, csview sv2)
                            { return sv.size == sv2.size && !memcmp(sv.str, sv2.str, sv.size); }
STC_INLINE size_t       csview_find(csview sv, csview needle)
                            { char* res = c_strnstrn(sv.str, needle.str, sv.size, needle.size);
                              return res ? res - sv.str : cstr_npos; }
STC_INLINE bool         csview_contains(csview sv, csview needle)
                            { return c_strnstrn(sv.str, needle.str, sv.size, needle.size) != NULL; }
STC_INLINE bool         csview_begins_with(csview sv, csview sub)
                            { if (sub.size > sv.size) return false;
                              return !memcmp(sv.str, sub.str, sub.size); }
STC_INLINE bool         csview_ends_with(csview sv, csview sub)
                            { if (sub.size > sv.size) return false;
                              return !memcmp(sv.str + sv.size - sub.size, sub.str, sub.size); }
STC_INLINE csview_iter_t csview_begin(const csview* self)
                            { return c_make(csview_iter_t){self->str}; }
STC_INLINE csview_iter_t csview_end(const csview* self)
                            { return c_make(csview_iter_t){self->str + self->size}; }
STC_INLINE void          csview_next(csview_iter_t* it) { ++it->ref; }


STC_INLINE csview csview_first_token(csview sv, csview sep) {
    const char* res = c_strnstrn(sv.str, sep.str, sv.size, sep.size); 
    return c_make(csview){sv.str, (res ? res - sv.str : sv.size)};
}

STC_INLINE csview csview_next_token(csview sv, csview sep, csview token) {
    if (token.str - sv.str + token.size == sv.size)
        return c_make(csview){sv.str + sv.size, 0};
    token.str += token.size + sep.size;
    size_t n = sv.size - (token.str - sv.str);
    const char* res = c_strnstrn(token.str, sep.str, n, sep.size);
    token.size = res ? res - token.str : n;
    return token;
}

/* cstr interaction with csview: */

STC_INLINE cstr         cstr_from_v(csview sv)
                            { return cstr_from_n(sv.str, sv.size); }
STC_INLINE csview       cstr_to_v(const cstr* self)
                            { return c_make(csview){self->str, _cstr_rep(self)->size}; }
STC_INLINE csview       cstr_trimmed(cstr s, size_t left, size_t right)
                            { return csview_trimmed(c_sv(s), left, right); }
STC_INLINE cstr*        cstr_assign_v(cstr* self, csview sv)
                            { return cstr_assign_n(self, sv.str, sv.size); }
STC_INLINE cstr*        cstr_append_v(cstr* self, csview sv)
                            { return cstr_append_n(self, sv.str, sv.size); }
STC_INLINE void         cstr_insert_v(cstr* self, size_t pos, csview sv)
                            { cstr_replace_n(self, pos, 0, sv.str, sv.size); }
STC_INLINE void         cstr_replace_v(cstr* self, size_t pos, size_t len, csview sv)
                            { cstr_replace_n(self, pos, len, sv.str, sv.size); }
STC_INLINE bool         cstr_equals_v(cstr s, csview sv)
                            { return sv.size == cstr_size(s) && !memcmp(s.str, sv.str, sv.size); }
STC_INLINE size_t       cstr_find_v(cstr s, csview needle)
                            { char* res = c_strnstrn(s.str, needle.str, cstr_size(s), needle.size);
                              return res ? res - s.str : cstr_npos; }
STC_INLINE bool         cstr_contains_v(cstr s, csview needle)
                            { return c_strnstrn(s.str, needle.str, cstr_size(s), needle.size) != NULL; }
STC_INLINE bool         cstr_begins_with_v(cstr s, csview sub)
                            { if (sub.size > cstr_size(s)) return false;
                              return !memcmp(s.str, sub.str, sub.size); }
STC_INLINE bool         cstr_ends_with_v(cstr s, csview sub)
                            { if (sub.size > cstr_size(s)) return false;
                              return !memcmp(s.str + cstr_size(s) - sub.size, sub.str, sub.size); }

/* ---- Adaptor functions ---- */

#define                 csview_compare_ref(xp, yp)  strcmp((xp)->str, (yp)->str)
STC_INLINE bool         csview_equals_ref(const csview* a, const csview* b)
                            { return a->size == b->size && !memcmp(a->str, b->str, a->size); }
#define                 csview_hash_ref(xp, none)  c_default_hash((xp)->str, (xp)->size)

/* ---- cstr-containers with csview emplace/lookup API ---- */

#define using_cvec_sv() \
    using_cvec(sv, cstr, csview_compare_ref, cstr_del, cstr_from_v, cstr_to_v, csview)
#define using_cdeq_sv() \
    using_cdeq(sv, cstr, csview_compare_ref, cstr_del, cstr_from_v, cstr_to_v, csview)
#define using_clist_sv() \
    using_clist(sv, cstr, csview_compare_ref, cstr_del, cstr_from_v, cstr_to_v, csview)


#define using_csmap_svkey(...) c_MACRO_OVERLOAD(using_csmap_svkey, __VA_ARGS__)

#define using_csmap_svkey_2(X, Mapped) \
            using_csmap_svkey_4(X, Mapped, c_default_del, c_default_fromraw)
#define using_csmap_svkey_3(X, Mapped, mappedDel) \
            using_csmap_svkey_4(X, Mapped, mappedDel, c_no_clone)
#define using_csmap_svkey_4(X, Mapped, mappedDel, mappedClone) \
            using_csmap_svkey_6(X, Mapped, mappedDel, mappedClone, c_default_toraw, Mapped)
#define using_csmap_svkey_6(X, Mapped, mappedDel, mappedFromRaw, mappedToRaw, RawMapped) \
            _c_using_aatree(csmap_##X, csmap_, cstr, Mapped, csview_compare_ref, \
                            mappedDel, mappedFromRaw, mappedToRaw, RawMapped, \
                            cstr_del, cstr_from_v, cstr_to_v, csview)
#define using_csmap_sv() \
            using_csmap_svkey_6(sv, cstr, cstr_del, cstr_from_v, cstr_to_v, csview)
#define using_csset_sv() \
            _c_using_aatree(csset_sv, csset_, cstr, cstr, csview_compare_ref, \
                            @@, @@, @@, void, cstr_del, cstr_from_v, cstr_to_v, csview)


#define using_cmap_svkey(...) c_MACRO_OVERLOAD(using_cmap_svkey, __VA_ARGS__)

#define using_cmap_svkey_2(X, Mapped) \
            using_cmap_svkey_4(X, Mapped, c_default_del, c_default_fromraw)
#define using_cmap_svkey_3(X, Mapped, mappedDel) \
            using_cmap_svkey_4(X, Mapped, mappedDel, c_no_clone)
#define using_cmap_svkey_4(X, Mapped, mappedDel, mappedClone) \
            using_cmap_svkey_6(X, Mapped, mappedDel, mappedClone, c_default_toraw, Mapped)
#define using_cmap_svkey_6(X, Mapped, mappedDel, mappedFromRaw, mappedToRaw, RawMapped) \
            _c_using_chash(cmap_##X, cmap_, cstr, Mapped, csview_equals_ref, csview_hash_ref, \
                            mappedDel, mappedFromRaw, mappedToRaw, RawMapped, \
                            cstr_del, cstr_from_v, cstr_to_v, csview)
#define using_cmap_sv() \
            using_cmap_svkey_6(sv, cstr, cstr_del, cstr_from_v, cstr_to_v, csview)
#define using_cset_sv() \
            _c_using_chash(cset_sv, cset_, cstr, cstr, csview_equals_ref, csview_hash_ref, \
                           @@, @@, @@, void, cstr_del, cstr_from_v, cstr_to_v, csview)

#endif