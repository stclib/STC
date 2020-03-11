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

#ifndef C_STRING__H__
#define C_STRING__H__

#include <malloc.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>

#include "c_defs.h"

typedef struct c_String {
    char* str;
} c_String;


static size_t _c_string_null_rep[] = {0, 0, 0};
#define       _c_string_rep(cs)      (((size_t *) (cs).str) - 2)

#define c_string_initializer   {(char* ) (_c_string_null_rep + 2)}
#define c_string_size(cs)      ((size_t) _c_string_rep(cs)[0])
#define c_string_capacity(cs)  ((size_t) _c_string_rep(cs)[1])
#define c_string_npos          ((size_t) -1)


static inline void c_string_reserve(c_String* self, size_t cap) {
    size_t len = c_string_size(*self), oldcap = c_string_capacity(*self);
    if (cap > oldcap) {
        size_t* rep = (size_t *) realloc(oldcap ? _c_string_rep(*self) : NULL, sizeof(size_t) * 2 + cap + 1);
        self->str = (char* ) (rep + 2);
        self->str[ rep[0] = len ] = '\0';
        rep[1] = cap;
    }
}

static inline void c_string_destroy(c_String* self) {
    if (c_string_capacity(*self)) {
        free(_c_string_rep(*self));
    }
}

static inline c_String c_string_init(void) {
    c_String cs = c_string_initializer;
    return cs;
}

static inline c_String c_string_makeN(const char* str, size_t len) {
    c_String cs = c_string_initializer;
    if (len) {
        c_string_reserve(&cs, len);
        memcpy(cs.str, str, len);
        cs.str[ _c_string_rep(cs)[0] = len ] = '\0';
    }
    return cs;
}

static inline c_String c_string_make(const char* str) {
    return c_string_makeN(str, strlen(str));
}

static inline c_String c_string_makeCopy(c_String cs) {
    return c_string_makeN(cs.str, c_string_size(cs));
}

static inline void c_string_clear(c_String* self) {
    c_String cs = c_string_initializer;
    c_string_destroy(self);
    *self = cs;
}

static inline c_String* c_string_assignN(c_String* self, const char* str, size_t len) {
    if (len) {
        c_string_reserve(self, len);
        memmove(self->str, str, len);
        self->str[_c_string_rep(*self)[0] = len] = '\0';
    }
    return self;
}

static inline c_String* c_string_assign(c_String* self, const char* str) {
    return c_string_assignN(self, str, strlen(str));
}

static inline c_String* c_string_copy(c_String* self, c_String cs2) {
    return c_string_assignN(self, cs2.str, c_string_size(cs2));
}


static inline c_String* c_string_appendN(c_String* self, const char* str, size_t len) {
    if (len) {
        size_t oldlen = c_string_size(*self), newlen = oldlen + len;
        if (newlen > c_string_capacity(*self))
            c_string_reserve(self, newlen * 5 / 3);
        memmove(&self->str[oldlen], str, len);
        self->str[_c_string_rep(*self)[0] = newlen] = '\0';
    }
    return self;
}

static inline c_String* c_string_append(c_String* self, const char* str) {
    return c_string_appendN(self, str, strlen(str));
}
static inline c_String* c_string_appendS(c_String* self, c_String cs2) {
    return c_string_appendN(self, cs2.str, c_string_size(cs2));
}


static inline void _c_string_internalMove(c_String* self, size_t pos1, size_t pos2) {
    if (pos1 == pos2)
        return;
    size_t len = c_string_size(*self), newlen = len + pos2 - pos1;
    if (newlen > c_string_capacity(*self))
        c_string_reserve(self, newlen * 5 / 3);
    memmove(&self->str[pos2], &self->str[pos1], len - pos1);
    self->str[_c_string_rep(*self)[0] = newlen] = '\0';
}

static inline void c_string_insertN(c_String* self, size_t pos, const char* str, size_t n) {
    char* xstr = (char *) memcpy(n > c_defs_max_alloca ? malloc(n) : alloca(n), str, n);
    _c_string_internalMove(self, pos, pos + n);
    memcpy(&self->str[pos], xstr, n);
    if (n > c_defs_max_alloca) free(xstr); 
}

static inline void c_string_insert(c_String* self, size_t pos, const char* str) {
    c_string_insertN(self, pos, str, strlen(str));
}

static inline void c_string_erase(c_String* self, size_t pos, size_t n) {
    size_t len = c_string_size(*self);
    if (len) {
        memmove(&self->str[pos], &self->str[pos + n], len - (pos + n));
        self->str[_c_string_rep(*self)[0] -= n] = '\0';
    }
}

static inline size_t c_string_findN(c_String cs, size_t pos, const char* needle, size_t n);

static inline size_t c_string_replaceN(c_String* self, size_t pos, const char* s1, size_t n1, const char* s2, size_t n2) {
    size_t pos2 = c_string_findN(*self, pos, s1, n1);
    if (pos2 == c_string_npos) return c_string_npos;
    char* xs2 = (char *) memcpy(n2 > c_defs_max_alloca ? malloc(n2) : alloca(n2), s2, n2);
    _c_string_internalMove(self, pos2 + n1, pos2 + n2);
    memcpy(&self->str[pos2], xs2, n2);
    if (n2 > c_defs_max_alloca) free(xs2);
    return pos2;
}

static inline size_t c_string_replace(c_String* self, size_t pos, const char* s1, const char* s2) {
    return c_string_replaceN(self, pos, s1, strlen(s1), s2, strlen(s2));
}


static inline char c_string_back(c_String cs) {
    return cs.str[c_string_size(cs) - 1];
}

static inline c_String* c_string_push(c_String* self, char value) {
    return c_string_appendN(self, &value, 1);
}


static inline void c_string_pop(c_String* self) {
    --_c_string_rep(*self)[0];
}

/* readonly */

static inline bool c_string_empty(c_String cs) {
    return c_string_size(cs) == 0;
}

static inline bool c_string_equals(c_String cs1, const char* str) {
    return strcmp(cs1.str, str) == 0;
}
static inline bool c_string_equalsS(c_String cs1, c_String cs2) {
    return strcmp(cs1.str, cs2.str) == 0;
}

static inline char* c_string_strnstr(c_String cs, size_t pos, const char* needle, size_t n) {
    char *x = cs.str + pos, // haystack
         *z = cs.str + c_string_size(cs) - n + 1;
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

static inline size_t c_string_findN(c_String cs, size_t pos, const char* needle, size_t n) {
    char* res = c_string_strnstr(cs, pos, needle, n);
    return res ? res - cs.str : c_string_npos;
}

static inline size_t c_string_find(c_String cs, size_t pos, const char* needle) {
    char* res = strstr(cs.str + pos, needle);
    return res ? res - cs.str : c_string_npos;
}

static inline char* c_string_splitFirst(const char* delimiters, c_String cs) {
    return strtok(cs.str, delimiters);
}

static inline char* c_string_splitNext(const char* delimiters) {
    return strtok(NULL, delimiters);
}


// c_Vector / c_Hashmap API functions:

#define                c_string_getRaw(x) ((x).str)
static inline uint32_t c_string_hashRaw(const char** str, size_t sz_ignored) { return c_defs_murmurHash(*str, strlen(*str)); }
static inline int      c_string_compareRaw(c_String* self, const char** str, size_t sz_ignored) { return strcmp(self->str, *str); }


#endif
