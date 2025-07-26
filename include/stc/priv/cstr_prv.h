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
// IWYU pragma: private, include "stc/cstr.h"
#ifndef STC_CSTR_PRV_H_INCLUDED
#define STC_CSTR_PRV_H_INCLUDED

#include <stdio.h> /* FILE*, vsnprintf */
#include <stdlib.h> /* malloc */
#include <stddef.h> /* size_t */
#include <stdarg.h> /* cstr_vfmt() */
/**************************** PRIVATE API **********************************/

#if defined __GNUC__ && !defined __clang__
  // linkage.h already does diagnostic push
  // Warns wrongfully on -O3 on cstr_assign(&str, "literal longer than 23 ...");
  #pragma GCC diagnostic ignored "-Warray-bounds"
#endif

enum  { cstr_s_cap = sizeof(cstr_buf) - 2 };
#define cstr_s_size(s)          ((isize)(s)->sml.size)
#define cstr_s_set_size(s, len) ((s)->sml.data[(s)->sml.size = (uint8_t)(len)] = 0)
#define cstr_s_data(s)          (s)->sml.data

#if defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    #define byte_rotl_(x, b)       ((x) << (b)*8 | (x) >> (sizeof(x) - (b))*8)
    #define cstr_l_cap(s)          (isize)(~byte_rotl_((s)->lon.ncap, sizeof((s)->lon.ncap) - 1))
    #define cstr_l_set_cap(s, cap) ((s)->lon.ncap = ~byte_rotl_((uintptr_t)(cap), 1))
#else
    #define cstr_l_cap(s)          (isize)(~(s)->lon.ncap)
    #define cstr_l_set_cap(s, cap) ((s)->lon.ncap = ~(uintptr_t)(cap))
#endif
#define cstr_l_size(s)          (isize)((s)->lon.size)
#define cstr_l_set_size(s, len) ((s)->lon.data[(s)->lon.size = (uintptr_t)(len)] = 0)
#define cstr_l_data(s)          (s)->lon.data
#define cstr_l_drop(s)          c_free((s)->lon.data, cstr_l_cap(s) + 1)

#define cstr_is_long(s)         ((s)->sml.size >= 128)
extern  char* _cstr_init(cstr* self, isize len, isize cap);
extern  char* _cstr_internal_move(cstr* self, isize pos1, isize pos2);

/**************************** PUBLIC API **********************************/

#define             cstr_init() (c_literal(cstr){0})
#define             cstr_lit(literal) cstr_from_n(literal, c_litstrlen(literal))

extern  cstr        cstr_from_replace(csview sv, csview search, csview repl, int32_t count);
extern  cstr        cstr_from_fmt(const char* fmt, ...) c_GNUATTR(format(printf, 1, 2));

extern  void        cstr_drop(const cstr* self);
extern  cstr*       cstr_take(cstr* self, const cstr s);
extern  char*       cstr_reserve(cstr* self, isize cap);
extern  void        cstr_shrink_to_fit(cstr* self);
extern  char*       cstr_resize(cstr* self, isize size, char value);
extern  isize       cstr_find_at(const cstr* self, isize pos, const char* search);
extern  isize       cstr_find_sv(const cstr* self, csview search);
extern  char*       cstr_assign_n(cstr* self, const char* str, isize len);
extern  char*       cstr_append_n(cstr* self, const char* str, isize len);
extern  isize       cstr_append_fmt(cstr* self, const char* fmt, ...) c_GNUATTR(format(printf, 2, 3));
extern  char*       cstr_append_uninit(cstr *self, isize len);

extern  bool        cstr_getdelim(cstr *self, int delim, FILE *fp);
extern  void        cstr_erase(cstr* self, isize pos, isize len);
extern  isize       cstr_printf(cstr* self, const char* fmt, ...) c_GNUATTR(format(printf, 2, 3));
extern  isize       cstr_vfmt(cstr* self, isize start, const char* fmt, va_list args);
extern  size_t      cstr_hash(const cstr *self);
extern  bool        cstr_u8_valid(const cstr* self);
extern  void        cstr_u8_erase(cstr* self, isize u8pos, isize u8len);

STC_INLINE cstr_buf cstr_getbuf(cstr* s) {
    return cstr_is_long(s) ? c_literal(cstr_buf){s->lon.data, cstr_l_size(s), cstr_l_cap(s)}
                           : c_literal(cstr_buf){s->sml.data, cstr_s_size(s), cstr_s_cap};
}
STC_INLINE zsview cstr_zv(const cstr* s) {
    return cstr_is_long(s) ? c_literal(zsview){s->lon.data, cstr_l_size(s)}
                           : c_literal(zsview){s->sml.data, cstr_s_size(s)};
}
STC_INLINE csview cstr_sv(const cstr* s) {
    return cstr_is_long(s) ? c_literal(csview){s->lon.data, cstr_l_size(s)}
                           : c_literal(csview){s->sml.data, cstr_s_size(s)};
}

