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
#ifndef CSTR__H__
#define CSTR__H__

#include "ccommon.h"
#include <stdlib.h> /* malloc */
#include <string.h>
#include <stdarg.h>
#include <stdio.h> /* vsnprintf */
#include <ctype.h>

typedef struct cstr { char* str; } cstr_t, cstr;
typedef struct { char *ref; } cstr_iter_t;
typedef char cstr_value_t;
#define cstr_npos ((size_t) (-1))

STC_API cstr_t  cstr_from_n(const char* str, size_t len);
STC_API cstr_t  cstr_from_fmt(const char* fmt, ...);
STC_API void    cstr_fmt(cstr_t* self, const char* fmt, ...);
STC_API size_t  cstr_reserve(cstr_t* self, size_t cap);
STC_API void    cstr_resize(cstr_t* self, size_t len, char fill);
STC_API cstr_t* cstr_assign_n(cstr_t* self, const char* str, size_t len);
STC_API cstr_t* cstr_append_n(cstr_t* self, const char* str, size_t len);
STC_API void    cstr_replace_n(cstr_t* self, size_t pos, size_t len, const char* str, size_t n);
STC_API void    cstr_erase_n(cstr_t* self, size_t pos, size_t n);
STC_API bool    cstr_getdelim(cstr_t *self, int delim, FILE *stream);
STC_API size_t  cstr_find(cstr_t s, const char* needle);
STC_API size_t  cstr_find_n(cstr_t s, const char* needle, size_t pos, size_t nlen);
STC_API size_t  cstr_ifind_n(cstr_t s, const char* needle, size_t pos, size_t nlen);

STC_API char*   c_strcopy(const char* src, char* dst, const char* dst_end, int termin);
STC_API int     c_strncasecmp(const char* s1, const char* s2, size_t n);
STC_API char*   c_strnfind(const char* s, const char* needle, size_t nmax);
STC_API char*   c_istrnfind(const char* s, const char* needle, size_t nmax);
STC_INLINE
uint64_t        c_strhash(const char* s) {return c_default_hash(s, strlen(s));}

struct cstr_rep { size_t size, cap; char str[sizeof(size_t)]; };
#define _cstr_rep(self) c_container_of((self)->str, struct cstr_rep, str)
/* optimal memory: based on malloc_usable_size() sequence: 24, 40, 56, ... */
#define _cstr_opt_mem(cap)  ((((offsetof(struct cstr_rep, str) + (cap) + 8)>>4)<<4) + 8)
/* optimal string capacity: 7, 23, 39, ... */
#define _cstr_opt_cap(cap)  (_cstr_opt_mem(cap) - offsetof(struct cstr_rep, str) - 1)

STC_LIBRARY_ONLY( extern const cstr_t cstr_inits; )
STC_STATIC_ONLY( static struct cstr_rep _cstr_nullrep = {0, 0, {0}};
                 static const cstr_t cstr_inits = {_cstr_nullrep.str}; )

STC_INLINE cstr_t
cstr_init() { return cstr_inits; }

STC_INLINE void
cstr_del(cstr_t* self) {
    if (_cstr_rep(self)->cap)
        c_free(_cstr_rep(self));
}

STC_INLINE cstr_t
cstr_with_capacity(size_t cap) {
    cstr_t s = cstr_inits;
    cstr_reserve(&s, cap);
    return s;
}
STC_INLINE cstr_t
cstr_with_size(size_t len, char fill) {
    cstr_t s = cstr_inits;
    cstr_resize(&s, len, fill);
    return s;
}
STC_INLINE cstr_t
cstr_from(const char* str) {
    return cstr_from_n(str, strlen(str));
}
STC_INLINE cstr_t
cstr_clone(cstr_t s) {
    return cstr_from_n(s.str, _cstr_rep(&s)->size);
}
STC_INLINE void
cstr_clear(cstr_t* self) {
    self->str[_cstr_rep(self)->size = 0] = '\0';
}

STC_INLINE char*
cstr_front(cstr_t* self) {return self->str;}
STC_INLINE char*
cstr_back(cstr_t* self) {return self->str + _cstr_rep(self)->size - 1;}

