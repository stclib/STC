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

/* A string type with short string optimization in C99 with optimal short string
 * utilization (23 characters with 24 bytes string representation).
 */
#ifndef CSTR_INCLUDED
#define CSTR_INCLUDED

#include <stc/ccommon.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#ifndef cstr_size_t
typedef size_t cstr_size_t;
#endif
typedef struct { char* data; cstr_size_t size, cap; } cstr_rep_t;
typedef const char cstr_literal_t[];

typedef union {
    struct { char* data; cstr_size_t size, ncap; } lon;
    struct { char data[sizeof(cstr_rep_t)]; } sso;
} cstr;

/**************************** PRIVATE API **********************************/

enum { SSO_CAP = offsetof(cstr, lon.ncap) + sizeof((cstr){{0}}.lon.ncap) - 1 };
#define cstr_is_long(s)         (bool)((s)->sso.data[SSO_CAP] & 128)
#define cstr_select_(s, memb)   (cstr_is_long(s) ? cstr_l_##memb : cstr_s_##memb)

#define cstr_s_cap(s)           SSO_CAP
#define cstr_s_size(s)          ((cstr_size_t)(SSO_CAP - (s)->sso.data[SSO_CAP]))
#define cstr_s_set_size(s, len) ((s)->sso.data[SSO_CAP] = SSO_CAP - (len), (s)->sso.data[len] = 0)
#define cstr_s_data(s)          (s)->sso.data
#define cstr_s_end(s)           ((s)->sso.data + cstr_s_size(s))

#if defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#define byte_rotl_(x, b)          ((x) << (b)*8 | (x) >> (sizeof(x) - (b))*8)
#define cstr_l_cap(s)           (~byte_rotl_((s)->lon.ncap, sizeof((s)->lon.ncap) - 1))
#define cstr_l_set_cap(s, cap)  ((s)->lon.ncap = ~byte_rotl_(cap, 1))
#else
#define cstr_l_cap(s)           (~(s)->lon.ncap)
#define cstr_l_set_cap(s, cap)  ((s)->lon.ncap = ~(cap))
#endif
#define cstr_l_size(s)          ((s)->lon.size)
#define cstr_l_set_size(s, len) ((s)->lon.data[(s)->lon.size = (len)] = 0)
#define cstr_l_data(s)          (s)->lon.data
#define cstr_l_end(s)           ((s)->lon.data + cstr_l_size(s))
#define cstr_l_del(s)           free((s)->lon.data)

STC_API char* cstr_init_(cstr* self, cstr_size_t len, cstr_size_t cap);
STC_API void cstr_internal_move_(cstr* self, size_t pos1, size_t pos2);

STC_INLINE void cstr_set_size_(cstr* self, cstr_size_t len) {
    cstr_select_(self, set_size(self, len));
}

STC_INLINE cstr_rep_t cstr_rep_(cstr* self) {
    return cstr_is_long(self)
            ? c_make(cstr_rep_t){self->lon.data, cstr_l_size(self), cstr_l_cap(self)}
            : c_make(cstr_rep_t){self->sso.data, cstr_s_size(self), cstr_s_cap(self)};
}

/**************************** PUBLIC API **********************************/

#define cstr_lit(lit) cstr_from_n(lit, sizeof((cstr_literal_t){lit}) - 1)
#define cstr_npos (~(cstr_size_t)0 >> 1)

STC_API char* cstr_reserve(cstr* self, cstr_size_t cap);
STC_API void cstr_shrink_to_fit(cstr* self);
STC_API void cstr_resize(cstr* self, cstr_size_t size, char value);
STC_API char* strnstrn(const char *s, const char *needle, size_t slen, size_t nlen);
STC_API cstr_size_t cstr_find_n(cstr s, const char* needle, cstr_size_t pos, cstr_size_t nmax);
STC_API void cstr_assign_n(cstr* self, const char* str, cstr_size_t n);
STC_API void cstr_append_n(cstr* self, const char* str, cstr_size_t n);
STC_API bool cstr_getdelim(cstr *self, int delim, FILE *fp);
STC_API void cstr_erase_n(cstr* self, size_t pos, size_t n);


