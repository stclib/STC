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

/* A string type with short string optimization in C99 with good small-string
 * optimization (22 characters with 24 bytes string).
 */
#ifndef CSTR_H_INCLUDED
#define CSTR_H_INCLUDED

#if defined i_extern || defined STC_EXTERN
#  define _i_extern
#endif
#include "ccommon.h"
#include "forward.h"
#include "utf8.h"
#include <stdlib.h> /* malloc */
#include <stdarg.h>
#include <stdio.h> /* vsnprintf */

/**************************** PRIVATE API **********************************/

#if defined __GNUC__ && !defined __clang__
#  pragma GCC diagnostic push
#  pragma GCC diagnostic ignored "-Warray-bounds"
#  pragma GCC diagnostic ignored "-Wstringop-overflow="
#endif

enum  { cstr_s_cap =            sizeof(cstr_buf) - 2 };
#define cstr_s_size(s)          ((intptr_t)(s)->sml.size)
#define cstr_s_set_size(s, len) ((s)->sml.size = (uint8_t)(len), (s)->sml.data[len] = 0)
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
#define cstr_l_drop(s)          c_free((s)->lon.data)

#define cstr_is_long(s)         ((s)->sml.size > 127)
STC_API char* _cstr_init(cstr* self, intptr_t len, intptr_t cap);
STC_API char* _cstr_internal_move(cstr* self, intptr_t pos1, intptr_t pos2);

/**************************** PUBLIC API **********************************/

#define cstr_lit(literal) cstr_from_n(literal, c_litstrlen(literal))
#define cstr_NULL (c_LITERAL(cstr){{{0}, 0}})
#define cstr_toraw(self) cstr_str(self)

STC_API char*       cstr_reserve(cstr* self, intptr_t cap);
STC_API void        cstr_shrink_to_fit(cstr* self);
STC_API char*       cstr_resize(cstr* self, intptr_t size, char value);
STC_API intptr_t    cstr_find_at(const cstr* self, intptr_t pos, const char* search);
STC_API char*       cstr_assign_n(cstr* self, const char* str, intptr_t len);
STC_API char*       cstr_append_n(cstr* self, const char* str, intptr_t len);
STC_API bool        cstr_getdelim(cstr *self, int delim, FILE *fp);
STC_API void        cstr_erase(cstr* self, intptr_t pos, intptr_t len);
STC_API void        cstr_u8_erase(cstr* self, intptr_t bytepos, intptr_t u8len);
STC_API cstr        cstr_from_fmt(const char* fmt, ...);
STC_API intptr_t    cstr_append_fmt(cstr* self, const char* fmt, ...);
STC_API intptr_t    cstr_printf(cstr* self, const char* fmt, ...);
STC_API cstr        cstr_replace_sv(csview sv, csview search, csview repl, int32_t count);

STC_INLINE cstr_buf cstr_buffer(cstr* s) {
    return cstr_is_long(s)
        ? c_LITERAL(cstr_buf){s->lon.data, cstr_l_size(s), cstr_l_cap(s)}
        : c_LITERAL(cstr_buf){s->sml.data, cstr_s_size(s), cstr_s_cap};
}
STC_INLINE csview cstr_sv(const cstr* s) {
    return cstr_is_long(s) ? c_LITERAL(csview){s->lon.data, cstr_l_size(s)}
                           : c_LITERAL(csview){s->sml.data, cstr_s_size(s)};
}

STC_INLINE cstr cstr_init(void)
    { return cstr_NULL; }

STC_INLINE cstr cstr_from_n(const char* str, const intptr_t len) {
    cstr s;
    c_memcpy(_cstr_init(&s, len, len), str, len);
    return s;
}

STC_INLINE cstr cstr_from(const char* str)
    { return cstr_from_n(str, c_strlen(str)); }

STC_INLINE cstr cstr_from_sv(csview sv)
    { return cstr_from_n(sv.str, sv.size); }

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
    *self = cstr_NULL;
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

STC_INLINE void _cstr_set_size(cstr* self, intptr_t len)
    { SSO_CALL(self, set_size(self, len)); }

