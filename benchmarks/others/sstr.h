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
#ifndef STRING_INCLUDED
#define STRING_INCLUDED

#include <stc/ccommon.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#ifndef sstr_size_t
typedef size_t sstr_size_t;
#endif
typedef struct { char* data; sstr_size_t size, cap; } sstr_rep_t;
typedef const char sstr_literal_t[];

typedef union {
    struct { char* data; sstr_size_t size, ncap; } lon;
    struct { char data[sizeof(sstr_rep_t)]; } sso;
} sstr;

/**************************** PRIVATE API **********************************/

enum { SSO_CAP = offsetof(sstr, lon.ncap) + sizeof((sstr){{0}}.lon.ncap) - 1 };
#define sstr_is_long(s)         (bool)((s)->sso.data[SSO_CAP] & 128)
#define sstr_select_(s, memb)   (sstr_is_long(s) ? sstr_l_##memb : sstr_s_##memb)

#define sstr_s_cap(s)           SSO_CAP
#define sstr_s_size(s)          ((sstr_size_t)(SSO_CAP - (s)->sso.data[SSO_CAP]))
#define sstr_s_set_size(s, len) ((s)->sso.data[SSO_CAP] = SSO_CAP - (len), (s)->sso.data[len] = 0)
#define sstr_s_data(s)          (s)->sso.data
#define sstr_s_end(s)           ((s)->sso.data + sstr_s_size(s))

#if defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#define byte_rotl_(x, b)          ((x) << (b)*8 | (x) >> (sizeof(x) - (b))*8)
#define sstr_l_cap(s)           (~byte_rotl_((s)->lon.ncap, sizeof((s)->lon.ncap) - 1))
#define sstr_l_set_cap(s, cap)  ((s)->lon.ncap = ~byte_rotl_(cap, 1))
#else
#define sstr_l_cap(s)           (~(s)->lon.ncap)
#define sstr_l_set_cap(s, cap)  ((s)->lon.ncap = ~(cap))
#endif
#define sstr_l_size(s)          ((s)->lon.size)
#define sstr_l_set_size(s, len) ((s)->lon.data[(s)->lon.size = (len)] = 0)
#define sstr_l_data(s)          (s)->lon.data
#define sstr_l_end(s)           ((s)->lon.data + sstr_l_size(s))
#define sstr_l_del(s)           free((s)->lon.data)

static inline void sstr_set_size_(sstr* self, sstr_size_t len) {
    sstr_select_(self, set_size(self, len));
}

static inline sstr_rep_t sstr_rep_(sstr* self) {
    return sstr_is_long(self)
            ? c_make(sstr_rep_t){self->lon.data, sstr_l_size(self), sstr_l_cap(self)}
            : c_make(sstr_rep_t){self->sso.data, sstr_s_size(self), sstr_s_cap(self)};
}

static inline char* sstr_init_(sstr* self, sstr_size_t len, sstr_size_t cap) {
    if (cap > SSO_CAP) {
        self->lon.data = (char *)malloc(cap + 1);
        sstr_l_set_size(self, len);
        sstr_l_set_cap(self, cap);
        return self->lon.data;
    }
    sstr_s_set_size(self, len);
    return self->sso.data;
}

static inline char* sstr_reserve(sstr* self, sstr_size_t cap);
static inline void sstr_internal_move_(sstr* self, size_t pos1, size_t pos2) {
    if (pos1 == pos2)
        return;
    sstr_rep_t rep = sstr_rep_(self);
    sstr_size_t newlen = rep.size + pos2 - pos1;
    if (newlen > rep.cap)
        rep.data = sstr_reserve(self, (rep.size*13 >> 3) + pos2 - pos1);
    memmove(&rep.data[pos2], &rep.data[pos1], rep.size - pos1);
    sstr_set_size_(self, newlen);
}

/**************************** PUBLIC API **********************************/

#define sstr_lit(lit) sstr_from_n(lit, sizeof((sstr_literal_t){lit}) - 1)
#define sstr_npos (~(sstr_size_t)0 >> 1)

static inline sstr sstr_init(void) {
    sstr s;
    sstr_s_set_size(&s, 0);
    return s;
}

static inline sstr sstr_from_n(const char* str, sstr_size_t n) {
    sstr s;
    memcpy(sstr_init_(&s, n, n), str, n);
    return s;
}

static inline sstr sstr_from(const char* str) {
    return sstr_from_n(str, strlen(str));
}

static inline sstr sstr_with_size(sstr_size_t size, char value) {
    sstr s;
    memset(sstr_init_(&s, size, size), value, size);
    return s;
}