STC_INLINE cstr cstr_init(void) {
    cstr s;
    cstr_s_set_size(&s, 0);
    return s;
}

STC_INLINE cstr cstr_from_n(const char* str, cstr_size_t n) {
    cstr s;
    memcpy(cstr_init_(&s, n, n), str, n);
    return s;
}

STC_INLINE cstr cstr_from(const char* str) {
    return cstr_from_n(str, strlen(str));
}

STC_INLINE cstr cstr_with_size(cstr_size_t size, char value) {
    cstr s;
    memset(cstr_init_(&s, size, size), value, size);
    return s;
}

STC_INLINE cstr cstr_with_capacity(cstr_size_t cap) {
    cstr s;
    cstr_init_(&s, 0, cap);
    return s;
}

STC_INLINE cstr cstr_clone(cstr s) {
    cstr_rep_t rep = cstr_rep_(&s);
    return cstr_from_n(rep.data, rep.size);
}

STC_INLINE void cstr_del(cstr* self) {
    if (cstr_is_long(self)) cstr_l_del(self);
}

STC_INLINE void cstr_clear(cstr* self) {
    cstr_set_size_(self, 0);
}

STC_INLINE char* cstr_data(cstr* self) {
    return cstr_select_(self, data(self));
}

STC_INLINE const char* cstr_str(const cstr* self) {
    return cstr_select_(self, data(self));
}

STC_INLINE bool cstr_empty(cstr s) {
    return cstr_select_(&s, size(&s)) == 0;
}

STC_INLINE cstr_size_t cstr_size(cstr s) {
    return cstr_select_(&s, size(&s));
}

STC_INLINE cstr_size_t cstr_length(cstr s) {
    return cstr_select_(&s, size(&s));
}

STC_INLINE cstr_size_t cstr_capacity(cstr s) {
    return cstr_select_(&s, cap(&s));
}

STC_INLINE bool cstr_equalto(cstr s1, const char* str) {
    return strcmp(cstr_str(&s1), str) == 0;
}

STC_INLINE bool cstr_equalto_s(cstr s1, cstr s2) {
    return strcmp(cstr_str(&s1), cstr_str(&s2)) == 0;
}

STC_INLINE int cstr_equals(const cstr* s1, const cstr* s2) {
    return strcmp(cstr_str(s1), cstr_str(s2)) == 0;
}

STC_INLINE int cstr_compare(const cstr* s1, const cstr* s2) {
    return strcmp(cstr_str(s1), cstr_str(s2));
}

STC_INLINE cstr_size_t cstr_find(cstr s, const char* needle) {
    const char *str = cstr_str(&s), *res = strstr(str, needle);
    return res ? res - str : cstr_npos;
}

STC_INLINE bool cstr_find_s(cstr s, cstr needle) {
   return cstr_find(s, cstr_str(&needle));
}

STC_INLINE bool cstr_contains(cstr s, const char* needle) {
   return strstr(cstr_str(&s), needle) != NULL;
}

STC_INLINE bool cstr_contains_s(cstr s, cstr needle) {
   return strstr(cstr_str(&s), cstr_str(&needle)) != NULL;
}

STC_INLINE bool cstr_starts_with(cstr s, const char* sub) {
    const char* str = cstr_str(&s);
    while (*sub && *str == *sub) ++str, ++sub;
    return *sub == 0;
}

STC_INLINE bool cstr_starts_with_s(cstr s, cstr sub) {
   return cstr_starts_with(s, cstr_str(&sub));
}

STC_INLINE bool cstr_ends_with(cstr s, const char* sub) {
    cstr_rep_t rep = cstr_rep_(&s); cstr_size_t n = strlen(sub);
    return n <= rep.size && memcmp(rep.data + rep.size - n, sub, n) == 0;
}

