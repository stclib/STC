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
#ifndef STC_CSTR_PRV_H_INCLUDED
#define STC_CSTR_PRV_H_INCLUDED

#include <stdlib.h> /* malloc */
#include <stdarg.h>
#include <stdio.h> /* vsnprintf */

/**************************** PRIVATE API **********************************/

#if defined __GNUC__ && !defined __clang__
  // linkage.h already does diagnostic push
  // Warns wrongfully on -O3 on cstr_assign(&str, "literal longer than 23 ...");
  #pragma GCC diagnostic ignored "-Warray-bounds"
#endif

enum  { cstr_s_last = sizeof(cstr_buf) - 1,
        cstr_s_cap = cstr_s_last - 1 };
#define cstr_s_size(s)          ((intptr_t)(s)->sml.data[cstr_s_last])
#define cstr_s_set_size(s, len) ((s)->sml.data[len] = 0, (s)->sml.data[cstr_s_last] = (char)(len))
#define cstr_s_data(s)          (s)->sml.data

#if defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    #define byte_rotl_(x, b)       ((x) << (b)*8 | (x) >> (sizeof(x) - (b))*8)
    #define cstr_l_cap(s)          (intptr_t)(~byte_rotl_((s)->lon.ncap, sizeof((s)->lon.ncap) - 1))
    #define cstr_l_set_cap(s, cap) ((s)->lon.ncap = ~byte_rotl_((size_t)(cap), 1))
#else
    #define cstr_l_cap(s)          (intptr_t)(~(s)->lon.ncap)
    #define cstr_l_set_cap(s, cap) ((s)->lon.ncap = ~(size_t)(cap))
#endif
#define cstr_l_size(s)          (intptr_t)((s)->lon.size)
#define cstr_l_set_size(s, len) ((s)->lon.data[(s)->lon.size = (size_t)(len)] = 0)
#define cstr_l_data(s)          (s)->lon.data
#define cstr_l_drop(s)          i_free((s)->lon.data, cstr_l_cap(s) + 1)

#define cstr_is_long(s)         (((s)->sml.data[cstr_s_last] & 128) != 0)
STC_API char* _cstr_init(cstr* self, intptr_t len, intptr_t cap);
STC_API char* _cstr_internal_move(cstr* self, intptr_t pos1, intptr_t pos2);

/**************************** PUBLIC API **********************************/

#define cstr_lit(literal) cstr_from_n(literal, c_litstrlen(literal))
#define cstr_null (c_LITERAL(cstr){0})
#define cstr_toraw(self) cstr_str(self)

STC_API char*       cstr_reserve(cstr* self, intptr_t cap);
STC_API void        cstr_shrink_to_fit(cstr* self);
STC_API char*       cstr_resize(cstr* self, intptr_t size, char value);
STC_API intptr_t    cstr_find_at(const cstr* self, intptr_t pos, const char* search);
STC_API intptr_t    cstr_find_sv(const cstr* self, csview search);
STC_API char*       cstr_assign_n(cstr* self, const char* str, intptr_t len);
STC_API char*       cstr_append_n(cstr* self, const char* str, intptr_t len);
STC_API char*       cstr_append_uninit(cstr *self, intptr_t len);
STC_API bool        cstr_getdelim(cstr *self, int delim, FILE *fp);
STC_API void        cstr_erase(cstr* self, intptr_t pos, intptr_t len);
STC_API void        cstr_u8_erase(cstr* self, intptr_t bytepos, intptr_t u8len);
STC_API cstr        cstr_from_fmt(const char* fmt, ...);
STC_API intptr_t    cstr_append_fmt(cstr* self, const char* fmt, ...);
STC_API intptr_t    cstr_printf(cstr* self, const char* fmt, ...);
STC_API cstr        cstr_replace_sv(csview sv, csview search, csview repl, int32_t count);
STC_API uint64_t    cstr_hash(const cstr *self);