static inline sstr sstr_with_capacity(sstr_size_t cap) {
    sstr s;
    sstr_init_(&s, 0, cap);
    return s;
}

static inline sstr sstr_clone(sstr s) {
    sstr_rep_t rep = sstr_rep_(&s);
    return sstr_from_n(rep.data, rep.size);
}

static inline void sstr_del(sstr* self) {
    if (sstr_is_long(self)) sstr_l_del(self);
}

static inline void sstr_clear(sstr* self) {
    sstr_set_size_(self, 0);
}

static inline void sstr_shrink_to_fit(sstr* self) {
    sstr_rep_t rep = sstr_rep_(self);
    if (rep.size == rep.cap)
        return;
    if (rep.size > SSO_CAP) {
        self->lon.data = (char *)realloc(self->lon.data, sstr_l_size(self) + 1);
        sstr_l_set_cap(self, sstr_l_size(self));
    } else if (rep.cap > SSO_CAP) {
        memcpy(self->sso.data, rep.data, rep.size + 1);
        sstr_s_set_size(self, rep.size);
        free(rep.data);
    }
}

static inline char* sstr_reserve(sstr* self, sstr_size_t cap) {
    if (sstr_is_long(self)) {
        if (cap > sstr_l_cap(self)) {
            self->lon.data = (char *)realloc(self->lon.data, cap + 1);
            sstr_l_set_cap(self, cap);
        }
        return self->lon.data;
    }
    /* from short to long: */
    if (cap > sstr_s_cap(self)) {
        char* data = (char *)malloc(cap + 1);
        sstr_size_t len = sstr_s_size(self);
        memcpy(data, self->sso.data, len);
        self->lon.data = data;
        sstr_l_set_size(self, len);
        sstr_l_set_cap(self, cap);
        return data;
    }
    return self->sso.data;
}

static inline void sstr_resize(sstr* self, sstr_size_t size, char value) {
    sstr_rep_t rep = sstr_rep_(self);
    if (size > rep.size) {
        if (size > rep.cap) rep.data = sstr_reserve(self, size);
        memset(rep.data + rep.size, value, size - rep.size);
    }
    sstr_set_size_(self, size);
}

static inline char* sstr_data(sstr* self) {
    return sstr_select_(self, data(self));
}

static inline const char* sstr_str(const sstr* self) {
    return sstr_select_(self, data(self));
}

static inline bool sstr_empty(sstr s) {
    return sstr_select_(&s, size(&s)) == 0;
}

static inline sstr_size_t sstr_size(sstr s) {
    return sstr_select_(&s, size(&s));
}

static inline sstr_size_t sstr_length(sstr s) {
    return sstr_select_(&s, size(&s));
}

static inline sstr_size_t sstr_capacity(sstr s) {
    return sstr_select_(&s, cap(&s));
}

static inline bool sstr_equals(sstr s1, const char* str) {
    return strcmp(sstr_str(&s1), str) == 0;
}

static inline bool sstr_equals_s(sstr s1, sstr s2) {
    return strcmp(sstr_str(&s1), sstr_str(&s2)) == 0;
}

static inline int sstr_equals_ref(const sstr* s1, const sstr* s2) {
    return strcmp(sstr_str(s1), sstr_str(s2)) == 0;
}

static inline int sstr_compare_ref(const sstr* s1, const sstr* s2) {
    return strcmp(sstr_str(s1), sstr_str(s2));
}

static inline sstr_size_t sstr_find(sstr s, const char* needle) {
    const char *str = sstr_str(&s), *res = strstr(str, needle);
    return res ? res - str : sstr_npos;
}

static inline bool sstr_find_s(sstr s, sstr needle) {
   return sstr_find(s, sstr_str(&needle));
}

static inline bool sstr_contains(sstr s, const char* needle) {
   return strstr(sstr_str(&s), needle) != NULL;
}

static inline bool sstr_contains_s(sstr s, sstr needle) {
   return strstr(sstr_str(&s), sstr_str(&needle)) != NULL;
}

static inline bool sstr_starts_with(sstr s, const char* sub) {
    const char* str = sstr_str(&s);
    while (*sub && *str == *sub) ++str, ++sub;
    return *sub == 0;
}

static inline bool sstr_starts_with_s(sstr s, sstr sub) {
   return sstr_starts_with(s, sstr_str(&sub));
}

static inline bool sstr_ends_with(sstr s, const char* sub) {
    sstr_rep_t rep = sstr_rep_(&s); sstr_size_t n = strlen(sub);
    return n <= rep.size && memcmp(rep.data + rep.size - n, sub, n) == 0;
}

static inline bool sstr_ends_with_s(sstr s, sstr sub) {
   return sstr_ends_with(s, sstr_str(&sub));
}

