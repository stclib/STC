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

/* A string type with short string optimization in C99 with optimal short string
 * utilization (23 characters with 24 bytes string representation).
 */
#ifdef STC_CSTR_V1
#include "alt/cstr.h"
#else
#ifndef CSTR_H_INCLUDED
#define CSTR_H_INCLUDED

#include "ccommon.h"
#include "forward.h"
#include "utf8.h"
#include <stdlib.h> /* malloc */
#include <stdarg.h>
#include <stdio.h> /* vsnprintf */
#include <ctype.h>

/**************************** PRIVATE API **********************************/

#if defined __GNUC__ && !defined __clang__
#  pragma GCC diagnostic push
#  pragma GCC diagnostic ignored "-Warray-bounds"
#  pragma GCC diagnostic ignored "-Wstringop-overflow="
#endif

enum  { cstr_s_cap =            sizeof(cstr_buf) - 1 };
#define cstr_s_size(s)          ((size_t)(cstr_s_cap - (s)->sml.last))
#define cstr_s_set_size(s, len) ((s)->sml.last = cstr_s_cap - (len), (s)->sml.data[len] = 0)
#define cstr_s_data(s)          (s)->sml.data
#define cstr_s_end(s)           ((s)->sml.data + cstr_s_size(s))

#if defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    #define byte_rotl_(x, b)       ((x) << (b)*8 | (x) >> (sizeof(x) - (b))*8)
    #define cstr_l_cap(s)          (~byte_rotl_((s)->lon.ncap, sizeof((s)->lon.ncap) - 1))
    #define cstr_l_set_cap(s, cap) ((s)->lon.ncap = ~byte_rotl_(cap, 1))
#else
    #define cstr_l_cap(s)          (~(s)->lon.ncap)
    #define cstr_l_set_cap(s, cap) ((s)->lon.ncap = ~(cap))
#endif
#define cstr_l_size(s)          ((s)->lon.size)
#define cstr_l_set_size(s, len) ((s)->lon.data[(s)->lon.size = (len)] = 0)
#define cstr_l_data(s)          (s)->lon.data
#define cstr_l_end(s)           ((s)->lon.data + cstr_l_size(s))
#define cstr_l_drop(s)          c_free((s)->lon.data)

#define cstr_is_long(s)         ((s)->sml.last > 127)
STC_API char* _cstr_init(cstr* self, size_t len, size_t cap);
STC_API char* _cstr_internal_move(cstr* self, size_t pos1, size_t pos2);

/**************************** PUBLIC API **********************************/

#define cstr_new(literal) cstr_from_n(literal, c_strlen_lit(literal))
#define cstr_npos (SIZE_MAX >> 1)
#define cstr_null (c_make(cstr){.sml = {.last = cstr_s_cap}})
#define cstr_toraw(self) cstr_str(self)

STC_API char*   cstr_reserve(cstr* self, size_t cap);
STC_API void    cstr_shrink_to_fit(cstr* self);
STC_API void    cstr_resize(cstr* self, size_t size, char value);
STC_API size_t  cstr_find_from(cstr s, size_t pos, const char* search);
STC_API char*   cstr_assign_n(cstr* self, const char* str, size_t n);
STC_API char*   cstr_append_n(cstr* self, const char* str, size_t n);
STC_API bool    cstr_getdelim(cstr *self, int delim, FILE *fp);
STC_API void    cstr_erase_n(cstr* self, size_t pos, size_t n);
STC_API cstr    cstr_from_fmt(const char* fmt, ...);
STC_API int     cstr_printf(cstr* self, const char* fmt, ...);
STC_API void    cstr_replace_all(cstr* self, const char* search, const char* repl);

STC_INLINE cstr_buf cstr_buffer(cstr* s) {
    return cstr_is_long(s)
        ? c_make(cstr_buf){s->lon.data, cstr_l_size(s), cstr_l_cap(s)}
        : c_make(cstr_buf){s->sml.data, cstr_s_size(s), cstr_s_cap};
}
STC_INLINE csview cstr_sv(const cstr* s) {
    return cstr_is_long(s) ? c_make(csview){s->lon.data, cstr_l_size(s)}
                           : c_make(csview){s->sml.data, cstr_s_size(s)};
}

