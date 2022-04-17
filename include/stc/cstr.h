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

#ifdef STC_USE_SSO
#include "alt/cstr.h"
#else
#ifndef CSTR_H_INCLUDED
#define CSTR_H_INCLUDED

#include "ccommon.h"
#include "forward.h"
#include <stdlib.h> /* malloc */
#include <string.h>
#include <stdarg.h>
#include <stdio.h> /* vsnprintf */
#include <ctype.h>

#define cstr_npos (SIZE_MAX >> 1)
typedef struct { size_t size, cap; char chr[1]; } cstr_priv;
#define _cstr_p(self) c_container_of((self)->str, cstr_priv, chr)
#ifdef _i_static 
    static cstr_priv _cstr_nullrep = {0, 0, {0}};
    static const cstr cstr_null = {_cstr_nullrep.chr};
#else
    extern const cstr cstr_null;
#endif
/* optimal memory: based on malloc_usable_size() sequence: 24, 40, 56, ... */
#define _cstr_opt_mem(cap)  ((((offsetof(cstr_priv, chr) + (cap) + 8)>>4)<<4) + 8)
/* optimal string capacity: 7, 23, 39, ... */
#define _cstr_opt_cap(cap)  (_cstr_opt_mem(cap) - offsetof(cstr_priv, chr) - 1)

STC_API cstr            cstr_from_n(const char* str, size_t n);
STC_API cstr            cstr_from_fmt(const char* fmt, ...);
STC_API cstr            cstr_from_replace_all(const char* str, size_t str_len,
                                              const char* find, size_t find_len,
                                              const char* repl, size_t repl_len);
STC_API char*           cstr_reserve(cstr* self, size_t cap);
STC_API void            cstr_resize(cstr* self, size_t len, char fill);
STC_API cstr*           cstr_assign_n(cstr* self, const char* str, size_t n);
STC_API int             cstr_printf(cstr* self, const char* fmt, ...);
STC_API cstr*           cstr_append_n(cstr* self, const char* str, size_t n);
STC_API void            cstr_replace_n(cstr* self, size_t pos, size_t len, const char* str, size_t n);
STC_API void            cstr_replace_all(cstr* self, const char* find, const char* replace);
STC_API void            cstr_erase_n(cstr* self, size_t pos, size_t n);
STC_API size_t          cstr_find(cstr s, const char* needle);
STC_API size_t          cstr_find_n(cstr s, const char* needle, size_t pos, size_t nmax);
STC_API bool            cstr_getdelim(cstr *self, int delim, FILE *stream);
STC_API void            cstr_replace_all(cstr* self, const char* find, const char* repl);

STC_INLINE cstr         cstr_init() { return cstr_null; }
#define                 cstr_toraw(self) (self)->str
#define                 cstr_new(literal) \
                            cstr_from_n(literal, c_strlen_lit(literal))
STC_INLINE cstr         cstr_from(const char* str)
                            { return cstr_from_n(str, strlen(str)); }
STC_INLINE const char*  cstr_str(const cstr* self) { return self->str; }
STC_INLINE char*        cstr_data(cstr* self) { return self->str; }
STC_INLINE size_t       cstr_size(cstr s) { return _cstr_p(&s)->size; }
STC_INLINE size_t       cstr_length(cstr s) { return _cstr_p(&s)->size; }
STC_INLINE size_t       cstr_capacity(cstr s) { return _cstr_p(&s)->cap; }
STC_INLINE bool         cstr_empty(cstr s) { return _cstr_p(&s)->size == 0; }
STC_INLINE void         cstr_drop(cstr* self)
                            { if (_cstr_p(self)->cap) c_free(_cstr_p(self)); }
STC_INLINE cstr         cstr_clone(cstr s)
                            { return cstr_from_n(s.str, _cstr_p(&s)->size); }
STC_INLINE void         cstr_clear(cstr* self)
                            { self->str[_cstr_p(self)->size = 0] = '\0'; }