STC_INLINE cstr cstr_from_n(const char* str, const isize len) {
    cstr s;
    c_memcpy(_cstr_init(&s, len, len), str, len);
    return s;
}

STC_INLINE cstr cstr_from(const char* str)
    { return cstr_from_n(str, c_strlen(str)); }

STC_INLINE cstr cstr_from_sv(csview sv)
    { return cstr_from_n(sv.buf, sv.size); }

STC_INLINE cstr cstr_from_zv(zsview zv)
    { return cstr_from_n(zv.str, zv.size); }

STC_INLINE cstr cstr_with_size(const isize size, const char value) {
    cstr s;
    c_memset(_cstr_init(&s, size, size), value, size);
    return s;
}

STC_INLINE cstr cstr_with_capacity(const isize cap) {
    cstr s;
    _cstr_init(&s, 0, cap);
    return s;
}

STC_INLINE cstr cstr_move(cstr* self) {
    cstr tmp = *self;
    *self = cstr_init();
    return tmp;
}

STC_INLINE cstr cstr_clone(cstr s) {
    csview sv = cstr_sv(&s);
    return cstr_from_n(sv.buf, sv.size);
}

#define SSO_CALL(s, call) (cstr_is_long(s) ? cstr_l_##call : cstr_s_##call)

STC_INLINE void _cstr_set_size(cstr* self, isize len)
    { SSO_CALL(self, set_size(self, len)); }

STC_INLINE void cstr_clear(cstr* self)
    { _cstr_set_size(self, 0); }

STC_INLINE char* cstr_data(cstr* self)
    { return SSO_CALL(self, data(self)); }

STC_INLINE const char* cstr_str(const cstr* self)
    { return SSO_CALL(self, data(self)); }

STC_INLINE const char* cstr_toraw(const cstr* self)
    { return SSO_CALL(self, data(self)); }

STC_INLINE isize cstr_size(const cstr* self)
    { return SSO_CALL(self, size(self)); }

STC_INLINE bool cstr_is_empty(const cstr* self)
    { return cstr_size(self) == 0; }

STC_INLINE isize cstr_capacity(const cstr* self)
    { return cstr_is_long(self) ? cstr_l_cap(self) : cstr_s_cap; }

STC_INLINE isize cstr_to_index(const cstr* self, cstr_iter it)
    { return it.ref - cstr_str(self); }

STC_INLINE cstr cstr_from_s(cstr s, isize pos, isize len)
    { return cstr_from_n(cstr_str(&s) + pos, len); }

STC_INLINE csview cstr_subview(const cstr* self, isize pos, isize len) {
    csview sv = cstr_sv(self);
    c_assert(((size_t)pos <= (size_t)sv.size) & (len >= 0));
    if (pos + len > sv.size) len = sv.size - pos;
    return c_literal(csview){sv.buf + pos, len};
}

STC_INLINE zsview cstr_tail(const cstr* self, isize len) {
    c_assert(len >= 0);
    csview sv = cstr_sv(self);
    if (len > sv.size) len = sv.size;
    return c_literal(zsview){&sv.buf[sv.size - len], len};
}

// BEGIN utf8 functions =====

STC_INLINE cstr cstr_u8_from(const char* str, isize u8pos, isize u8len)
    { str = utf8_at(str, u8pos); return cstr_from_n(str, utf8_to_index(str, u8len)); }

STC_INLINE isize cstr_u8_size(const cstr* self)
    { return utf8_count(cstr_str(self)); }

STC_INLINE isize cstr_u8_to_index(const cstr* self, isize u8pos)
    { return utf8_to_index(cstr_str(self), u8pos); }

STC_INLINE zsview cstr_u8_tail(const cstr* self, isize u8len) {
    csview sv = cstr_sv(self);
    const char* p = &sv.buf[sv.size];
    while (u8len && p != sv.buf)
        u8len -= (*--p & 0xC0) != 0x80;
    return c_literal(zsview){p, sv.size - (p - sv.buf)};
}

STC_INLINE csview cstr_u8_subview(const cstr* self, isize u8pos, isize u8len)
    { return utf8_subview(cstr_str(self), u8pos, u8len); }

STC_INLINE cstr_iter cstr_u8_at(const cstr* self, isize u8pos) {
    csview sv;
    sv.buf = utf8_at(cstr_str(self), u8pos);
    sv.size = utf8_chr_size(sv.buf);
    c_assert(sv.size);
    return c_literal(cstr_iter){.chr = sv};
}

// utf8 iterator