STC_INLINE cstr cstr_init(void)
    { return cstr_null; }

STC_INLINE cstr cstr_from_n(const char* str, const size_t n) {
    cstr s;
    memcpy(_cstr_init(&s, n, n), str, n);
    return s;
}

STC_INLINE cstr cstr_from(const char* str)
    { return cstr_from_n(str, strlen(str)); }

STC_INLINE cstr cstr_with_size(const size_t size, const char value) {
    cstr s;
    memset(_cstr_init(&s, size, size), value, size);
    return s;
}

STC_INLINE cstr cstr_with_capacity(const size_t cap) {
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
    return cstr_from_n(sv.str, sv.size);
}

STC_INLINE void cstr_drop(cstr* self) {
    if (cstr_is_long(self))
        cstr_l_drop(self);
}

#define SSO_CALL(s, call) (cstr_is_long(s) ? cstr_l_##call : cstr_s_##call)

STC_INLINE void _cstr_set_size(cstr* self, size_t len)
    { SSO_CALL(self, set_size(self, len)); }

STC_INLINE char* cstr_data(cstr* self) 
    { return SSO_CALL(self, data(self)); }

STC_INLINE const char* cstr_str(const cstr* self)
    { return SSO_CALL(self, data(self)); }

STC_INLINE bool cstr_empty(cstr s) 
    { return s.sml.last == cstr_s_cap; }

STC_INLINE size_t cstr_size(cstr s)
    { return SSO_CALL(&s, size(&s)); }

STC_INLINE size_t cstr_length(cstr s)
    { return SSO_CALL(&s, size(&s)); }

STC_INLINE size_t cstr_capacity(cstr s)
    { return cstr_is_long(&s) ? cstr_l_cap(&s) : cstr_s_cap; }

// utf8 methods defined in/depending on src/utf8code.c:

extern cstr cstr_tolower(const cstr* self);
extern cstr cstr_toupper(const cstr* self);
extern void cstr_lowercase(cstr* self);
extern void cstr_uppercase(cstr* self);

STC_INLINE bool cstr_valid_u8(const cstr* self) 
    { return utf8_valid(cstr_str(self)); }

// other utf8 

STC_INLINE size_t cstr_size_u8(cstr s) 
    { return utf8_size(cstr_str(&s)); }

STC_INLINE size_t cstr_size_n_u8(cstr s, size_t nbytes) 
    { return utf8_size_n(cstr_str(&s), nbytes); }

STC_INLINE csview cstr_at(const cstr* self, size_t bytepos) {
    csview sv = cstr_sv(self);
    sv.str += bytepos;
    sv.size = utf8_codep_size(sv.str);
    return sv;
}

STC_INLINE csview cstr_at_u8(const cstr* self, size_t u8idx) {
    csview sv = cstr_sv(self);
    sv.str = utf8_at(sv.str, u8idx);
    sv.size = utf8_codep_size(sv.str);
    return sv;
}

STC_INLINE size_t cstr_pos_u8(const cstr* self, size_t u8idx) 
    { return utf8_pos(cstr_str(self), u8idx); }

// utf8 iterator

STC_INLINE cstr_iter cstr_begin(const cstr* self) { 
    const char* str = cstr_str(self);
    return c_make(cstr_iter){.chr = {str, utf8_codep_size(str)}};
}
STC_INLINE cstr_iter cstr_end(const cstr* self) {
    csview sv = cstr_sv(self);
    return c_make(cstr_iter){sv.str + sv.size};
}
STC_INLINE void cstr_next(cstr_iter* it) {
    it->ref += it->chr.size;
    it->chr.size = utf8_codep_size(it->ref);
}


STC_INLINE void cstr_clear(cstr* self)
    { _cstr_set_size(self, 0); }

STC_INLINE char* cstr_expand_uninit(cstr *self, size_t n) {
    size_t len = cstr_size(*self); char* d;
    if (!(d = cstr_reserve(self, len + n))) return NULL;
    _cstr_set_size(self, len + n);
    return d + len;
}

STC_INLINE int cstr_cmp(const cstr* s1, const cstr* s2) 
    { return strcmp(cstr_str(s1), cstr_str(s2)); }