STC_INLINE cstr*        cstr_assign(cstr* self, const char* str)
                            { return cstr_assign_n(self, str, strlen(str)); }
STC_INLINE cstr*        cstr_copy(cstr* self, cstr s)
                            { return cstr_assign_n(self, s.str, _cstr_p(&s)->size); }
STC_INLINE cstr*        cstr_append(cstr* self, const char* str)
                            { return cstr_append_n(self, str, strlen(str)); }
STC_INLINE cstr*        cstr_append_s(cstr* self, cstr s)
                            { return cstr_append_n(self, s.str, _cstr_p(&s)->size); }
STC_INLINE void         cstr_push_back(cstr* self, char value)
                            { cstr_append_n(self, &value, 1); }
STC_INLINE void         cstr_pop_back(cstr* self)
                            { self->str[ --_cstr_p(self)->size ] = '\0'; }
STC_INLINE void         cstr_insert_n(cstr* self, const size_t pos, const char* str, const size_t n)
                            { cstr_replace_n(self, pos, 0, str, n); }
STC_INLINE void         cstr_insert(cstr* self, const size_t pos, const char* str)
                            { cstr_replace_n(self, pos, 0, str, strlen(str)); }
STC_INLINE void         cstr_insert_s(cstr* self, const size_t pos, cstr s)
                            { cstr_replace_n(self, pos, 0, s.str, _cstr_p(&s)->size); }
STC_INLINE void         cstr_replace(cstr* self, const size_t pos, const size_t len, const char* str)
                            { cstr_replace_n(self, pos, len, str, strlen(str)); }
STC_INLINE void         cstr_replace_s(cstr* self, const size_t pos, const size_t len, cstr s)
                            { cstr_replace_n(self, pos, len, s.str, _cstr_p(&s)->size); }
STC_INLINE void         cstr_erase(cstr* self, const size_t pos)
                            { cstr_erase_n(self, pos, 1); }
STC_INLINE char*        cstr_front(cstr* self) { return self->str; }
STC_INLINE char*        cstr_back(cstr* self)
                            { return self->str + _cstr_p(self)->size - 1; }
STC_INLINE bool         cstr_equals(cstr s, const char* str)
                            { return strcmp(s.str, str) == 0; }
STC_INLINE bool         cstr_equals_s(cstr s1, cstr s2)
                            { return strcmp(s1.str, s2.str) == 0; }
STC_INLINE bool         cstr_contains(cstr s, const char* needle)
                            { return strstr(s.str, needle) != NULL; }
STC_INLINE bool         cstr_getline(cstr *self, FILE *stream)
                            { return cstr_getdelim(self, '\n', stream); }

STC_INLINE cstr_rep_t cstr_rep(cstr* s) { 
    cstr_priv* p = _cstr_p(s);
    return c_make(cstr_rep_t){s->str, p->size, p->cap};
}

STC_INLINE cstr cstr_with_capacity(const size_t cap) {
    cstr s = cstr_null;
    cstr_reserve(&s, cap);
    return s;
}

STC_INLINE cstr cstr_with_size(const size_t len, const char fill) {
    cstr s = cstr_null;
    cstr_resize(&s, len, fill);
    return s;
}

STC_INLINE cstr* cstr_take(cstr* self, cstr s) {
    if (self->str != s.str && _cstr_p(self)->cap)
        c_free(_cstr_p(self));
    self->str = s.str;
    return self;
}

STC_INLINE cstr cstr_move(cstr* self) {
    cstr tmp = *self;
    *self = cstr_null;
    return tmp;
}

STC_INLINE bool cstr_starts_with(cstr s, const char* sub) {
    while (*sub && *s.str == *sub) ++s.str, ++sub;
    return *sub == 0;
}

STC_INLINE bool cstr_ends_with(cstr s, const char* sub) {
    const size_t n = strlen(sub), sz = _cstr_p(&s)->size;
    return n <= sz && !memcmp(s.str + sz - n, sub, n);
}

