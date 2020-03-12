// MIT License
//
// Copyright (c) 2020 Tyge Løvset, NORCE, www.norceresearch.no
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#ifndef CSTRING__H__
#define CSTRING__H__

#include <malloc.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>

#include "cdefs.h"

typedef struct CString {
    char* str;
} CString;


static size_t _cstring_null_rep[] = {0, 0, 0};
#define       _cstring_rep(cs)      (((size_t *) (cs).str) - 2)

#define cstring_initializer   {(char* ) (_cstring_null_rep + 2)}
#define cstring_size(cs)      ((size_t) _cstring_rep(cs)[0])
#define cstring_capacity(cs)  ((size_t) _cstring_rep(cs)[1])
#define cstring_npos          c_npos


static inline void cstring_reserve(CString* self, size_t cap) {
    size_t len = cstring_size(*self), oldcap = cstring_capacity(*self);
    if (cap > oldcap) {
        size_t* rep = (size_t *) realloc(oldcap ? _cstring_rep(*self) : NULL, sizeof(size_t) * 2 + cap + 1);
        self->str = (char* ) (rep + 2);
        self->str[ rep[0] = len ] = '\0';
        rep[1] = cap;
    }
}

static inline void cstring_destroy(CString* self) {
    if (cstring_capacity(*self)) {
        free(_cstring_rep(*self));
    }
}

static inline CString cstring_init(void) {
    CString cs = cstring_initializer;
    return cs;
}

static inline CString cstring_makeN(const char* str, size_t len) {
    CString cs = cstring_initializer;
    if (len) {
        cstring_reserve(&cs, len);
        memcpy(cs.str, str, len);
        cs.str[ _cstring_rep(cs)[0] = len ] = '\0';
    }
    return cs;
}

static inline CString cstring_make(const char* str) {
    return cstring_makeN(str, strlen(str));
}

static inline CString cstring_makeCopy(CString cs) {
    return cstring_makeN(cs.str, cstring_size(cs));
}

static inline void cstring_clear(CString* self) {
    CString cs = cstring_initializer;
    cstring_destroy(self);
    *self = cs;
}

static inline CString* cstring_assignN(CString* self, const char* str, size_t len) {
    if (len) {
        cstring_reserve(self, len);
        memmove(self->str, str, len);
        self->str[_cstring_rep(*self)[0] = len] = '\0';
    }
    return self;
}

static inline CString* cstring_assign(CString* self, const char* str) {
    return cstring_assignN(self, str, strlen(str));
}

static inline CString* cstring_copy(CString* self, CString cs2) {
    return cstring_assignN(self, cs2.str, cstring_size(cs2));
}


static inline CString* cstring_appendN(CString* self, const char* str, size_t len) {
    if (len) {
        size_t oldlen = cstring_size(*self), newlen = oldlen + len;
        if (newlen > cstring_capacity(*self))
            cstring_reserve(self, newlen * 5 / 3);
        memmove(&self->str[oldlen], str, len);
        self->str[_cstring_rep(*self)[0] = newlen] = '\0';
    }
    return self;
}

static inline CString* cstring_append(CString* self, const char* str) {
    return cstring_appendN(self, str, strlen(str));
}
static inline CString* cstring_appendS(CString* self, CString cs2) {
    return cstring_appendN(self, cs2.str, cstring_size(cs2));
}


static inline void _cstring_internalMove(CString* self, size_t pos1, size_t pos2) {
    if (pos1 == pos2)
        return;
    size_t len = cstring_size(*self), newlen = len + pos2 - pos1;
    if (newlen > cstring_capacity(*self))
        cstring_reserve(self, newlen * 5 / 3);
    memmove(&self->str[pos2], &self->str[pos1], len - pos1);
    self->str[_cstring_rep(*self)[0] = newlen] = '\0';
}

static inline void cstring_insertN(CString* self, size_t pos, const char* str, size_t n) {
    char* xstr = (char *) memcpy(n > c_max_alloca ? malloc(n) : alloca(n), str, n);
    _cstring_internalMove(self, pos, pos + n);
    memcpy(&self->str[pos], xstr, n);
    if (n > c_max_alloca) free(xstr); 
}

static inline void cstring_insert(CString* self, size_t pos, const char* str) {
    cstring_insertN(self, pos, str, strlen(str));
}

static inline void cstring_erase(CString* self, size_t pos, size_t n) {
    size_t len = cstring_size(*self);
    if (len) {
        memmove(&self->str[pos], &self->str[pos + n], len - (pos + n));
        self->str[_cstring_rep(*self)[0] -= n] = '\0';
    }
}

static inline size_t cstring_findN(CString cs, size_t pos, const char* needle, size_t n);

static inline size_t cstring_replaceN(CString* self, size_t pos, const char* s1, size_t n1, const char* s2, size_t n2) {
    size_t pos2 = cstring_findN(*self, pos, s1, n1);
    if (pos2 == cstring_npos) return cstring_npos;
    char* xs2 = (char *) memcpy(n2 > c_max_alloca ? malloc(n2) : alloca(n2), s2, n2);
    _cstring_internalMove(self, pos2 + n1, pos2 + n2);
    memcpy(&self->str[pos2], xs2, n2);
    if (n2 > c_max_alloca) free(xs2);
    return pos2;
}

static inline size_t cstring_replace(CString* self, size_t pos, const char* s1, const char* s2) {
    return cstring_replaceN(self, pos, s1, strlen(s1), s2, strlen(s2));
}


static inline char cstring_back(CString cs) {
    return cs.str[cstring_size(cs) - 1];
}

static inline CString* cstring_push(CString* self, char value) {
    return cstring_appendN(self, &value, 1);
}


static inline void cstring_pop(CString* self) {
    --_cstring_rep(*self)[0];
}

/* readonly */

static inline bool cstring_empty(CString cs) {
    return cstring_size(cs) == 0;
}

static inline bool cstring_equals(CString cs1, const char* str) {
    return strcmp(cs1.str, str) == 0;
}
static inline bool cstring_equalsS(CString cs1, CString cs2) {
    return strcmp(cs1.str, cs2.str) == 0;
}

static inline char* cstring_strnstr(CString cs, size_t pos, const char* needle, size_t n) {
    char *x = cs.str + pos, // haystack
         *z = cs.str + cstring_size(cs) - n + 1;
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

static inline size_t cstring_findN(CString cs, size_t pos, const char* needle, size_t n) {
    char* res = cstring_strnstr(cs, pos, needle, n);
    return res ? res - cs.str : cstring_npos;
}

static inline size_t cstring_find(CString cs, size_t pos, const char* needle) {
    char* res = strstr(cs.str + pos, needle);
    return res ? res - cs.str : cstring_npos;
}

static inline char* cstring_splitFirst(const char* delimiters, CString cs) {
    return strtok(cs.str, delimiters);
}

static inline char* cstring_splitNext(const char* delimiters) {
    return strtok(NULL, delimiters);
}


// CVector / CMap API functions:

#define                cstring_getRaw(x) ((x).str)
static inline uint32_t cstring_hashRaw(const char** str, size_t sz_ignored) { return c_murmurHash(*str, strlen(*str)); }
static inline int      cstring_compareRaw(CString* self, const char** str, size_t sz_ignored) { return strcmp(self->str, *str); }


#endif