STC_INLINE int cstr_icmp(const cstr* s1, const cstr* s2)
    { return utf8_icmp(cstr_str(s1), cstr_str(s2)); }

STC_INLINE bool cstr_eq(const cstr* s1, const cstr* s2) {
    csview x = cstr_sv(s1), y = cstr_sv(s2);
    return x.size == y.size && !memcmp(x.str, y.str, x.size);
}

STC_INLINE bool cstr_equals(cstr s1, const char* str)
    { return !strcmp(cstr_str(&s1), str); }

STC_INLINE bool cstr_iequals(cstr s1, const char* str)
    { return !utf8_icmp(cstr_str(&s1), str); }

STC_INLINE bool cstr_equals_s(cstr s1, cstr s2)
    { return !cstr_cmp(&s1, &s2); }

STC_INLINE size_t cstr_find(cstr s, const char* search) {
    const char *str = cstr_str(&s), *res = strstr((char*)str, search);
    return res ? res - str : cstr_npos;
}

STC_INLINE size_t cstr_find_s(cstr s, cstr search)
    { return cstr_find(s, cstr_str(&search)); }

STC_INLINE bool cstr_contains(cstr s, const char* search)
    { return strstr(cstr_data(&s), search) != NULL; }

STC_INLINE bool cstr_contains_s(cstr s, cstr search)
    { return strstr(cstr_data(&s), cstr_str(&search)) != NULL; }

STC_INLINE bool cstr_starts_with(cstr s, const char* sub) {
    const char* str = cstr_str(&s);
    while (*sub && *str == *sub) ++str, ++sub;
    return !*sub;
}
STC_INLINE bool cstr_istarts_with(cstr s, const char* sub) {
    csview sv = cstr_sv(&s); 
    size_t n = strlen(sub);
    return n <= sv.size && !utf8_icmp_n(cstr_npos, sv.str, sv.size, sub, n);
}

STC_INLINE bool cstr_starts_with_s(cstr s, cstr sub)
    { return cstr_starts_with(s, cstr_str(&sub)); }

STC_INLINE bool cstr_ends_with(cstr s, const char* sub) {
    csview sv = cstr_sv(&s);
    size_t n = strlen(sub);
    return n <= sv.size && !memcmp(sv.str + sv.size - n, sub, n);
}

STC_INLINE bool cstr_iends_with(cstr s, const char* sub) {
    csview sv = cstr_sv(&s); 
    size_t n = strlen(sub);
    return n <= sv.size && !utf8_icmp(sv.str + sv.size - n, sub);
}

STC_INLINE bool cstr_ends_with_s(cstr s, cstr sub)
    { return cstr_ends_with(s, cstr_str(&sub)); }

STC_INLINE char* cstr_assign(cstr* self, const char* str)
    { return cstr_assign_n(self, str, strlen(str)); }

STC_INLINE char* cstr_assign_s(cstr* self, cstr s) {
    csview sv = cstr_sv(&s);
    return cstr_assign_n(self, sv.str, sv.size);
}

STC_INLINE void cstr_copy(cstr* self, cstr s)
    { cstr_assign_s(self, s); }

STC_INLINE char* cstr_append(cstr* self, const char* str)
    { return cstr_append_n(self, str, strlen(str)); }

STC_INLINE char* cstr_append_s(cstr* self, cstr s) {
    csview sv = cstr_sv(&s);
    return cstr_append_n(self, sv.str, sv.size);
}

STC_INLINE void cstr_replace_n(cstr* self, size_t pos, size_t len, const char* repl, size_t n) {
    char* d = _cstr_internal_move(self, pos + len, pos + n);
    memcpy(d + pos, repl, n);
}

STC_INLINE void cstr_replace_at(cstr* self, size_t pos, size_t len, const char* repl)
    { cstr_replace_n(self, pos, len, repl, strlen(repl)); }

STC_INLINE size_t cstr_replace_from(cstr* self, size_t pos, const char* search, const char* repl) {
    pos = cstr_find_from(*self, pos, search);
    if (pos == cstr_npos)
        return pos;
    const size_t rlen = strlen(repl);
    cstr_replace_n(self, pos, strlen(search), repl, rlen);
    return pos + rlen;
}