static inline char* strnstrn(const char *s, const char *needle, size_t slen, size_t nlen) {
    if (!nlen) return (char *)s;
    if (nlen > slen) return NULL;
    slen -= nlen;
    do {
        if (*s == *needle && !memcmp(s, needle, nlen)) return (char *)s;
        ++s;
    } while (slen--);
    return NULL;
}

static inline sstr_size_t
sstr_find_n(sstr s, const char* needle, sstr_size_t pos, sstr_size_t nmax) {
    sstr_rep_t rep = sstr_rep_(&s);
    sstr_size_t nlen = (sstr_size_t) strlen(needle);
    if (pos > rep.size) return sstr_npos;
    char* res = strnstrn(rep.data + pos, needle, rep.size, nmax < nlen ? nmax : nlen);
    return res ? res - rep.data : sstr_npos;
}

static inline void sstr_assign_n(sstr* self, const char* str, sstr_size_t n) {
    sstr_rep_t rep = sstr_rep_(self);
    if (n > rep.cap) {
        rep.data = (char *)realloc(sstr_is_long(self) ? rep.data : NULL, n + 1);
        sstr_l_set_cap(self, n);
    }
    memmove(rep.data, str, n);
    sstr_set_size_(self, n);
}

static inline void sstr_assign(sstr* self, const char* str) {
    sstr_assign_n(self, str, strlen(str));
}

static inline void sstr_copy(sstr* self, sstr s) {
    sstr_rep_t rep = sstr_rep_(&s);
    sstr_assign_n(self, rep.data, rep.size);
}

static inline void sstr_append_n(sstr* self, const char* str, sstr_size_t n) {
    sstr_rep_t rep = sstr_rep_(self);
    if (rep.size + n > rep.cap) {
        sstr_size_t off = (sstr_size_t)(str - rep.data); /* handle self append */
        rep.data = sstr_reserve(self, (rep.size*13 >> 3) + n);
        if (off <= rep.size) str = rep.data + off;
    }
    memcpy(rep.data + rep.size, str, n);
    sstr_set_size_(self, rep.size + n);
}

static inline void sstr_append(sstr* self, const char* str) {
    sstr_append_n(self, str, strlen(str));
}

static inline void sstr_append_s(sstr* self, sstr s) {
    sstr_rep_t rep = sstr_rep_(&s);
    sstr_append_n(self, rep.data, rep.size);
}

static inline void sstr_replace_n(sstr* self, size_t pos, size_t len, const char* str, size_t n) {
    sstr_internal_move_(self, pos + len, pos + n);
    memcpy(&sstr_data(self)[pos], str, n);
}

static inline void sstr_replace(sstr* self, size_t pos, size_t len, const char* str) {
    sstr_replace_n(self, pos, len, str, strlen(str));
}

static inline void sstr_replace_s(sstr* self, size_t pos, size_t len, sstr s) {
    sstr_rep_t rep = sstr_rep_(&s);
    sstr_replace_n(self, pos, len, rep.data, rep.size);
}

static inline void sstr_insert_n(sstr* self, size_t pos, const char* str, size_t n) {
    sstr_replace_n(self, pos, 0, str, n);
}

static inline void sstr_insert(sstr* self, size_t pos, const char* str) {
    sstr_replace_n(self, pos, 0, str, strlen(str));
}

static inline void sstr_insert_s(sstr* self, size_t pos, sstr s) {
    sstr_rep_t rep = sstr_rep_(&s);
    sstr_replace_n(self, pos, 0, rep.data, rep.size);
}

static inline void sstr_erase_n(sstr* self, size_t pos, size_t n) {
    sstr_rep_t rep = sstr_rep_(self);
    if (n > rep.size - pos) n = rep.size - pos;
    memmove(&rep.data[pos], &rep.data[pos + n], rep.size - (pos + n));
    sstr_set_size_(self, rep.size - n);
}

static inline bool sstr_getdelim(sstr *self, int delim, FILE *fp) {
    int c = fgetc(fp);
    if (c == EOF)
        return false;
    sstr_size_t pos = 0;
    sstr_rep_t rep = sstr_rep_(self);
    for (;;) {
        if (c == delim || c == EOF) {
            sstr_set_size_(self, pos);
            return true;
        }
        if (pos == rep.cap) {
            sstr_set_size_(self, pos);
            rep.data = sstr_reserve(self, (rep.cap = (rep.cap*13 >> 3) + 16));
        }
        rep.data[pos++] = (char) c;
        c = fgetc(fp);
    }
}

static inline bool sstr_getline(sstr *self, FILE *fp) {
    return sstr_getdelim(self, '\n', fp);
}

#endif