STC_INLINE int c_strncasecmp(const char* s1, const char* s2, size_t nmax) {
    int ret = 0;
    while (nmax-- && (ret = tolower(*s1++) - tolower(*s2)) == 0 && *s2++)
        ;
    return ret;
}

/* container adaptor functions: */
#define  cstr_cmp(xp, yp)     strcmp((xp)->str, (yp)->str)
#define  cstr_eq(xp, yp)      (!cstr_cmp(xp, yp))
#define  cstr_hash(xp, dummy) c_strhash((xp)->str)

/* -------------------------- IMPLEMENTATION ------------------------- */
#if defined(_i_implement)

#ifndef _i_static
static cstr_priv _cstr_nullrep = {0, 0, {0}};
const cstr cstr_null = {_cstr_nullrep.chr};
#endif

STC_DEF char*
cstr_reserve(cstr* self, const size_t cap) {
    cstr_priv* p = _cstr_p(self);
    const size_t oldcap = p->cap;
    if (cap > oldcap) {
        p = (cstr_priv*) c_realloc(((oldcap != 0) & (p != &_cstr_nullrep)) ? p : NULL, _cstr_opt_mem(cap));
        self->str = p->chr;
        if (oldcap == 0) self->str[p->size = 0] = '\0';
        p->cap = _cstr_opt_cap(cap);
    }
    return self->str;
}

STC_DEF void
cstr_resize(cstr* self, const size_t len, const char fill) {
    const size_t n =  _cstr_p(self)->size;
    cstr_reserve(self, len);
    if (len > n) memset(self->str + n, fill, len - n);
    if (len | n) self->str[_cstr_p(self)->size = len] = '\0';
}

STC_DEF cstr
cstr_from_n(const char* str, const size_t n) {
    if (n == 0) return cstr_null;
    cstr_priv* prv = (cstr_priv*) c_malloc(_cstr_opt_mem(n));
    cstr s = {(char *) memcpy(prv->chr, str, n)};
    s.str[prv->size = n] = '\0';
    prv->cap = _cstr_opt_cap(n);
    return s;
}

#if defined(__clang__)
#  pragma clang diagnostic push
#  pragma clang diagnostic ignored "-Wdeprecated-declarations"
#elif defined(_MSC_VER)
#  pragma warning(push)
#  pragma warning(disable: 4996)
#endif

STC_DEF int
cstr_vfmt(cstr* self, const char* fmt, va_list args) {
    va_list args2;
    va_copy(args2, args);
    int len = vsnprintf(NULL, (size_t)0, fmt, args);
    cstr_reserve(self, len);
    vsprintf(self->str, fmt, args2);
    va_end(args2);
    return _cstr_p(self)->size = len;
}

#if defined(__clang__)
#  pragma clang diagnostic pop
#elif defined(_MSC_VER)
#  pragma warning(pop)
#endif

STC_DEF cstr
cstr_from_fmt(const char* fmt, ...) {
    cstr ret = cstr_null;
    va_list args; va_start(args, fmt);
    cstr_vfmt(&ret, fmt, args);
    va_end(args);
    return ret;
}

STC_DEF int
cstr_printf(cstr* self, const char* fmt, ...) {
    cstr ret = cstr_null;
    va_list args;
    va_start(args, fmt);
    int n = cstr_vfmt(&ret, fmt, args);
    va_end(args);
    cstr_drop(self);
    *self = ret;
    return n;
}

STC_DEF cstr*
cstr_assign_n(cstr* self, const char* str, const size_t n) {
    if (n || _cstr_p(self)->cap) {
        cstr_reserve(self, n);
        memmove(self->str, str, n);
        self->str[_cstr_p(self)->size = n] = '\0';
    }
    return self;
}