STC_INLINE size_t cstr_replace(cstr* self, const char* search, const char* repl)
    { return cstr_replace_from(self, 0, search, repl); }

STC_INLINE void cstr_replace_s(cstr* self, size_t pos, size_t len, cstr s) {
    csview sv = cstr_sv(&s);
    cstr_replace_n(self, pos, len, sv.str, sv.size);
}

STC_INLINE void cstr_insert_n(cstr* self, size_t pos, const char* str, size_t n)
    { cstr_replace_n(self, pos, 0, str, n); }

STC_INLINE void cstr_insert(cstr* self, size_t pos, const char* str)
    { cstr_replace_n(self, pos, 0, str, strlen(str)); }

STC_INLINE void cstr_insert_s(cstr* self, size_t pos, cstr s) {
    csview sv = cstr_sv(&s);
    cstr_replace_n(self, pos, 0, sv.str, sv.size);
}

STC_INLINE bool cstr_getline(cstr *self, FILE *fp)
    { return cstr_getdelim(self, '\n', fp); }

STC_INLINE uint64_t cstr_hash(const cstr *self) {
    csview sv = cstr_sv(self);
    return c_fasthash(sv.str, sv.size);
}

/* -------------------------- IMPLEMENTATION ------------------------- */
#if defined(i_implement) || defined(i_extern)

STC_DEF char* _cstr_internal_move(cstr* self, const size_t pos1, const size_t pos2) {
    cstr_buf r = cstr_buffer(self);
    if (pos1 != pos2) {
        const size_t newlen = r.size + pos2 - pos1;
        if (newlen > r.cap)
            r.data = cstr_reserve(self, (r.size*3 >> 1) + pos2 - pos1);
        memmove(&r.data[pos2], &r.data[pos1], r.size - pos1);
        _cstr_set_size(self, newlen);
    }
    return r.data;
}

STC_DEF char* _cstr_init(cstr* self, const size_t len, const size_t cap) {
    if (cap > cstr_s_cap) {
        self->lon.data = (char *)c_malloc(cap + 1);
        cstr_l_set_size(self, len);
        cstr_l_set_cap(self, cap);
        return self->lon.data;
    }
    cstr_s_set_size(self, len);
    return self->sml.data;
}

STC_DEF void cstr_shrink_to_fit(cstr* self) {
    cstr_buf r = cstr_buffer(self);
    if (r.size == r.cap)
        return;
    if (r.size > cstr_s_cap) {
        self->lon.data = (char *)c_realloc(self->lon.data, r.size + 1);
        cstr_l_set_cap(self, r.size);
    } else if (r.cap > cstr_s_cap) {
        memcpy(self->sml.data, r.data, r.size + 1);
        cstr_s_set_size(self, r.size);
        c_free(r.data);
    }
}

STC_DEF char* cstr_reserve(cstr* self, const size_t cap) {
    if (cstr_is_long(self)) {
        if (cap > cstr_l_cap(self)) {
            self->lon.data = (char *)c_realloc(self->lon.data, cap + 1);
            cstr_l_set_cap(self, cap);
        }
        return self->lon.data;
    }
    /* from short to long: */
    if (cap > cstr_s_cap) {
        char* data = (char *)c_malloc(cap + 1);
        const size_t len = cstr_s_size(self);
        memcpy(data, self->sml.data, len);
        self->lon.data = data;
        cstr_l_set_size(self, len);
        cstr_l_set_cap(self, cap);
        return data;
    }
    return self->sml.data;
}

STC_DEF void cstr_resize(cstr* self, const size_t size, const char value) {
    cstr_buf r = cstr_buffer(self);
    if (size > r.size) {
        if (size > r.cap) r.data = cstr_reserve(self, size);
        memset(r.data + r.size, value, size - r.size);
    }
    _cstr_set_size(self, size);
}

STC_DEF size_t cstr_find_from(cstr s, const size_t pos, const char* search) {
    csview sv = cstr_sv(&s);
    if (pos > sv.size) return cstr_npos;
    const char* res = strstr((char*)sv.str + pos, search);
    return res ? res - sv.str : cstr_npos;
}