STC_INLINE char* cstr_data(cstr* self) 
    { return SSO_CALL(self, data(self)); }

STC_INLINE const char* cstr_str(const cstr* self)
    { return SSO_CALL(self, data(self)); }

STC_INLINE bool cstr_empty(const cstr* self) 
    { return self->sml.size == 0; }

STC_INLINE intptr_t cstr_size(const cstr* self)
    { return SSO_CALL(self, size(self)); }

STC_INLINE intptr_t cstr_capacity(const cstr* self)
    { return cstr_is_long(self) ? cstr_l_cap(self) : cstr_s_cap; }

// utf8 methods defined in/depending on src/utf8code.c:

extern cstr cstr_tocase(csview sv, int k);

STC_INLINE cstr cstr_casefold_sv(csview sv)
    { return cstr_tocase(sv, 0); }

STC_INLINE cstr cstr_tolower_sv(csview sv)
    { return cstr_tocase(sv, 1); }

STC_INLINE cstr cstr_toupper_sv(csview sv)
    { return cstr_tocase(sv, 2); }

STC_INLINE cstr cstr_tolower(const char* str) 
    { return cstr_tolower_sv(c_sv(str, c_strlen(str))); }

STC_INLINE cstr cstr_toupper(const char* str) 
    { return cstr_toupper_sv(c_sv(str, c_strlen(str))); }

STC_INLINE void cstr_lowercase(cstr* self) 
    { cstr_take(self, cstr_tolower_sv(cstr_sv(self))); }

STC_INLINE void cstr_uppercase(cstr* self) 
    { cstr_take(self, cstr_toupper_sv(cstr_sv(self))); }

STC_INLINE bool cstr_valid_utf8(const cstr* self)
    { return utf8_valid(cstr_str(self)); }

// other utf8 

STC_INLINE intptr_t cstr_u8_size(const cstr* self) 
    { return utf8_size(cstr_str(self)); }

STC_INLINE intptr_t cstr_u8_size_n(const cstr* self, intptr_t nbytes) 
    { return utf8_size_n(cstr_str(self), nbytes); }

STC_INLINE intptr_t cstr_u8_to_pos(const cstr* self, intptr_t u8idx)
    { return utf8_pos(cstr_str(self), u8idx); }

STC_INLINE const char* cstr_u8_at(const cstr* self, intptr_t u8idx) 
    { return utf8_at(cstr_str(self), u8idx); }

STC_INLINE csview cstr_u8_chr(const cstr* self, intptr_t u8idx) {
    const char* str = cstr_str(self);
    csview sv;
    sv.str = utf8_at(str, u8idx);
    sv.size = utf8_chr_size(sv.str);
    return sv;
}

// utf8 iterator

STC_INLINE cstr_iter cstr_begin(const cstr* self) { 
    csview sv = cstr_sv(self);
    if (!sv.size) return c_LITERAL(cstr_iter){NULL};
    return c_LITERAL(cstr_iter){.u8 = {{sv.str, utf8_chr_size(sv.str)}}};
}
STC_INLINE cstr_iter cstr_end(const cstr* self) {
    (void)self; return c_LITERAL(cstr_iter){NULL};
}
STC_INLINE void cstr_next(cstr_iter* it) {
    it->ref += it->u8.chr.size;
    it->u8.chr.size = utf8_chr_size(it->ref);
    if (!*it->ref) it->ref = NULL;
}
STC_INLINE cstr_iter cstr_advance(cstr_iter it, intptr_t pos) {
    int inc = -1;
    if (pos > 0) pos = -pos, inc = 1;
    while (pos && *it.ref) pos += (*(it.ref += inc) & 0xC0) != 0x80;
    it.u8.chr.size = utf8_chr_size(it.ref);
    if (!*it.ref) it.ref = NULL;
    return it;
}


STC_INLINE void cstr_clear(cstr* self)
    { _cstr_set_size(self, 0); }

STC_INLINE char* cstr_append_uninit(cstr *self, intptr_t len) {
    intptr_t sz = cstr_size(self);
    char* d = cstr_reserve(self, sz + len);
    if (!d) return NULL;
    _cstr_set_size(self, sz + len);
    return d + sz;
}

