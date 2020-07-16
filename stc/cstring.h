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
#ifndef CSTRING__H__
#define CSTRING__H__

#include <stdlib.h> /* alloca, malloc */
#include <malloc.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h> /* vsnprintf */

#include "cdefs.h"

typedef struct CString {
    char* str;
} CString;


static size_t _cstring_nullrep[] = {0, 0, 0};
#define       _cstring_rep(self)   (((size_t *) (self)->str) - 2)
#define       _cstring_size(s)     ((size_t *) (s).str)[-2]
#define       _cstring_mem(cap)    (sizeof(size_t) * (3 + (cap)/sizeof(size_t)))

#define cstring_size(s)      ((const size_t *) (s).str)[-2]
#define cstring_capacity(s)  ((const size_t *) (s).str)[-1]
#define cstring_npos         ((size_t) (-1))


static const CString cstring_init = {(char* ) &_cstring_nullrep[2]};

static inline void
cstring_reserve(CString* self, size_t cap) {
    size_t len = cstring_size(*self), oldcap = cstring_capacity(*self);
    if (cap > oldcap) {
        size_t* rep = (size_t *) realloc(oldcap ? _cstring_rep(self) : NULL, _cstring_mem(cap));
        self->str = (char *) (rep + 2);
        self->str[rep[0] = len] = '\0';
        rep[1] = cap;
    }
}

static inline void
cstring_resize(CString* self, size_t len, char fill) {
    size_t n = cstring_size(*self);
    cstring_reserve(self, len);
    if (len > n) memset(self->str + n, fill, len - n);
    self->str[_cstring_size(*self) = len] = '\0';
}

static inline void
cstring_destroy(CString* self) {
    if (cstring_capacity(*self)) {
        free(_cstring_rep(self));
    }
}

static inline CString
cstring_makeFilled(size_t len, char fill) {
    CString s = cstring_init;
    if (len) cstring_resize(&s, len, fill);
    return s;
}

static inline CString
cstring_makeReserved(size_t cap) {
    if (cap == 0) return cstring_init;
    size_t *rep = (size_t *) malloc(_cstring_mem(cap));
    CString s = {(char *) (rep + 2)};
    rep[0] = 0, rep[1] = cap, s.str[0] = '\0';
    return s;
}

static inline CString
cstring_makeN(const char* str, size_t len) {
    if (len == 0) return cstring_init;
    size_t *rep = (size_t *) malloc(_cstring_mem(len));
    CString s = {(char *) (rep + 2)};
    memcpy(s.str, str, len);
    s.str[rep[0] = rep[1] = len] = '\0';
    return s;
}

static inline CString
cstring_make(const char* str) {
    return cstring_makeN(str, strlen(str));
}

static inline CString
cstring_makeCopy(CString s) {
    return cstring_makeN(s.str, cstring_size(s));
}

static inline CString
cstring_makeFmt(const char* fmt, ...) {
    CString tmp = cstring_init;
    int len;
    va_list args;
    va_start(args, fmt);
    len = vsnprintf(NULL, (size_t)0, fmt, args);
    if (len > 0) {
        tmp = cstring_makeReserved(len);
        vsprintf(tmp.str, fmt, args);
        _cstring_size(tmp) = len;
    }
    va_end(args);
    return tmp;
}


static inline void
cstring_clear(CString* self) {
    cstring_destroy(self);
    *self = cstring_init;
}

static inline CString*
cstring_assignN(CString* self, const char* str, size_t len) {
    if (len || cstring_capacity(*self)) {
        cstring_reserve(self, len);
        memmove(self->str, str, len);
        self->str[_cstring_size(*self) = len] = '\0';
    }
    return self;
}

static inline CString*
cstring_assign(CString* self, const char* str) {
    return cstring_assignN(self, str, strlen(str));
}

static inline CString*
cstring_copy(CString* self, CString s) {
    return cstring_assignN(self, s.str, cstring_size(s));
}

static inline CString*
cstring_take(CString* self, CString s) {
    if (self->str != s.str && cstring_capacity(*self))
        free(_cstring_rep(self));
    self->str = s.str;
    return self;
}

static inline CString
cstring_move(CString* self) {
    CString tmp = *self;
    *self = cstring_init;
    return tmp;
}

static inline CString*
cstring_appendN(CString* self, const char* str, size_t len) {
    if (len) {
        size_t oldlen = cstring_size(*self), newlen = oldlen + len;
        if (newlen > cstring_capacity(*self))
            cstring_reserve(self, newlen * 5 / 3);
        memmove(&self->str[oldlen], str, len);
        self->str[_cstring_size(*self) = newlen] = '\0';
    }
    return self;
}
static inline CString*
cstring_append(CString* self, const char* str) {
    return cstring_appendN(self, str, strlen(str));
}
static inline CString*
cstring_appendC(CString* self, char ch) {
    return cstring_appendN(self, &ch, 1);
}
static inline CString*
cstring_appendS(CString* self, CString s) {
    return cstring_appendN(self, s.str, cstring_size(s));
}

