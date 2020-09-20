/* MIT License
 *
 * Copyright (c) 2020 Tyge Løvset, NORCE, www.norceresearch.no
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
#ifndef CSTR__H__
#define CSTR__H__

#include <stdlib.h> /* malloc */
#include <string.h>
#include <stdarg.h>
#include <stdio.h> /* vsnprintf */
#include "cdefs.h"

typedef struct cstr { char* str; } cstr_t;
typedef struct { char *get; } cstr_iter_t;
typedef char cstr_value_t;

static size_t _cstr_nullrep[] = {0, 0, 0};

static  cstr_t cstr_INIT = {(char* ) &_cstr_nullrep[2]};
#define cstr_size(s)       ((const size_t *) (s).str)[-2]
#define cstr_capacity(s)   ((const size_t *) (s).str)[-1]
#define cstr_empty(s)      (cstr_size(s) == 0)
#define cstr_NPOS          ((size_t) (-1))

STC_API cstr_t
cstr_n(const char* str, size_t len);
STC_API cstr_t
cstr_from(const char* fmt, ...);
STC_API void
cstr_reserve(cstr_t* self, size_t cap);
STC_API void
cstr_resize(cstr_t* self, size_t len, char fill);
STC_API cstr_t*
cstr_assign_n(cstr_t* self, const char* str, size_t len);
STC_API cstr_t*
cstr_append_n(cstr_t* self, const char* str, size_t len);
STC_API void
cstr_replace_n(cstr_t* self, size_t pos, size_t len, const char* str, size_t n);
STC_API void
cstr_erase(cstr_t* self, size_t pos, size_t n);
STC_API char*
c_strnstr(const char* s, const char* needle, size_t n);

#define _cstr_rep(self) (((size_t *) (self)->str) - 2)
#define _cstr_size(s)   ((size_t *) (s).str)[-2]
/* match common malloc_usable_size() sequence: 24, 40, 56, ... */
#define _cstr_mem(size)  ((((size) + 24) >> 4) * 16 + 8)
/* gives true string capacity: 7, 23, 39, ... */
#define _cstr_cap(size)  ((((size) + 24) >> 4) * 16 - 9)

STC_INLINE cstr_t
cstr_init() {return cstr_INIT;}

STC_INLINE void
cstr_del(cstr_t* self) {
    if (cstr_capacity(*self))
        c_free(_cstr_rep(self));
}

STC_INLINE cstr_t
cstr_with_capacity(size_t cap) {
    cstr_t s = cstr_INIT;
    cstr_reserve(&s, cap);
    return s;
}
STC_INLINE cstr_t
cstr_with_size(size_t len, char fill) {
    cstr_t s = cstr_INIT;
    cstr_resize(&s, len, fill);
    return s;
}
STC_INLINE cstr_t
cstr(const char* str) {
    return cstr_n(str, strlen(str));
}

STC_INLINE cstr_t
cstr_clone(cstr_t s) {
    return cstr_n(s.str, cstr_size(s));
}

STC_INLINE void
cstr_clear(cstr_t* self) {
    self->str[_cstr_size(*self) = 0] = '\0';
}

STC_INLINE char*
cstr_front(cstr_t* self) {return self->str;}
STC_INLINE char*
cstr_back(cstr_t* self) {return self->str + _cstr_size(*self) - 1;}

STC_INLINE cstr_iter_t
cstr_begin(cstr_t* self) {cstr_iter_t it = {self->str}; return it;}
STC_INLINE cstr_iter_t
cstr_end(cstr_t* self) {
    cstr_iter_t it = {self->str + cstr_size(*self)}; return it;
}
STC_INLINE void cstr_next(cstr_iter_t* it) { ++it->get; }
STC_INLINE char* cstr_itval(cstr_iter_t it) {return it.get;}

STC_INLINE cstr_t*
cstr_assign(cstr_t* self, const char* str) {
    return cstr_assign_n(self, str, strlen(str));
}

STC_INLINE cstr_t*
cstr_take(cstr_t* self, cstr_t s) {
    if (self->str != s.str && cstr_capacity(*self))
        c_free(_cstr_rep(self));
    self->str = s.str;
    return self;
}
STC_INLINE cstr_t
cstr_move(cstr_t* self) {
    cstr_t tmp = *self;
    *self = cstr_INIT;
    return tmp;
}

STC_INLINE cstr_t*
cstr_append(cstr_t* self, const char* str) {
    return cstr_append_n(self, str, strlen(str));
}
STC_INLINE cstr_t*
cstr_push_back(cstr_t* self, char value) {
    return cstr_append_n(self, &value, 1);
}

STC_INLINE void
cstr_pop_back(cstr_t* self) {
    self->str[ --_cstr_size(*self) ] = '\0';
}

STC_INLINE void
cstr_insert_n(cstr_t* self, size_t pos, const char* str, size_t n) {
    cstr_replace_n(self, pos, 0, str, n);
}
STC_INLINE void
cstr_insert(cstr_t* self, size_t pos, const char* str) {
    cstr_replace_n(self, pos, 0, str, strlen(str));
}
STC_INLINE void
cstr_replace(cstr_t* self, size_t pos, size_t len, const char* str) {
    cstr_replace_n(self, pos, len, str, strlen(str));
}

/* readonly */

STC_INLINE bool
cstr_equals(cstr_t s1, const char* str) {
    return strcmp(s1.str, str) == 0;
}
STC_INLINE bool
cstr_equals_s(cstr_t s1, cstr_t s2) {
    return strcmp(s1.str, s2.str) == 0;
}
STC_INLINE int
cstr_compare(const cstr_t *s1, const cstr_t *s2) {
    return strcmp(s1->str, s2->str);
}
STC_INLINE size_t
cstr_find_n(const cstr_t* s, const char* needle, size_t pos, size_t n) {
    char* res = c_strnstr(s->str + pos, needle, n);
    return res ? res - s->str : cstr_NPOS;
}
STC_INLINE size_t
cstr_find(const cstr_t* s, const char* needle) {
    char* res = strstr(s->str, needle);
    return res ? res - s->str : cstr_NPOS;
}