STC_INLINE cstr_buf cstr_buffer(cstr* s) {
    return cstr_is_long(s) ? c_LITERAL(cstr_buf){s->lon.data, cstr_l_size(s), cstr_l_cap(s)}
                           : c_LITERAL(cstr_buf){s->sml.data, cstr_s_size(s), cstr_s_cap};
}
STC_INLINE czview cstr_zv(const cstr* s) {
    return cstr_is_long(s) ? c_LITERAL(czview){s->lon.data, cstr_l_size(s)}
                           : c_LITERAL(czview){s->sml.data, cstr_s_size(s)};
}
STC_INLINE csview cstr_sv(const cstr* s) {
    return cstr_is_long(s) ? c_LITERAL(csview){s->lon.data, cstr_l_size(s)}
                           : c_LITERAL(csview){s->sml.data, cstr_s_size(s)};
}

STC_INLINE cstr cstr_init(void)
    { return cstr_null; }

STC_INLINE cstr cstr_from_n(const char* str, const intptr_t len) {
    cstr s;
    c_memcpy(_cstr_init(&s, len, len), str, len);
    return s;
}

STC_INLINE cstr cstr_from(const char* str)
    { return cstr_from_n(str, c_strlen(str)); }

STC_INLINE cstr cstr_from_sv(csview sv)
    { return cstr_from_n(sv.buf, sv.size); }

STC_INLINE cstr cstr_from_zv(czview zv)
    { return cstr_from_n(zv.str, zv.size); }

STC_INLINE cstr cstr_with_size(const intptr_t size, const char value) {
    cstr s;
    c_memset(_cstr_init(&s, size, size), value, size);
    return s;
}

STC_INLINE cstr cstr_with_capacity(const intptr_t cap) {
    cstr s;
    _cstr_init(&s, 0, cap);
    return s;
}

STC_INLINE cstr* cstr_take(cstr* self, const cstr s) {
    if (cstr_is_long(self) && self->lon.data != s.lon.data)
        cstr_l_drop(self);
    *self = s;
    return self;
}

STC_INLINE cstr cstr_move(cstr* self) {
    cstr tmp = *self;
    *self = cstr_null;
    return tmp;
}

STC_INLINE cstr cstr_clone(cstr s) {
    csview sv = cstr_sv(&s);
    return cstr_from_n(sv.buf, sv.size);
}

STC_INLINE void cstr_drop(cstr* self) {
    if (cstr_is_long(self))
        cstr_l_drop(self);
}

#define SSO_CALL(s, call) (cstr_is_long(s) ? cstr_l_##call : cstr_s_##call)

STC_INLINE void _cstr_set_size(cstr* self, intptr_t len)
    { SSO_CALL(self, set_size(self, len)); }

STC_INLINE void cstr_clear(cstr* self)
    { _cstr_set_size(self, 0); }

STC_INLINE char* cstr_data(cstr* self)
    { return SSO_CALL(self, data(self)); }

STC_INLINE const char* cstr_str(const cstr* self)
    { return SSO_CALL(self, data(self)); }

STC_INLINE bool cstr_empty(const cstr* self)
    { return cstr_s_size(self) == 0; }

STC_INLINE intptr_t cstr_size(const cstr* self)
    { return SSO_CALL(self, size(self)); }

STC_INLINE intptr_t cstr_capacity(const cstr* self)
    { return cstr_is_long(self) ? cstr_l_cap(self) : (intptr_t)cstr_s_cap; }

STC_INLINE intptr_t cstr_topos(const cstr* self, cstr_iter it)
    { return it.ref - cstr_str(self); }


// requires linking with utf8 symbols, e.g. #define i_import before including cstr.h in one TU.

extern cstr cstr_casefold_sv(csview sv);
extern cstr cstr_tolower_sv(csview sv);
extern cstr cstr_toupper_sv(csview sv);
extern cstr cstr_tolower(const char* str);
extern cstr cstr_toupper(const char* str);
extern void cstr_lowercase(cstr* self);
extern void cstr_uppercase(cstr* self);
extern bool cstr_valid_utf8(const cstr* self);