STC_INLINE int cstr_cmp(const cstr* s1, const cstr* s2) 
    { return strcmp(cstr_str(s1), cstr_str(s2)); }

STC_INLINE int cstr_icmp(const cstr* s1, const cstr* s2)
    { return utf8_icmp(cstr_str(s1), cstr_str(s2)); }

STC_INLINE bool cstr_eq(const cstr* s1, const cstr* s2) {
    csview x = cstr_sv(s1), y = cstr_sv(s2);
    return x.size == y.size && !c_memcmp(x.str, y.str, x.size);
}


STC_INLINE bool cstr_equals(const cstr* self, const char* str)
    { return !strcmp(cstr_str(self), str); }

STC_INLINE bool cstr_equals_sv(const cstr* self, csview sv)
    { return sv.size == cstr_size(self) && !c_memcmp(cstr_str(self), sv.str, sv.size); }

STC_INLINE bool cstr_equals_s(const cstr* self, cstr s)
    { return !cstr_cmp(self, &s); }

STC_INLINE bool cstr_iequals(const cstr* self, const char* str)
    { return !utf8_icmp(cstr_str(self), str); }


STC_INLINE intptr_t cstr_find(const cstr* self, const char* search) {
    const char *str = cstr_str(self), *res = strstr((char*)str, search);
    return res ? (res - str) : c_NPOS;
}

STC_API intptr_t cstr_find_sv(const cstr* self, csview search);

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
    return !c_memcmp(cstr_str(self), sub.str, sub.size);
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
    return !c_memcmp(sv.str + sv.size - sub.size, sub.str, sub.size);
}

STC_INLINE bool cstr_ends_with_s(const cstr* self, cstr sub)
    { return cstr_ends_with_sv(self, cstr_sv(&sub)); }

STC_INLINE bool cstr_ends_with(const cstr* self, const char* sub)
    { return cstr_ends_with_sv(self, c_sv(sub, c_strlen(sub))); }

STC_INLINE bool cstr_iends_with(const cstr* self, const char* sub) {
    csview sv = cstr_sv(self); 
    intptr_t n = c_strlen(sub);
    return n <= sv.size && !utf8_icmp(sv.str + sv.size - n, sub);
}


STC_INLINE char* cstr_assign(cstr* self, const char* str)
    { return cstr_assign_n(self, str, c_strlen(str)); }

STC_INLINE char* cstr_assign_sv(cstr* self, csview sv)
    { return cstr_assign_n(self, sv.str, sv.size); }

STC_INLINE char* cstr_copy(cstr* self, cstr s) {
    csview sv = cstr_sv(&s);
    return cstr_assign_n(self, sv.str, sv.size);
}


STC_INLINE char* cstr_push(cstr* self, const char* chr)
    { return cstr_append_n(self, chr, utf8_chr_size(chr)); }

STC_INLINE void cstr_pop(cstr* self) {
    csview sv = cstr_sv(self);
    const char* s = sv.str + sv.size;
    while ((*--s & 0xC0) == 0x80) ;
    _cstr_set_size(self, (s - sv.str));
}

STC_INLINE char* cstr_append(cstr* self, const char* str)
    { return cstr_append_n(self, str, c_strlen(str)); }

STC_INLINE void cstr_append_sv(cstr* self, csview sv)
    { cstr_append_n(self, sv.str, sv.size); }

STC_INLINE char* cstr_append_s(cstr* self, cstr s) {
    csview sv = cstr_sv(&s);
    return cstr_append_n(self, sv.str, sv.size);
}

#define cstr_replace(...) c_MACRO_OVERLOAD(cstr_replace, __VA_ARGS__)
#define cstr_replace_3(self, search, repl) cstr_replace_4(self, search, repl, INT32_MAX)
STC_INLINE void cstr_replace_4(cstr* self, const char* search, const char* repl, int32_t count) {
    cstr_take(self, cstr_replace_sv(cstr_sv(self), c_sv(search, c_strlen(search)),
                                                   c_sv(repl, c_strlen(repl)), count));
}