STC_INLINE cstr_iter_t
cstr_begin(cstr_t* self) {cstr_iter_t it = {self->str}; return it;}
STC_INLINE cstr_iter_t
cstr_end(cstr_t* self) {
    cstr_iter_t it = {self->str + _cstr_rep(self)->size}; return it;
}
STC_INLINE void cstr_next(cstr_iter_t* it) { ++it->ref; }
STC_INLINE char* cstr_itval(cstr_iter_t it) {return it.ref;}

STC_INLINE cstr_t*
cstr_assign(cstr_t* self, const char* str) {
    return cstr_assign_n(self, str, strlen(str));
}

STC_INLINE cstr_t*
cstr_take(cstr_t* self, cstr_t s) {
    if (self->str != s.str && _cstr_rep(self)->cap)
        c_free(_cstr_rep(self));
    self->str = s.str;
    return self;
}
STC_INLINE cstr_t
cstr_move(cstr_t* self) {
    cstr_t tmp = *self;
    *self = cstr_inits;
    return tmp;
}

STC_INLINE cstr_t*
cstr_append(cstr_t* self, const char* str) {
    return cstr_append_n(self, str, strlen(str));
}
STC_INLINE void
cstr_push_back(cstr_t* self, char value) {
    cstr_append_n(self, &value, 1);
}
STC_INLINE void
cstr_pop_back(cstr_t* self) {
    self->str[ --_cstr_rep(self)->size ] = '\0';
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
STC_INLINE void
cstr_erase(cstr_t* self, size_t pos) {cstr_erase_n(self, pos, 1);}

STC_INLINE bool
cstr_getline(cstr_t *self, FILE *stream) {
    return cstr_getdelim(self, '\n', stream);
}

/* readonly */

STC_INLINE size_t 
cstr_size(cstr_t s) {return _cstr_rep(&s)->size;}
STC_INLINE size_t
cstr_capacity(cstr_t s) {return _cstr_rep(&s)->cap;}
STC_INLINE size_t
cstr_empty(cstr_t s) {return _cstr_rep(&s)->size == 0;}
STC_INLINE size_t
cstr_length(cstr_t s) { return _cstr_rep(&s)->size; }

STC_INLINE bool
cstr_equals(cstr_t s1, const char* str) {
    return strcmp(s1.str, str) == 0;
}
STC_INLINE bool
cstr_iequals(cstr_t s1, const char* str) {
    return c_strncasecmp(s1.str, str, cstr_npos) == 0;
}
STC_INLINE bool
cstr_equals_s(cstr_t s1, cstr_t s2) {
    return strcmp(s1.str, s2.str) == 0;
}

STC_INLINE bool
cstr_contains(cstr_t s, const char* needle) {
    return strstr(s.str, needle) != NULL;
}
STC_INLINE bool
cstr_icontains(cstr_t s, const char* needle) {
    return c_istrnfind(s.str, needle, cstr_npos) != NULL;
}

STC_INLINE bool
cstr_begins_with(cstr_t s, const char* needle) {
    return strncmp(s.str, needle, strlen(needle)) == 0;
}
STC_INLINE bool
cstr_ibegins_with(cstr_t s, const char* needle) {
    return c_strncasecmp(s.str, needle, strlen(needle)) == 0;
}

STC_INLINE bool
cstr_ends_with(cstr_t s, const char* needle) {
    size_t n = strlen(needle), sz = _cstr_rep(&s)->size;
    return n <= sz ? memcmp(s.str + sz - n, needle, n) == 0 : false;
}
STC_INLINE bool
cstr_iends_with(cstr_t s, const char* needle) {
    size_t n = strlen(needle), sz = _cstr_rep(&s)->size;
    return n <= sz ? c_strncasecmp(s.str + sz - n, needle, n) == 0 : false;
}

/* cvec/cmap adaption functions: */
#define  cstr_to_raw(x)          ((x)->str)
#define  cstr_compare_raw(x, y)  strcmp(*(x), *(y))
#define  cstr_equals_raw(x, y)   (strcmp(*(x), *(y)) == 0)
#define  cstr_hash_raw(p, none)  c_default_hash(*(p), strlen(*(p)))
#define  cstr_compare_ref(x, y)  strcmp((x)->str, (y)->str)
#define  cstr_equals_ref(x, y)   (strcmp((x)->str, (y)->str) == 0)

/* -------------------------- IMPLEMENTATION ------------------------- */

#if !defined(STC_HEADER) || defined(STC_IMPLEMENTATION)

STC_LIBRARY_ONLY( static struct cstr_rep _cstr_nullrep = {0, 0, {0}};
                  const cstr_t cstr_inits = {_cstr_nullrep.str}; )

STC_DEF size_t
cstr_reserve(cstr_t* self, size_t cap) {
    struct cstr_rep* rep = _cstr_rep(self); 
    size_t oldcap = rep->cap;
    if (cap > oldcap) {
        rep = (struct cstr_rep*) c_realloc(oldcap ? rep : NULL, _cstr_opt_mem(cap));
        self->str = rep->str;
        if (oldcap == 0) self->str[rep->size = 0] = '\0';
        return (rep->cap = _cstr_opt_cap(cap));
    }
    return oldcap;
}

STC_DEF void
cstr_resize(cstr_t* self, size_t len, char fill) {
    size_t n =  _cstr_rep(self)->size;
    cstr_reserve(self, len);
    if (len > n) memset(self->str + n, fill, len - n);
    if (len | n) self->str[_cstr_rep(self)->size = len] = '\0';
}

STC_DEF cstr_t
cstr_from_n(const char* str, size_t len) {
    if (len == 0) return cstr_inits;
    struct cstr_rep* rep = (struct cstr_rep*) c_malloc(_cstr_opt_mem(len));
    cstr_t s = {(char *) memcpy(rep->str, str, len)};
    s.str[rep->size = len] = '\0';
    rep->cap = _cstr_opt_cap(len);
    return s;
}

#if defined(__clang__)
#  pragma clang diagnostic push
#  pragma clang diagnostic ignored "-Wdeprecated-declarations"
#elif defined(_MSC_VER)
#  pragma warning(push)
#  pragma warning(disable: 4996)
#endif
STC_DEF void
cstr_vfmt(cstr_t* self, const char* fmt, va_list args) {
    va_list args2;
    va_copy(args2, args);
    int len = vsnprintf(NULL, (size_t)0, fmt, args);
    cstr_reserve(self, len);
    vsprintf(self->str, fmt, args2);
    _cstr_rep(self)->size = len;
    va_end(args2);
}

STC_DEF void
cstr_fmt(cstr_t* self, const char* fmt, ...) {
    va_list args; va_start(args, fmt);
    cstr_vfmt(self, fmt, args);
    va_end(args);
}

STC_DEF cstr_t
cstr_from_fmt(const char* fmt, ...) {
    cstr_t ret = cstr_inits;
    va_list args; va_start(args, fmt);
    cstr_vfmt(&ret, fmt, args);
    va_end(args);
    return ret;
}
#if defined(__clang__)
#  pragma clang diagnostic pop
#elif defined(_MSC_VER)
#  pragma warning(pop)
#endif

STC_DEF cstr_t*
cstr_assign_n(cstr_t* self, const char* str, size_t len) {
    if (len || _cstr_rep(self)->cap) {
        cstr_reserve(self, len);
        memmove(self->str, str, len);
        self->str[_cstr_rep(self)->size = len] = '\0';
    }
    return self;
}

STC_DEF cstr_t*
cstr_append_n(cstr_t* self, const char* str, size_t len) {
    if (len) {
        size_t oldlen = _cstr_rep(self)->size, newlen = oldlen + len;
        if (newlen > _cstr_rep(self)->cap) {
            /* handle self append */
            size_t off = (size_t) (str - self->str);
            cstr_reserve(self, newlen*3/2);
            if (off <= oldlen) str = self->str + off;
        }
        memcpy(&self->str[oldlen], str, len);
        self->str[_cstr_rep(self)->size = newlen] = '\0';
    }
    return self;
}

STC_INLINE void _cstr_internal_move(cstr_t* self, size_t pos1, size_t pos2) {
    if (pos1 == pos2)
        return;
    size_t len = _cstr_rep(self)->size, newlen = len + pos2 - pos1;
    if (newlen > _cstr_rep(self)->cap)
        cstr_reserve(self, newlen*3/2);
    memmove(&self->str[pos2], &self->str[pos1], len - pos1);
    self->str[_cstr_rep(self)->size = newlen] = '\0';
}

STC_DEF void
cstr_replace_n(cstr_t* self, size_t pos, size_t len, const char* str, size_t n) {
    c_withbuffer (xstr, char, n) {
        memcpy(xstr, str, n);
        _cstr_internal_move(self, pos + len, pos + n);
        memcpy(&self->str[pos], xstr, n);
    }
}

STC_DEF void
cstr_erase_n(cstr_t* self, size_t pos, size_t n) {
    size_t len = _cstr_rep(self)->size;
    if (len) {
        memmove(&self->str[pos], &self->str[pos + n], len - (pos + n));
        self->str[_cstr_rep(self)->size -= n] = '\0';
    }
}

STC_DEF bool
cstr_getdelim(cstr_t *self, int delim, FILE *fp) {
    size_t pos = 0, cap = _cstr_rep(self)->cap;
    int c = fgetc(fp);
    if (c == EOF)
        return false;
    for (;;) {
        if (pos == cap)
            cap = cstr_reserve(self, cap*3/2 + 34);
        if (c == delim || c == EOF) {
            self->str[_cstr_rep(self)->size = pos] = '\0';
            return true;
        }
        self->str[pos++] = (char) c;
        c = fgetc(fp);
    }
}

STC_DEF size_t
cstr_find(cstr_t s, const char* needle) {
    char* res = strstr(s.str, needle);
    return res ? res - s.str : cstr_npos;
}
STC_DEF size_t
cstr_find_n(cstr_t s, const char* needle, size_t pos, size_t nlen) {
    if (pos > _cstr_rep(&s)->size) return cstr_npos;
    char* res = c_strnfind(s.str + pos, needle, nlen);
    return res ? res - s.str : cstr_npos;
}
STC_DEF size_t
cstr_ifind_n(cstr_t s, const char* needle, size_t pos, size_t nlen) {
    if (pos > _cstr_rep(&s)->size) return cstr_npos;
    char* res = c_istrnfind(s.str + pos, needle, nlen);
    return res ? res - s.str : cstr_npos;
}

/* http://graphics.stanford.edu/~seander/bithacks.html#ZeroInWord */
STC_DEF char*
c_strcopy(const char* s, char* d, const char* d_end, int c) {
    enum {w = sizeof(uintptr_t)};
    #define _sc_z (~(uintptr_t)0/255)
    for (uintptr_t x; d + w <= d_end; s += w, d += w) {
        memcpy(&x, s, w); /* check if x contains c: */
        if (((x - _sc_z) & ~x & _sc_z<<7) ^ (_sc_z*(uint8_t) c)) break;
        memcpy(d, &x, w);
    }
    while (d < d_end) if ((*d++ = *s++) == (uint8_t) c) return d - 1;
    return NULL;
}

STC_DEF int
c_strncasecmp(const char* s1, const char* s2, size_t n) {
    int ret = 0;
    while (n-- && (ret = tolower(*s1++) - tolower(*s2)) == 0 && *s2++) ;
    return ret;
}

STC_DEF char*
c_strnfind(const char* s, const char* needle, size_t nmax) {
    ptrdiff_t sum = 0;
    const char *t = s, *p = needle;
    while (*p && nmax--) {
        if (!*t) return NULL;
        sum += *t++ - *p++;
    }
    nmax = t - s;
    for (;;) {
        if (sum == 0 && memcmp(s, needle, nmax) == 0)
            return (char *) s;
        if (!*t) return NULL;
        sum += *t++ - *s++;
    }
}

STC_DEF char*
c_istrnfind(const char* s, const char* needle, size_t nmax) {
    ptrdiff_t sum = 0;
    const char *t = s, *p = needle;
    while (*p && nmax--) {
        if (!*t) return NULL;
        sum += tolower(*t++) - tolower(*p++);
    }
    nmax = t - s;
    for (;;) {
        if (sum == 0 && c_strncasecmp(s, needle, nmax) == 0)
            return (char *) s;
        if (!*t) return NULL;
        sum += tolower(*t++) - tolower(*s++);
    }
}

#endif

#endif