STC_INLINE cstr cstr_from_pos(cstr s, intptr_t pos, intptr_t len)
    { return cstr_from_n(cstr_str(&s) + pos, len); }

STC_INLINE intptr_t cstr_u8_size(const cstr* self)
    { return utf8_size(cstr_str(self)); }

STC_INLINE intptr_t cstr_u8_size_n(const cstr* self, intptr_t nbytes)
    { return utf8_size_n(cstr_str(self), nbytes); }

STC_INLINE intptr_t cstr_u8_topos(const cstr* self, intptr_t u8idx)
    { return utf8_pos(cstr_str(self), u8idx); }

STC_INLINE const char* cstr_u8_at(const cstr* self, intptr_t u8idx)
    { return utf8_at(cstr_str(self), u8idx); }

STC_INLINE csview cstr_u8_chr(const cstr* self, intptr_t u8idx) {
    const char* str = cstr_str(self);
    csview sv;
    sv.buf = utf8_at(str, u8idx);
    sv.size = utf8_chr_size(sv.buf);
    return sv;
}

// utf8 iterator

STC_INLINE cstr_iter cstr_begin(const cstr* self) {
    csview sv = cstr_sv(self);
    if (!sv.size) return c_LITERAL(cstr_iter){.ref = NULL};
    return c_LITERAL(cstr_iter){.chr = {sv.buf, utf8_chr_size(sv.buf)}};
}
STC_INLINE cstr_iter cstr_end(const cstr* self) {
    (void)self; return c_LITERAL(cstr_iter){NULL};
}
STC_INLINE void cstr_next(cstr_iter* it) {
    it->ref += it->chr.size;
    it->chr.size = utf8_chr_size(it->ref);
    if (!*it->ref) it->ref = NULL;
}
STC_INLINE cstr_iter cstr_advance(cstr_iter it, intptr_t u8pos) {
    int inc = -1;
    if (u8pos > 0)
        u8pos = -u8pos, inc = 1;
    while (u8pos && *it.ref)
        u8pos += (*(it.ref += inc) & 0xC0) != 0x80;
    it.chr.size = utf8_chr_size(it.ref);
    if (!*it.ref) it.ref = NULL;
    return it;
}


STC_INLINE int cstr_cmp(const cstr* s1, const cstr* s2)
    { return strcmp(cstr_str(s1), cstr_str(s2)); }

STC_INLINE int cstr_icmp(const cstr* s1, const cstr* s2)
    { return utf8_icmp(cstr_str(s1), cstr_str(s2)); }

STC_INLINE bool cstr_eq(const cstr* s1, const cstr* s2) {
    csview x = cstr_sv(s1), y = cstr_sv(s2);
    return x.size == y.size && !c_memcmp(x.buf, y.buf, x.size);
}


STC_INLINE bool cstr_equals(const cstr* self, const char* str)
    { return !strcmp(cstr_str(self), str); }

STC_INLINE bool cstr_equals_sv(const cstr* self, csview sv)
    { return sv.size == cstr_size(self) && !c_memcmp(cstr_str(self), sv.buf, sv.size); }

STC_INLINE bool cstr_equals_s(const cstr* self, cstr s)
    { return !cstr_cmp(self, &s); }

STC_INLINE bool cstr_iequals(const cstr* self, const char* str)
    { return !utf8_icmp(cstr_str(self), str); }


STC_INLINE intptr_t cstr_find(const cstr* self, const char* search) {
    const char *str = cstr_str(self), *res = strstr((char*)str, search);
    return res ? (res - str) : c_NPOS;
}

STC_INLINE intptr_t cstr_find_s(const cstr* self, cstr search)
    { return cstr_find(self, cstr_str(&search)); }


STC_INLINE bool cstr_contains(const cstr* self, const char* search)
    { return strstr((char*)cstr_str(self), search) != NULL; }

STC_INLINE bool cstr_contains_sv(const cstr* self, csview search)
    { return cstr_find_sv(self, search) != c_NPOS; }