STC_INLINE void cstr_replace_at_sv(cstr* self, intptr_t pos, intptr_t len, const csview repl) {
    char* d = _cstr_internal_move(self, pos + len, pos + repl.size);
    c_memcpy(d + pos, repl.str, repl.size);
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

STC_INLINE void cstr_insert_s(cstr* self, intptr_t pos, cstr s) {
    csview sv = cstr_sv(&s);
    cstr_replace_at_sv(self, pos, 0, sv);
}


STC_INLINE bool cstr_getline(cstr *self, FILE *fp)
    { return cstr_getdelim(self, '\n', fp); }

STC_API uint64_t cstr_hash(const cstr *self);

#ifdef _i_extern
static struct {
    int      (*conv_asc)(int);
    uint32_t (*conv_utf)(uint32_t);
}
fn_tocase[] = {{tolower, utf8_casefold},
               {tolower, utf8_tolower},
               {toupper, utf8_toupper}};

cstr cstr_tocase(csview sv, int k) {
    cstr out = cstr_init();
    char *buf = cstr_reserve(&out, sv.size*3/2);
    const char *end = sv.str + sv.size;
    uint32_t cp; intptr_t sz = 0;
    utf8_decode_t d = {.state=0};

    while (sv.str < end) {
        do { utf8_decode(&d, (uint8_t)*sv.str++); } while (d.state);
        if (d.codep < 128)
            buf[sz++] = (char)fn_tocase[k].conv_asc((int)d.codep);
        else {
            cp = fn_tocase[k].conv_utf(d.codep);
            sz += utf8_encode(buf + sz, cp);
        }
    }
    _cstr_set_size(&out, sz);
    cstr_shrink_to_fit(&out);
    return out;
}
#endif

/* -------------------------- IMPLEMENTATION ------------------------- */
#if defined(i_implement)

STC_DEF uint64_t cstr_hash(const cstr *self) {
    csview sv = cstr_sv(self);
    return cfasthash(sv.str, sv.size);
}

STC_DEF intptr_t cstr_find_sv(const cstr* self, csview search) {
    csview sv = cstr_sv(self);
    char* res = cstrnstrn(sv.str, search.str, sv.size, search.size);
    return res ? (res - sv.str) : c_NPOS;
}

STC_DEF char* _cstr_internal_move(cstr* self, const intptr_t pos1, const intptr_t pos2) {
    cstr_buf r = cstr_buffer(self);
    if (pos1 != pos2) {
        const intptr_t newlen = (r.size + pos2 - pos1);
        if (newlen > r.cap)
            r.data = cstr_reserve(self, r.size*3/2 + pos2 - pos1);
        c_memmove(&r.data[pos2], &r.data[pos1], r.size - pos1);
        _cstr_set_size(self, newlen);
    }
    return r.data;
}

STC_DEF char* _cstr_init(cstr* self, const intptr_t len, const intptr_t cap) {
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
        c_memcpy(self->sml.data, r.data, r.size + 1);
        cstr_s_set_size(self, r.size);
        c_free(r.data);
    }
}

STC_DEF char* cstr_reserve(cstr* self, const intptr_t cap) {
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
        const intptr_t len = cstr_s_size(self);
        c_memcpy(data, self->sml.data, cstr_s_cap + 1);
        self->lon.data = data;
        self->lon.size = (size_t)len;
        cstr_l_set_cap(self, cap);
        return data;
    }
    return self->sml.data;
}

STC_DEF char* cstr_resize(cstr* self, const intptr_t size, const char value) {
    cstr_buf r = cstr_buffer(self);
    if (size > r.size) {
        if (size > r.cap && !(r.data = cstr_reserve(self, size)))
            return NULL;
        c_memset(r.data + r.size, value, size - r.size);
    }
    _cstr_set_size(self, size);
    return r.data;
}

STC_DEF intptr_t cstr_find_at(const cstr* self, const intptr_t pos, const char* search) {
    csview sv = cstr_sv(self);
    if (pos > sv.size) return c_NPOS;
    const char* res = strstr((char*)sv.str + pos, search);
    return res ? (res - sv.str) : c_NPOS;
}

STC_DEF char* cstr_assign_n(cstr* self, const char* str, const intptr_t len) {
    char* d = cstr_reserve(self, len);
    if (d) { c_memmove(d, str, len); _cstr_set_size(self, len); }
    return d;
}