STC_DEF cstr*
cstr_append_n(cstr* self, const char* str, const size_t n) {
    if (n == 0) return self;
    const size_t oldlen = _cstr_p(self)->size, newlen = oldlen + n;
    if (newlen > _cstr_p(self)->cap) {
        const size_t off = (size_t) (str - self->str); /* handle self append */
        cstr_reserve(self, (oldlen*3 >> 1) + n);
        if (off <= oldlen) str = self->str + off;
    }
    memcpy(&self->str[oldlen], str, n);
    self->str[_cstr_p(self)->size = newlen] = '\0';
    return self;
}

STC_INLINE void _cstr_internal_move(cstr* self, const size_t pos1, const size_t pos2) {
    if (pos1 == pos2)
        return;
    const size_t len = _cstr_p(self)->size, newlen = len + pos2 - pos1;
    if (newlen > _cstr_p(self)->cap)
        cstr_reserve(self, (len*3 >> 1) + pos2 - pos1);
    memmove(&self->str[pos2], &self->str[pos1], len - pos1);
    self->str[_cstr_p(self)->size = newlen] = '\0';
}

STC_DEF void
cstr_replace_n(cstr* self, const size_t pos, size_t len, const char* str, const size_t n) {
    const size_t sz = cstr_size(*self);
    if (len > sz - pos) len = sz - pos;
    c_autobuf (xstr, char, n) {
        memcpy(xstr, str, n);
        _cstr_internal_move(self, pos + len, pos + n);
        memcpy(&self->str[pos], xstr, n);
    }
}

STC_DEF cstr
cstr_from_replace_all(const char* str, const size_t str_len,
                      const char* find, const size_t find_len,
                      const char* repl, const size_t repl_len) {
    cstr out = cstr_null;
    size_t from = 0; char* res;
    if (find_len)
        while ((res = c_strnstrn(str + from, find, str_len - from, find_len))) {
            const size_t pos = res - str;
            cstr_append_n(&out, str + from, pos - from);
            cstr_append_n(&out, repl, repl_len);
            from = pos + find_len;
        }
    cstr_append_n(&out, str + from, str_len - from);
    return out;
}

STC_DEF void
cstr_replace_all(cstr* self, const char* find, const char* repl) {
    cstr_take(self, cstr_from_replace_all(self->str, _cstr_p(self)->size,
                                          find, strlen(find), repl, strlen(repl)));
}

STC_DEF void
cstr_erase_n(cstr* self, const size_t pos, size_t n) {
    const size_t len = _cstr_p(self)->size;
    if (n > len - pos) n = len - pos;
    if (len) {
        memmove(&self->str[pos], &self->str[pos + n], len - (pos + n));
        self->str[_cstr_p(self)->size -= n] = '\0';
    }
}

STC_DEF bool
cstr_getdelim(cstr *self, const int delim, FILE *fp) {
    size_t pos = 0, cap = _cstr_p(self)->cap;
    char* d = self->str;
    int c = fgetc(fp);
    if (c == EOF)
        return false;
    for (;;) {
        if (c == delim || c == EOF) {
            if (cap) d[_cstr_p(self)->size = pos] = '\0';
            return true;
        }
        if (pos == cap) {
            d = cstr_reserve(self, (cap*3 >> 1) + 16);
            cap = cstr_capacity(*self);
        }
        d[pos++] = (char) c;
        c = fgetc(fp);
    }
}

STC_DEF size_t
cstr_find(cstr s, const char* needle) {
    char* res = strstr(s.str, needle);
    return res ? res - s.str : cstr_npos;
}

STC_DEF size_t
cstr_find_n(cstr s, const char* needle, const size_t pos, const size_t nmax) {
    if (pos > _cstr_p(&s)->size) return cstr_npos;
    const size_t nlen = strlen(needle);
    char* res = c_strnstrn(s.str + pos, needle, _cstr_p(&s)->size - pos, nmax < nlen ? nmax : nlen);
    return res ? res - s.str : cstr_npos;
}

#endif
#endif
#endif
#undef i_opt