STC_INLINE bool cstr_ends_with_s(cstr s, cstr sub) {
   return cstr_ends_with(s, cstr_str(&sub));
}

STC_INLINE void cstr_assign(cstr* self, const char* str) {
    cstr_assign_n(self, str, strlen(str));
}

STC_INLINE void cstr_copy(cstr* self, cstr s) {
    cstr_rep_t rep = cstr_rep_(&s);
    cstr_assign_n(self, rep.data, rep.size);
}

STC_INLINE void cstr_append(cstr* self, const char* str) {
    cstr_append_n(self, str, strlen(str));
}

STC_INLINE void cstr_append_s(cstr* self, cstr s) {
    cstr_rep_t rep = cstr_rep_(&s);
    cstr_append_n(self, rep.data, rep.size);
}

STC_INLINE void cstr_replace_n(cstr* self, size_t pos, size_t len, const char* str, size_t n) {
    cstr_internal_move_(self, pos + len, pos + n);
    memcpy(&cstr_data(self)[pos], str, n);
}

STC_INLINE void cstr_replace(cstr* self, size_t pos, size_t len, const char* str) {
    cstr_replace_n(self, pos, len, str, strlen(str));
}

STC_INLINE void cstr_replace_s(cstr* self, size_t pos, size_t len, cstr s) {
    cstr_rep_t rep = cstr_rep_(&s);
    cstr_replace_n(self, pos, len, rep.data, rep.size);
}

STC_INLINE void cstr_insert_n(cstr* self, size_t pos, const char* str, size_t n) {
    cstr_replace_n(self, pos, 0, str, n);
}

STC_INLINE void cstr_insert(cstr* self, size_t pos, const char* str) {
    cstr_replace_n(self, pos, 0, str, strlen(str));
}

STC_INLINE void cstr_insert_s(cstr* self, size_t pos, cstr s) {
    cstr_rep_t rep = cstr_rep_(&s);
    cstr_replace_n(self, pos, 0, rep.data, rep.size);
}

STC_INLINE bool cstr_getline(cstr *self, FILE *fp) {
    return cstr_getdelim(self, '\n', fp);
}

/* -------------------------- IMPLEMENTATION ------------------------- */

#if !defined(STC_HEADER) || defined(STC_IMPLEMENTATION)

STC_DEF void cstr_internal_move_(cstr* self, size_t pos1, size_t pos2) {
    if (pos1 == pos2)
        return;
    cstr_rep_t rep = cstr_rep_(self);
    cstr_size_t newlen = rep.size + pos2 - pos1;
    if (newlen > rep.cap)
        rep.data = cstr_reserve(self, (rep.size*13 >> 3) + pos2 - pos1);
    memmove(&rep.data[pos2], &rep.data[pos1], rep.size - pos1);
    cstr_set_size_(self, newlen);
}

STC_DEF char* cstr_init_(cstr* self, cstr_size_t len, cstr_size_t cap) {
    if (cap > SSO_CAP) {
        self->lon.data = (char *)malloc(cap + 1);
        cstr_l_set_size(self, len);
        cstr_l_set_cap(self, cap);
        return self->lon.data;
    }
    cstr_s_set_size(self, len);
    return self->sso.data;
}

STC_DEF void cstr_shrink_to_fit(cstr* self) {
    cstr_rep_t rep = cstr_rep_(self);
    if (rep.size == rep.cap)
        return;
    if (rep.size > SSO_CAP) {
        self->lon.data = (char *)realloc(self->lon.data, cstr_l_size(self) + 1);
        cstr_l_set_cap(self, cstr_l_size(self));
    } else if (rep.cap > SSO_CAP) {
        memcpy(self->sso.data, rep.data, rep.size + 1);
        cstr_s_set_size(self, rep.size);
        free(rep.data);
    }
}