static inline void _cstring_internalMove(CString* self, size_t pos1, size_t pos2) {
    if (pos1 == pos2)
        return;
    size_t len = cstring_size(*self), newlen = len + pos2 - pos1;
    if (newlen > cstring_capacity(*self))
        cstring_reserve(self, newlen * 5 / 3);
    memmove(&self->str[pos2], &self->str[pos1], len - pos1);
    self->str[_cstring_size(*self) = newlen] = '\0';
}

static inline void
cstring_insertN(CString* self, size_t pos, const char* str, size_t n) {
    char* xstr = (char *) memcpy(n > c_max_alloca ? malloc(n) : alloca(n), str, n);
    _cstring_internalMove(self, pos, pos + n);
    memcpy(&self->str[pos], xstr, n);
    if (n > c_max_alloca) free(xstr); 
}

static inline void
cstring_insert(CString* self, size_t pos, const char* str) {
    cstring_insertN(self, pos, str, strlen(str));
}

static inline void
cstring_erase(CString* self, size_t pos, size_t n) {
    size_t len = cstring_size(*self);
    if (len) {
        memmove(&self->str[pos], &self->str[pos + n], len - (pos + n));
        self->str[_cstring_size(*self) -= n] = '\0';
    }
}

static inline size_t cstring_findN(CString s, size_t pos, const char* needle, size_t n);

static inline size_t
cstring_replaceN(CString* self, size_t pos, const char* str1, size_t n1, const char* str2, size_t n2) {
    size_t pos2 = cstring_findN(*self, pos, str1, n1);
    if (pos2 == cstring_npos) return cstring_npos;
    char* xstr2 = (char *) memcpy(n2 > c_max_alloca ? malloc(n2) : alloca(n2), str2, n2);
    _cstring_internalMove(self, pos2 + n1, pos2 + n2);
    memcpy(&self->str[pos2], xstr2, n2);
    if (n2 > c_max_alloca) free(xstr2);
    return pos2;
}

static inline size_t
cstring_replace(CString* self, size_t pos, const char* str1, const char* str2) {
    return cstring_replaceN(self, pos, str1, strlen(str1), str2, strlen(str2));
}


static inline char
cstring_back(CString s) {
    return s.str[cstring_size(s) - 1];
}

static inline CString*
cstring_push(CString* self, char value) {
    return cstring_appendN(self, &value, 1);
}


static inline void
cstring_pop(CString* self) {
    --_cstring_size(*self);
}

/* readonly */

static inline bool
cstring_empty(CString s) {
    return cstring_size(s) == 0;
}

static inline bool
cstring_equals(CString s1, const char* str) {
    return strcmp(s1.str, str) == 0;
}
static inline bool
cstring_equalsS(CString s1, CString s2) {
    return strcmp(s1.str, s2.str) == 0;
}

static inline int
cstring_compare(const void* s1, const void* s2) {
    return strcmp(((const CString*)s1)->str, ((const CString*)s2)->str);
}

static inline char*
cstring_strnstr(CString s, size_t pos, const char* needle, size_t n) {
    char *x = s.str + pos, /* haystack */
         *z = s.str + cstring_size(s) - n + 1;
    if (x >= z)
        return NULL;
    ptrdiff_t sum = 0;
    const char *y = x, *p = needle, *q = needle + n;
    while (p != q)
        sum += *y++ - *p++;
    while (x != z) {
        if (sum == 0 && memcmp(x, needle, n) == 0)
            return x;
        sum += *y++ - *x++;
    }
    return NULL;
}

static inline size_t
cstring_findN(CString s, size_t pos, const char* needle, size_t n) {
    char* res = cstring_strnstr(s, pos, needle, n);
    return res ? res - s.str : cstring_npos;
}

static inline size_t
cstring_find(CString s, size_t pos, const char* needle) {
    char* res = strstr(s.str + pos, needle);
    return res ? res - s.str : cstring_npos;
}


/* CVector / CMap API functions: */

#define                cstring_getRaw(x) ((x)->str)
#define                cstring_compareRaw(x, y) strcmp(*(x), *(y))
#define                cstring_equalsRaw(x, y) (strcmp(*(x), *(y)) == 0)
static inline uint32_t cstring_hashRaw(const char* const* sPtr, size_t ignored) {
    return c_defaultHash(*sPtr, strlen(*sPtr));
}

#endif