STC_INLINE cstr_iter cstr_begin(const cstr* self) {
    csview sv = cstr_sv(self);
    cstr_iter it = {.chr = {sv.buf, utf8_chr_size(sv.buf)}};
    return it;
}
STC_INLINE cstr_iter cstr_end(const cstr* self) {
    (void)self; cstr_iter it = {0}; return it;
}
STC_INLINE void cstr_next(cstr_iter* it) {
    it->ref += it->chr.size;
    it->chr.size = utf8_chr_size(it->ref);
    if (*it->ref == '\0') it->ref = NULL;
}

STC_INLINE cstr_iter cstr_advance(cstr_iter it, isize u8pos) {
    it.ref = utf8_offset(it.ref, u8pos);
    it.chr.size = utf8_chr_size(it.ref);
    if (*it.ref == '\0') it.ref = NULL;
    return it;
}

// utf8 case conversion: requires `#define i_import` before including cstr.h in one TU.
extern  cstr cstr_tocase_sv(csview sv, int k);

STC_INLINE cstr cstr_casefold_sv(csview sv)
    { return cstr_tocase_sv(sv, 0); }

STC_INLINE cstr cstr_tolower_sv(csview sv)
    { return cstr_tocase_sv(sv, 1); }

STC_INLINE cstr cstr_toupper_sv(csview sv)
    { return cstr_tocase_sv(sv, 2); }

STC_INLINE cstr cstr_tolower(const char* str)
    { return cstr_tolower_sv(c_sv(str, c_strlen(str))); }

STC_INLINE cstr cstr_toupper(const char* str)
    { return cstr_toupper_sv(c_sv(str, c_strlen(str))); }

STC_INLINE void cstr_lowercase(cstr* self)
    { cstr_take(self, cstr_tolower_sv(cstr_sv(self))); }

STC_INLINE void cstr_uppercase(cstr* self)
    { cstr_take(self, cstr_toupper_sv(cstr_sv(self))); }

STC_INLINE bool cstr_istarts_with(const cstr* self, const char* sub) {
    csview sv = cstr_sv(self);
    isize len = c_strlen(sub);
    return len <= sv.size && !utf8_icompare((sv.size = len, sv), c_sv(sub, len));
}

STC_INLINE bool cstr_iends_with(const cstr* self, const char* sub) {
    csview sv = cstr_sv(self);
    isize len = c_strlen(sub);
    return len <= sv.size && !utf8_icmp(sv.buf + sv.size - len, sub);
}

STC_INLINE int cstr_icmp(const cstr* s1, const cstr* s2)
    { return utf8_icmp(cstr_str(s1), cstr_str(s2)); }

STC_INLINE bool cstr_ieq(const cstr* s1, const cstr* s2) {
    csview x = cstr_sv(s1), y = cstr_sv(s2);
    return x.size == y.size && !utf8_icompare(x, y);
}

STC_INLINE bool cstr_iequals(const cstr* self, const char* str)
    { return !utf8_icmp(cstr_str(self), str); }

// END utf8 =====

STC_INLINE int cstr_cmp(const cstr* s1, const cstr* s2)
    { return strcmp(cstr_str(s1), cstr_str(s2)); }

STC_INLINE bool cstr_eq(const cstr* s1, const cstr* s2) {
    csview x = cstr_sv(s1), y = cstr_sv(s2);
    return x.size == y.size && !c_memcmp(x.buf, y.buf, x.size);
}

STC_INLINE bool cstr_equals(const cstr* self, const char* str)
    { return !strcmp(cstr_str(self), str); }

STC_INLINE bool cstr_equals_sv(const cstr* self, csview sv)
    { return sv.size == cstr_size(self) && !c_memcmp(cstr_str(self), sv.buf, sv.size); }

STC_INLINE isize cstr_find(const cstr* self, const char* search) {
    const char *str = cstr_str(self), *res = strstr((char*)str, search);
    return res ? (res - str) : c_NPOS;
}

STC_INLINE bool cstr_contains(const cstr* self, const char* search)
    { return strstr((char*)cstr_str(self), search) != NULL; }

STC_INLINE bool cstr_contains_sv(const cstr* self, csview search)
    { return cstr_find_sv(self, search) != c_NPOS; }


STC_INLINE bool cstr_starts_with_sv(const cstr* self, csview sub) {
    if (sub.size > cstr_size(self)) return false;
    return !c_memcmp(cstr_str(self), sub.buf, sub.size);
}

STC_INLINE bool cstr_starts_with(const cstr* self, const char* sub) {
    const char* str = cstr_str(self);
    while (*sub && *str == *sub) ++str, ++sub;
    return !*sub;
}

STC_INLINE bool cstr_ends_with_sv(const cstr* self, csview sub) {
    csview sv = cstr_sv(self);
    if (sub.size > sv.size) return false;
    return !c_memcmp(sv.buf + sv.size - sub.size, sub.buf, sub.size);
}