STC_DEF char* cstr_reserve(cstr* self, cstr_size_t cap) {
    if (cstr_is_long(self)) {
        if (cap > cstr_l_cap(self)) {
            self->lon.data = (char *)realloc(self->lon.data, cap + 1);
            cstr_l_set_cap(self, cap);
        }
        return self->lon.data;
    }
    /* from short to long: */
    if (cap > cstr_s_cap(self)) {
        char* data = (char *)malloc(cap + 1);
        cstr_size_t len = cstr_s_size(self);
        memcpy(data, self->sso.data, len);
        self->lon.data = data;
        cstr_l_set_size(self, len);
        cstr_l_set_cap(self, cap);
        return data;
    }
    return self->sso.data;
}

STC_DEF void cstr_resize(cstr* self, cstr_size_t size, char value) {
    cstr_rep_t rep = cstr_rep_(self);
    if (size > rep.size) {
        if (size > rep.cap) rep.data = cstr_reserve(self, size);
        memset(rep.data + rep.size, value, size - rep.size);
    }
    cstr_set_size_(self, size);
}

STC_DEF char* strnstrn(const char *s, const char *needle, size_t slen, size_t nlen) {
    if (!nlen) return (char *)s;
    if (nlen > slen) return NULL;
    slen -= nlen;
    do {
        if (*s == *needle && !memcmp(s, needle, nlen)) return (char *)s;
        ++s;
    } while (slen--);
    return NULL;
}

STC_DEF cstr_size_t
cstr_find_n(cstr s, const char* needle, cstr_size_t pos, cstr_size_t nmax) {
    cstr_rep_t rep = cstr_rep_(&s);
    cstr_size_t nlen = (cstr_size_t) strlen(needle);
    if (pos > rep.size) return cstr_npos;
    char* res = strnstrn(rep.data + pos, needle, rep.size, nmax < nlen ? nmax : nlen);
    return res ? res - rep.data : cstr_npos;
}

STC_DEF void cstr_assign_n(cstr* self, const char* str, cstr_size_t n) {
    cstr_rep_t rep = cstr_rep_(self);
    if (n > rep.cap) {
        rep.data = (char *)realloc(cstr_is_long(self) ? rep.data : NULL, n + 1);
        cstr_l_set_cap(self, n);
    }
    memmove(rep.data, str, n);
    cstr_set_size_(self, n);
}

STC_DEF void cstr_append_n(cstr* self, const char* str, cstr_size_t n) {
    cstr_rep_t rep = cstr_rep_(self);
    if (rep.size + n > rep.cap) {
        cstr_size_t off = (cstr_size_t)(str - rep.data); /* handle self append */
        rep.data = cstr_reserve(self, (rep.size*13 >> 3) + n);
        if (off <= rep.size) str = rep.data + off;
    }
    memcpy(rep.data + rep.size, str, n);
    cstr_set_size_(self, rep.size + n);
}

STC_DEF bool cstr_getdelim(cstr *self, int delim, FILE *fp) {
    int c = fgetc(fp);
    if (c == EOF)
        return false;
    cstr_size_t pos = 0;
    cstr_rep_t rep = cstr_rep_(self);
    for (;;) {
        if (c == delim || c == EOF) {
            cstr_set_size_(self, pos);
            return true;
        }
        if (pos == rep.cap) {
            cstr_set_size_(self, pos);
            rep.data = cstr_reserve(self, (rep.cap = (rep.cap*13 >> 3) + 16));
        }
        rep.data[pos++] = (char) c;
        c = fgetc(fp);
    }
}

STC_DEF void cstr_erase_n(cstr* self, size_t pos, size_t n) {
    cstr_rep_t rep = cstr_rep_(self);
    if (n > rep.size - pos) n = rep.size - pos;
    memmove(&rep.data[pos], &rep.data[pos + n], rep.size - (pos + n));
    cstr_set_size_(self, rep.size - n);
}

#endif
#endif