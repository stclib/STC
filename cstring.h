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
#include <assert.h>
#include <string.h>
#include <stdint.h>

#include "cdef.h"

typedef uint32_t cstring_size_t;
typedef struct CString {
    char* str;
} CString;


static cstring_size_t _cstring_null_rep[] = {0, 0, 0};
#define               _cstring_rep(cs)      (((cstring_size_t *) (cs).str) - 2)

#define cstring_initializer   {(char* ) (_cstring_null_rep + 2)}
#define cstring_size(cs)      ((cstring_size_t) _cstring_rep(cs)[0])
#define cstring_capacity(cs)  ((cstring_size_t) _cstring_rep(cs)[1])


static inline void cstring_reserve(CString* self, cstring_size_t cap) {
    cstring_size_t len = cstring_size(*self), oldcap = cstring_capacity(*self);
    if (cap > oldcap) {
        cstring_size_t* rep = (cstring_size_t *) realloc(oldcap ? _cstring_rep(*self) : NULL, sizeof(cstring_size_t) * 2 + cap + 1);
        rep[0] = len;
        rep[1] = cap;
        self->str = (char* ) (rep + 2);
        self->str[len] = '\0';
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

static inline CString cstring_makeN(const char* str, cstring_size_t len) {
    CString cs = cstring_initializer;
    if (len) {
        cstring_reserve(&cs, len);
        memcpy(cs.str, str, len);
        _cstring_rep(cs)[0] = len;
        cs.str[len] = '\0';
    }
    return cs;
}

static inline CString cstring_make(const char* str) {
    return cstring_makeN(str, strlen(str));
}

static inline CString cstring_clone(CString cs) {
    return cstring_makeN(cs.str, cstring_size(cs));
}

static inline void cstring_clear(CString* self) {
    CString cs = cstring_initializer;
    cstring_destroy(self);
    *self = cs;
}

static inline CString* cstring_assignN(CString* self, const char* str, cstring_size_t len) {
    if (len) {
        cstring_reserve(self, len);
        memmove(self->str, str, len);
        self->str[len] = '\0';
        _cstring_rep(*self)[0] = len;
    }
    return self;
}

static inline CString* cstring_assign(CString* self, const char* str) {
    return cstring_assignN(self, str, strlen(str));
}

static inline CString* cstring_assignCopy(CString* self, CString cs2) {
    return cstring_assignN(self, cs2.str, cstring_size(cs2));
}


static inline CString* cstring_appendN(CString* self, const char* str, cstring_size_t len) {
    if (len) {
        cstring_size_t oldlen = cstring_size(*self), newlen = oldlen + len;
        if (newlen > cstring_capacity(*self))
            cstring_reserve(self, (newlen * 5) / 3);
        memmove(&self->str[oldlen], str, len);
        self->str[newlen] = '\0';
        _cstring_rep(*self)[0] = newlen;
    }
    return self;
}

static inline CString* cstring_append(CString* self, const char* str) {
    return cstring_appendN(self, str, strlen(str));
}
static inline CString* cstring_appendCopy(CString* self, CString cs2) {
    return cstring_appendN(self, cs2.str, cstring_size(cs2));
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

static inline char* cstring_find(CString cs, const char* needle) {
    return strstr(cs.str, needle);
}

static inline char* cstring_splitFirst(const char* delimiters, CString cs) {
    return strtok(cs.str, delimiters);
}

static inline char* cstring_splitNext(const char* delimiters) {
    return strtok(NULL, delimiters);
}


// CVector / CMap API functions:

#define                cstring_getRaw(x) ((x).str)
static inline uint32_t cstring_hash(const char** str, size_t sz_ignored) { return cdef_murmurHash(*str, strlen(*str)); }
static inline int      cstring_compare(CString* self, const char** str, size_t sz_ignored) { return strcmp(self->str, *str); }


#endif