/* cvec/cmap API functions: */

#define  cstr_to_raw(x)          ((x)->str)
#define  cstr_compare_raw(x, y)  strcmp(*(x), *(y))
#define  cstr_equals_raw(x, y)   (strcmp(*(x), *(y)) == 0)

STC_INLINE uint32_t c_string_hash(const char* str) {
    uint32_t hash = 5381, c; /* djb2 */
    while ((c = *str++)) hash = ((hash << 5) + hash) ^ c;
    return hash;
}

STC_INLINE uint32_t cstr_hash_raw(const char* const* spp, size_t ignored) {
    return c_string_hash(*spp);
}

/* -------------------------- IMPLEMENTATION ------------------------- */

#if !defined(STC_HEADER) || defined(STC_IMPLEMENTATION)

STC_API void
cstr_reserve(cstr_t* self, size_t cap) {
    size_t len = cstr_size(*self), oldcap = cstr_capacity(*self);
    if (cap > oldcap) {
        size_t* rep = (size_t *) c_realloc(oldcap ? _cstr_rep(self) : NULL, _cstr_mem(cap));
        self->str = (char *) (rep + 2);
        self->str[rep[0] = len] = '\0';
        rep[1] = _cstr_cap(cap);
    }
}

STC_API void
cstr_resize(cstr_t* self, size_t len, char fill) {
    size_t n = cstr_size(*self);
    cstr_reserve(self, len);
    if (len > n) memset(self->str + n, fill, len - n);
    if (len | n) self->str[_cstr_size(*self) = len] = '\0';
}

STC_API cstr_t
cstr_n(const char* str, size_t len) {
    if (len == 0) return cstr_INIT;
    size_t *rep = (size_t *) c_malloc(_cstr_mem(len));
    cstr_t s = {(char *) memcpy(rep + 2, str, len)};
    s.str[rep[0] = len] = '\0';
    rep[1] = _cstr_cap(len);
    return s;
}

STC_API cstr_t
cstr_from(const char* fmt, ...) {
    #if defined(__clang__)
    #  pragma clang diagnostic push
    #  pragma clang diagnostic ignored "-Wdeprecated-declarations"
    #elif defined(_MSC_VER)
    #  pragma warning(push)
    #  pragma warning(disable: 4996)
    #endif
    cstr_t tmp = cstr_INIT;
    va_list args, args2;
    va_start(args, fmt);
    va_copy(args2, args);
    int len = vsnprintf(NULL, (size_t)0, fmt, args);
    va_end(args);
    if (len > 0) {
        tmp = cstr_with_capacity(len);
        vsprintf(tmp.str, fmt, args2);
        _cstr_size(tmp) = len;
    }
    va_end(args2);
    return tmp;
    #if defined(__clang__)
    #  pragma clang diagnostic pop
    #elif defined(_MSC_VER)
    #  pragma warning(pop)
    #endif
}

STC_API cstr_t*
cstr_assign_n(cstr_t* self, const char* str, size_t len) {
    if (len || cstr_capacity(*self)) {
        cstr_reserve(self, len);
        memmove(self->str, str, len);
        self->str[_cstr_size(*self) = len] = '\0';
    }
    return self;
}

STC_API cstr_t*
cstr_append_n(cstr_t* self, const char* str, size_t len) {
    if (len) {
        size_t oldlen = cstr_size(*self), newlen = oldlen + len;
        if (newlen > cstr_capacity(*self)) {
            /* handle self append */
            size_t off = (size_t) (str - self->str);
            cstr_reserve(self, newlen * 3 / 2);
            if (off <= oldlen) str = self->str + off;
        }
        memcpy(&self->str[oldlen], str, len);
        self->str[_cstr_size(*self) = newlen] = '\0';
    }
    return self;
}

STC_INLINE void _cstr_internal_move(cstr_t* self, size_t pos1, size_t pos2) {
    if (pos1 == pos2)
        return;
    size_t len = cstr_size(*self), newlen = len + pos2 - pos1;
    if (newlen > cstr_capacity(*self))
        cstr_reserve(self, newlen * 3 / 2);
    memmove(&self->str[pos2], &self->str[pos1], len - pos1);
    self->str[_cstr_size(*self) = newlen] = '\0';
}

STC_API void
cstr_replace_n(cstr_t* self, size_t pos, size_t len, const char* str, size_t n) {
    char buf[_c_max_buffer];
    char* xstr = (char *) memcpy(n > _c_max_buffer ? c_malloc(n) : buf, str, n);
    _cstr_internal_move(self, pos + len, pos + n);
    memcpy(&self->str[pos], xstr, n);
    if (n > _c_max_buffer) c_free(xstr);
}

STC_API void
cstr_erase(cstr_t* self, size_t pos, size_t n) {
    size_t len = cstr_size(*self);
    if (len) {
        memmove(&self->str[pos], &self->str[pos + n], len - (pos + n));
        self->str[_cstr_size(*self) -= n] = '\0';
    }
}

STC_API char*
c_strnstr(const char* x, const char* needle, size_t n) {
    ptrdiff_t sum = 0;
    const char *y = x, *p = needle, *q = needle + n;
    while (*y && *p && p != q) {
        sum += *y++ - *p++;
    }
    while (*y) {
        if (sum == 0 && strncmp(x, needle, n) == 0)
            return (char *) x;
        sum += *y++ - *x++;
    }
    return NULL;
}

#endif

#endif