STC_INLINE bool cstr_ends_with(const cstr* self, const char* sub)
    { return cstr_ends_with_sv(self, c_sv(sub, c_strlen(sub))); }

STC_INLINE char* cstr_assign(cstr* self, const char* str)
    { return cstr_assign_n(self, str, c_strlen(str)); }

STC_INLINE char* cstr_assign_sv(cstr* self, csview sv)
    { return cstr_assign_n(self, sv.buf, sv.size); }

STC_INLINE char* cstr_copy(cstr* self, cstr s) {
    csview sv = cstr_sv(&s);
    return cstr_assign_n(self, sv.buf, sv.size);
}


STC_INLINE char* cstr_push(cstr* self, const char* chr)
    { return cstr_append_n(self, chr, utf8_chr_size(chr)); }

STC_INLINE void cstr_pop(cstr* self) {
    csview sv = cstr_sv(self);
    const char* s = sv.buf + sv.size;
    while ((*--s & 0xC0) == 0x80) ;
    _cstr_set_size(self, (s - sv.buf));
}

STC_INLINE char* cstr_append(cstr* self, const char* str)
    { return cstr_append_n(self, str, c_strlen(str)); }

STC_INLINE char* cstr_append_sv(cstr* self, csview sv)
    { return cstr_append_n(self, sv.buf, sv.size); }

STC_INLINE char* cstr_append_s(cstr* self, cstr s)
    { return cstr_append_sv(self, cstr_sv(&s)); }

#define cstr_join(self, sep, vec) do { \
    struct _vec_s { cstr* data; ptrdiff_t size; } \
          *_vec = (struct _vec_s*)&(vec); \
    (void)sizeof((vec).data == _vec->data && &(vec).size == &_vec->size); \
    cstr_join_sn(self, sep, _vec->data, _vec->size); \
} while (0);

#define cstr_join_items(self, sep, ...) \
    cstr_join_n(self, sep, c_make_array(const char*, __VA_ARGS__), c_sizeof((const char*[])__VA_ARGS__)/c_sizeof(char*))

STC_INLINE void cstr_join_n(cstr* self, const char* sep, const char* arr[], isize n) {
    const char* _sep = cstr_is_empty(self) ? "" : sep;
    while (n--) { cstr_append(self, _sep); cstr_append(self, *arr++); _sep = sep; }
}
STC_INLINE void cstr_join_sn(cstr* self, const char* sep, const cstr arr[], isize n) {
    const char* _sep = cstr_is_empty(self) ? "" : sep;
    while (n--) { cstr_append(self, _sep); cstr_append_s(self, *arr++); _sep = sep; }
}


STC_INLINE void cstr_replace_sv(cstr* self, csview search, csview repl, int32_t count)
    { cstr_take(self, cstr_from_replace(cstr_sv(self), search, repl, count)); }

STC_INLINE void cstr_replace_nfirst(cstr* self, const char* search, const char* repl, int32_t count)
    { cstr_replace_sv(self, c_sv(search, c_strlen(search)), c_sv(repl, c_strlen(repl)), count); }

STC_INLINE void cstr_replace(cstr* self, const char* search, const char* repl)
    { cstr_replace_nfirst(self, search, repl, INT32_MAX); }


STC_INLINE void cstr_replace_at_sv(cstr* self, isize pos, isize len, const csview repl) {
    char* d = _cstr_internal_move(self, pos + len, pos + repl.size);
    c_memcpy(d + pos, repl.buf, repl.size);
}
STC_INLINE void cstr_replace_at(cstr* self, isize pos, isize len, const char* repl)
    { cstr_replace_at_sv(self, pos, len, c_sv(repl, c_strlen(repl))); }

STC_INLINE void cstr_u8_replace(cstr* self, isize u8pos, isize u8len, const char* repl) {
    const char* s = cstr_str(self); csview span = utf8_subview(s, u8pos, u8len);
    cstr_replace_at(self, span.buf - s, span.size, repl);
}


STC_INLINE void cstr_insert_sv(cstr* self, isize pos, csview sv)
    { cstr_replace_at_sv(self, pos, 0, sv); }

STC_INLINE void cstr_insert(cstr* self, isize pos, const char* str)
    { cstr_replace_at_sv(self, pos, 0, c_sv(str, c_strlen(str))); }

STC_INLINE void cstr_u8_insert(cstr* self, isize u8pos, const char* str)
    { cstr_insert(self, utf8_to_index(cstr_str(self), u8pos), str); }

STC_INLINE bool cstr_getline(cstr *self, FILE *fp)
    { return cstr_getdelim(self, '\n', fp); }

#endif // STC_CSTR_PRV_H_INCLUDED