STC_DEF char* cstr_append_n(cstr* self, const char* str, const intptr_t len) {
    cstr_buf r = cstr_buffer(self);
    if (r.size + len > r.cap) {
        const size_t off = (size_t)(str - r.data);
        r.data = cstr_reserve(self, r.size*3/2 + len);
        if (!r.data) return NULL;
        if (off <= (size_t)r.size) str = r.data + off; /* handle self append */
    }
    c_memcpy(r.data + r.size, str, len);
    _cstr_set_size(self, r.size + len);
    return r.data;
}

STC_DEF bool cstr_getdelim(cstr *self, const int delim, FILE *fp) {
    int c = fgetc(fp);
    if (c == EOF)
        return false;
    intptr_t pos = 0;
    cstr_buf r = cstr_buffer(self);
    for (;;) {
        if (c == delim || c == EOF) {
            _cstr_set_size(self, pos);
            return true;
        }
        if (pos == r.cap) {
            _cstr_set_size(self, pos);
            r.data = cstr_reserve(self, (r.cap = r.cap*3/2 + 16));
        }
        r.data[pos++] = (char) c;
        c = fgetc(fp);
    }
}

STC_DEF cstr
cstr_replace_sv(csview in, csview search, csview repl, int32_t count) {
    cstr out = cstr_NULL;
    intptr_t from = 0; char* res;
    if (!count) count = INT32_MAX;
    if (search.size)
        while (count-- && (res = cstrnstrn(in.str + from, search.str, in.size - from, search.size))) {
            const intptr_t pos = (res - in.str);
            cstr_append_n(&out, in.str + from, pos - from);
            cstr_append_n(&out, repl.str, repl.size);
            from = pos + search.size;
        }
    cstr_append_n(&out, in.str + from, in.size - from);
    return out;
}

STC_DEF void cstr_erase(cstr* self, const intptr_t pos, intptr_t len) {
    cstr_buf r = cstr_buffer(self);
    if (len > r.size - pos) len = r.size - pos;
    c_memmove(&r.data[pos], &r.data[pos + len], r.size - (pos + len));
    _cstr_set_size(self, r.size - len);
}

STC_DEF void cstr_u8_erase(cstr* self, const intptr_t bytepos, const intptr_t u8len) {
    cstr_buf r = cstr_buffer(self);
    intptr_t len = utf8_pos(r.data + bytepos, u8len);
    c_memmove(&r.data[bytepos], &r.data[bytepos + len], r.size - (bytepos + len));
    _cstr_set_size(self, r.size - len);
}

#if defined(__clang__)
#  pragma clang diagnostic push
#  pragma clang diagnostic ignored "-Wdeprecated-declarations"
#elif defined(_MSC_VER)
#  pragma warning(push)
#  pragma warning(disable: 4996)
#endif

STC_DEF intptr_t cstr_vfmt(cstr* self, intptr_t start, const char* fmt, va_list args) {
    va_list args2;
    va_copy(args2, args);
    const int n = vsnprintf(NULL, 0ULL, fmt, args);
    vsprintf(cstr_reserve(self, start + n) + start, fmt, args2);
    va_end(args2);
    _cstr_set_size(self, start + n);
    return n;
}
#if defined(__clang__)
#  pragma clang diagnostic pop
#elif defined(_MSC_VER)
#  pragma warning(pop)
#endif

STC_DEF cstr cstr_from_fmt(const char* fmt, ...) {
    cstr s = cstr_NULL;
    va_list args;
    va_start(args, fmt);
    cstr_vfmt(&s, 0, fmt, args);
    va_end(args);
    return s;
}

STC_DEF intptr_t cstr_append_fmt(cstr* self, const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    const intptr_t n = cstr_vfmt(self, cstr_size(self), fmt, args);
    va_end(args);
    return n;
}

/* NB! self-data in args is UB */
STC_DEF intptr_t cstr_printf(cstr* self, const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    const intptr_t n = cstr_vfmt(self, 0, fmt, args);
    va_end(args);
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
#undef _i_extern