STC_DEF char* cstr_assign_n(cstr* self, const char* str, const size_t n) {
    char* d = cstr_reserve(self, n);
    memmove(d, str, n);
    _cstr_set_size(self, n);
    return d;
}

STC_DEF char* cstr_append_n(cstr* self, const char* str, const size_t n) {
    cstr_buf r = cstr_buffer(self);
    if (r.size + n > r.cap) {
        const size_t off = (size_t)(str - r.data);
        r.data = cstr_reserve(self, (r.size*3 >> 1) + n);
        if (off <= r.size) str = r.data + off; /* handle self append */
    }
    memcpy(r.data + r.size, str, n);
    _cstr_set_size(self, r.size + n);
    return r.data;
}

STC_DEF bool cstr_getdelim(cstr *self, const int delim, FILE *fp) {
    int c = fgetc(fp);
    if (c == EOF)
        return false;
    size_t pos = 0;
    cstr_buf r = cstr_buffer(self);
    for (;;) {
        if (c == delim || c == EOF) {
            _cstr_set_size(self, pos);
            return true;
        }
        if (pos == r.cap) {
            _cstr_set_size(self, pos);
            r.data = cstr_reserve(self, (r.cap = (r.cap*3 >> 1) + 16));
        }
        r.data[pos++] = (char) c;
        c = fgetc(fp);
    }
}

STC_DEF cstr
cstr_from_replace_all(const char* str, const size_t str_len,
                      const char* search, const size_t search_len,
                      const char* repl, const size_t repl_len) {
    cstr out = cstr_null;
    size_t from = 0; char* res;
    if (search_len)
        while ((res = c_strnstrn(str + from, search, str_len - from, search_len))) {
            const size_t pos = res - str;
            cstr_append_n(&out, str + from, pos - from);
            cstr_append_n(&out, repl, repl_len);
            from = pos + search_len;
        }
    cstr_append_n(&out, str + from, str_len - from);
    return out;
}

STC_DEF void
cstr_replace_all(cstr* self, const char* search, const char* repl) {
    csview sv = cstr_sv(self);
    cstr_take(self, cstr_from_replace_all(sv.str, sv.size, search, strlen(search),
                                                           repl, strlen(repl)));
}

STC_DEF void cstr_erase_n(cstr* self, const size_t pos, size_t n) {
    cstr_buf r = cstr_buffer(self);
    if (n > r.size - pos) n = r.size - pos;
    memmove(&r.data[pos], &r.data[pos + n], r.size - (pos + n));
    _cstr_set_size(self, r.size - n);
}

#if defined(__clang__)
#  pragma clang diagnostic push
#  pragma clang diagnostic ignored "-Wdeprecated-declarations"
#elif defined(_MSC_VER)
#  pragma warning(push)
#  pragma warning(disable: 4996)
#endif

STC_DEF int cstr_vfmt(cstr* self, const char* fmt, va_list args) {
    va_list args2;
    va_copy(args2, args);
    const int n = vsnprintf(NULL, (size_t)0, fmt, args);
    cstr_reserve(self, n);
    vsprintf(cstr_data(self), fmt, args2);
    va_end(args2);
    _cstr_set_size(self, n);
    return n;
}
#if defined(__clang__)
#  pragma clang diagnostic pop
#elif defined(_MSC_VER)
#  pragma warning(pop)
#endif

STC_DEF cstr cstr_from_fmt(const char* fmt, ...) {
    cstr s = cstr_null;
    va_list args; va_start(args, fmt);
    cstr_vfmt(&s, fmt, args);
    va_end(args);
    return s;
}

STC_DEF int cstr_printf(cstr* self, const char* fmt, ...) {
    cstr s = cstr_null;
    va_list args; va_start(args, fmt);
    const int n = cstr_vfmt(&s, fmt, args);
    va_end(args);
    cstr_drop(self); *self = s;
    return n;
}

#endif // i_implement
#if defined __GNUC__ && !defined __clang__
#  pragma GCC diagnostic pop
#endif
#endif // CSTR_H_INCLUDED
#undef i_opt
#undef i_header
#undef i_static
#undef i_implement
#undef i_extern
#endif // !STC_CSTR_V1