STC_INLINE bool cstr_contains_s(const cstr* self, cstr search)
    { return strstr((char*)cstr_str(self), cstr_str(&search)) != NULL; }


STC_INLINE bool cstr_starts_with_sv(const cstr* self, csview sub) {
    if (sub.size > cstr_size(self)) return false;
    return !c_memcmp(cstr_str(self), sub.buf, sub.size);
}

STC_INLINE bool cstr_starts_with(const cstr* self, const char* sub) {
    const char* str = cstr_str(self);
    while (*sub && *str == *sub) ++str, ++sub;
    return !*sub;
}

STC_INLINE bool cstr_starts_with_s(const cstr* self, cstr sub)
    { return cstr_starts_with_sv(self, cstr_sv(&sub)); }

STC_INLINE bool cstr_istarts_with(const cstr* self, const char* sub) {
    csview sv = cstr_sv(self);
    intptr_t len = c_strlen(sub);
    return len <= sv.size && !utf8_icmp_sv(sv, c_sv(sub, len));
}


STC_INLINE bool cstr_ends_with_sv(const cstr* self, csview sub) {
    csview sv = cstr_sv(self);
    if (sub.size > sv.size) return false;
    return !c_memcmp(sv.buf + sv.size - sub.size, sub.buf, sub.size);
}

STC_INLINE bool cstr_ends_with_s(const cstr* self, cstr sub)
    { return cstr_ends_with_sv(self, cstr_sv(&sub)); }

STC_INLINE bool cstr_ends_with(const cstr* self, const char* sub)
    { return cstr_ends_with_sv(self, c_sv(sub, c_strlen(sub))); }

STC_INLINE bool cstr_iends_with(const cstr* self, const char* sub) {
    csview sv = cstr_sv(self);
    intptr_t n = c_strlen(sub);
    return n <= sv.size && !utf8_icmp(sv.buf + sv.size - n, sub);
}


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

#define cstr_replace(...) c_MACRO_OVERLOAD(cstr_replace, __VA_ARGS__)
#define cstr_replace_3(self, search, repl) cstr_replace_4(self, search, repl, INT32_MAX)
STC_INLINE void cstr_replace_4(cstr* self, const char* search, const char* repl, int32_t count) {
    cstr_take(self, cstr_replace_sv(cstr_sv(self), c_sv(search, c_strlen(search)),
                                                   c_sv(repl, c_strlen(repl)), count));
}

STC_INLINE void cstr_replace_at_sv(cstr* self, intptr_t pos, intptr_t len, const csview repl) {
    char* d = _cstr_internal_move(self, pos + len, pos + repl.size);
    c_memcpy(d + pos, repl.buf, repl.size);
}

STC_INLINE void cstr_replace_at(cstr* self, intptr_t pos, intptr_t len, const char* repl)
    { cstr_replace_at_sv(self, pos, len, c_sv(repl, c_strlen(repl))); }

STC_INLINE void cstr_replace_at_s(cstr* self, intptr_t pos, intptr_t len, cstr repl)
    { cstr_replace_at_sv(self, pos, len, cstr_sv(&repl)); }

STC_INLINE void cstr_u8_replace_at(cstr* self, intptr_t bytepos, intptr_t u8len, csview repl)
    { cstr_replace_at_sv(self, bytepos, utf8_pos(cstr_str(self) + bytepos, u8len), repl); }


STC_INLINE void cstr_insert(cstr* self, intptr_t pos, const char* str)
    { cstr_replace_at_sv(self, pos, 0, c_sv(str, c_strlen(str))); }

STC_INLINE void cstr_insert_sv(cstr* self, intptr_t pos, csview sv)
    { cstr_replace_at_sv(self, pos, 0, sv); }

STC_INLINE void cstr_insert_s(cstr* self, intptr_t pos, cstr s)
    { cstr_replace_at_sv(self, pos, 0, cstr_sv(&s)); }

STC_INLINE bool cstr_getline(cstr *self, FILE *fp)
    { return cstr_getdelim(self, '\n', fp); }

#endif // STC_CSTR_PRV_H_INCLUDED