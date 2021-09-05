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
#ifndef CLIST_H_INCLUDED
#define CLIST_H_INCLUDED

/*  Circular Singly-linked Lists.

    This implements a std::forward_list-like class in C, but because it is circular,
    it also support push* and splice* at both ends of the list. This makes it ideal
    for being used as a queue, unlike std::forward_list. Basic usage is similar to cvec:

    #include <stdio.h>
    #include <stc/clist.h>
    #include <stc/crandom.h>
    using_clist(ix, int64_t);

    int main() {
        c_forvar (clist_ix list = clist_ix_init(), clist_ix_del(&list))
        {
            stc64_t rng = stc64_init(12345);
            int n;
            for (int i=0; i<1000000; ++i) // one million
                clist_ix_push_back(&list, stc64_rand(&rng) >> 32);
            n = 0;
            c_foreach (i, clist_ix, list)
                if (++n % 10000 == 0) printf("%8d: %10zd\n", n, i.ref->value);
            // Sort them...
            clist_ix_sort(&list); // mergesort O(n*log n)
            n = 0;
            puts("sorted");
            c_foreach (i, clist_ix, list)
                if (++n % 10000 == 0) printf("%8d: %10zd\n", n, i.ref->value);
        }
    }
*/
#include "ccommon.h"
#include <stdlib.h>



#define _c_clist_complete_types(Self) \
    struct cx_node_t { \
        struct cx_node_t *next; \
        cx_value_t value; \
    }

_c_clist_types(clist_VOID, int);
_c_clist_complete_types(clist_VOID);
STC_API size_t _clist_count(const clist_VOID* self);
#define _clist_node(Self, vp) c_container_of(vp, cx_node_t, value)


    defTypes( _c_clist_types(Self, i_VAL); ) \
    _c_clist_complete_types(Self); \
    typedef i_VALRAW cx_rawvalue_t; \
\
    STC_API Self              cx_memb(_clone)(Self cx); \
    STC_API void            cx_memb(_del)(Self* self); \
    STC_API void            cx_memb(_push_back)(Self* self, i_VAL value); \
    STC_API void            cx_memb(_push_front)(Self* self, i_VAL value); \
    STC_API cx_iter_t     cx_memb(_insert)(Self* self, cx_iter_t it, i_VAL value); \
    STC_API void            cx_memb(_emplace_items)(Self *self, const cx_rawvalue_t arr[], size_t n); \
    STC_API cx_iter_t     cx_memb(_erase_at)(Self* self, cx_iter_t it); \
    STC_API cx_iter_t     cx_memb(_erase_range)(Self* self, cx_iter_t it1, cx_iter_t it2); \
    STC_API size_t          cx_memb(_remove)(Self* self, i_VALRAW val); \
    STC_API cx_iter_t     cx_memb(_splice)(Self* self, cx_iter_t it, Self* other); \
    STC_API Self              cx_memb(_split_off)(Self* self, cx_iter_t it1, cx_iter_t it2); \
    STC_API void            cx_memb(_sort)(Self* self); \
    STC_API cx_iter_t     cx_memb(_find_in)(cx_iter_t it1, cx_iter_t it2, i_VALRAW val); \
    STC_API cx_node_t*    cx_memb(_erase_after_)(Self* self, cx_node_t* node); \
\
    STC_INLINE Self           cx_memb(_init)(void) { return c_make(Self){NULL}; } \
    STC_INLINE bool         cx_memb(_empty)(Self cx) { return cx.last == NULL; } \
    STC_INLINE size_t       cx_memb(_count)(Self cx) \
                                { return _clist_count((const clist_VOID*) &cx); } \
    STC_INLINE void         cx_memb(_clear)(Self* self) {cx_memb(_del)(self); } \
    STC_INLINE i_VAL        cx_memb(_value_fromraw)(i_VALRAW raw) { return i_VALFROM(raw); } \
    STC_INLINE i_VALRAW     cx_memb(_value_toraw)(cx_value_t* pval) { return i_VALTO(pval); } \
    STC_INLINE i_VAL        cx_memb(_value_clone)(i_VAL val) \
                                { return i_VALFROM(i_VALTO(&val)); } \
    STC_INLINE void         cx_memb(_pop_front)(Self* self) \
                                {cx_memb(_erase_after_)(self, self->last); } \
    STC_INLINE cx_iter_t  cx_memb(_erase)(Self* self, cx_iter_t it) \
                                { return cx_memb(_erase_at)(self, it); } \
    STC_INLINE void         cx_memb(_emplace_back)(Self* self, i_VALRAW raw) \
                                {cx_memb(_push_back)(self, i_VALFROM(raw)); } \
    STC_INLINE void         cx_memb(_emplace_front)(Self* self, i_VALRAW raw) \
                                {cx_memb(_push_front)(self, i_VALFROM(raw)); } \
    STC_INLINE cx_iter_t  cx_memb(_emplace)(Self* self, cx_iter_t it, i_VALRAW raw) \
                                { return cx_memb(_insert)(self, it, i_VALFROM(raw)); } \
    STC_INLINE cx_value_t* cx_memb(_front)(const Self* self) { return &self->last->next->value; } \
    STC_INLINE cx_value_t* cx_memb(_back)(const Self* self) { return &self->last->value; } \
\
    STC_INLINE cx_iter_t \
    cx_memb(_iter)(const Self* self, cx_node_t* prev) { \
        return c_make(cx_iter_t){&self->last, prev, &prev->next->value}; \
    } \
\
    STC_INLINE cx_iter_t \
    cx_memb(_begin)(const Self* self) { \
        cx_value_t* head = self->last ? &self->last->next->value : NULL; \
        return c_make(cx_iter_t){&self->last, self->last, head}; \
    } \
\
    STC_INLINE cx_iter_t \
    cx_memb(_end)(const Self* self) { \
        return c_make(cx_iter_t){NULL}; \
    } \
\
    STC_INLINE void \
    cx_memb(_next)(cx_iter_t* it) { \
        cx_node_t* node = it->prev = _clist_node(Self, it->ref); \
        it->ref = (node == *it->_last ? NULL : &node->next->value); \
    } \
\
    STC_INLINE cx_iter_t \
    cx_memb(_fwd)(cx_iter_t it, size_t n) { \
        while (n-- && it.ref) cx_memb(_next)(&it); \
        return it; \
    } \
\
    STC_INLINE cx_iter_t \
    cx_memb(_splice_range)(Self* self, cx_iter_t it, \
                      Self* other, cx_iter_t it1, cx_iter_t it2) { \
        Self tmp = cx_memb(_split_off)(other, it1, it2); \
        return cx_memb(_splice)(self, it, &tmp); \
    } \
\
    STC_INLINE cx_iter_t \
    cx_memb(_find)(const Self* self, i_VALRAW val) { \
        return cx_memb(_find_in)(cx_memb(_begin)(self), cx_memb(_end)(self), val); \
    } \
\
    STC_INLINE cx_value_t* \
    cx_memb(_get)(const Self* self, i_VALRAW val) { \
        return cx_memb(_find_in)(cx_memb(_begin)(self), cx_memb(_end)(self), val).ref; \
    } \
\
    _c_implement_clist(Self, i_VAL, i_CMP, i_VALDEL, i_VALFROM, i_VALTO, i_VALRAW) \
    struct stc_trailing_semicolon

/* -------------------------- IMPLEMENTATION ------------------------- */

#if !defined(STC_HEADER) || defined(STC_IMPLEMENTATION)
#define _c_implement_clist(Self, i_VAL, i_CMP, i_VALDEL, i_VALFROM, i_VALTO, i_VALRAW) \
\
    STC_DEF Self \
    cx_memb(_clone)(Self cx) { \
        Self out = cx_memb(_init)(); \
        c_foreach_3 (it, Self, cx) cx_memb(_emplace_back)(&out, i_VALTO(it.ref)); \
        return out; \
    } \
\
    STC_DEF void \
    cx_memb(_del)(Self* self) { \
        while (self->last) cx_memb(_erase_after_)(self, self->last); \
    } \
\
    STC_DEF void \
    cx_memb(_push_back)(Self* self, i_VAL value) { \
        _c_clist_insert_after(self, Self, self->last, value); \
        self->last = entry; \
    } \
\
    STC_DEF void \
    cx_memb(_push_front)(Self* self, i_VAL value) { \
        _c_clist_insert_after(self, Self, self->last, value); \
        if (!self->last) self->last = entry; \
    } \
\
    STC_DEF void \
    cx_memb(_emplace_items)(Self *self, const cx_rawvalue_t arr[], size_t n) { \
        for (size_t i=0; i<n; ++i) cx_memb(_push_back)(self, i_VALFROM(arr[i])); \
    } \
\
    STC_DEF cx_iter_t \
    cx_memb(_insert)(Self* self, cx_iter_t it, i_VAL value) { \
        cx_node_t* node = it.ref ? it.prev : self->last; \
        _c_clist_insert_after(self, Self, node, value); \
        if (!self->last || !it.ref) { \
            it.prev = self->last ? self->last : entry; \
            self->last = entry; \
        } \
        it.ref = &entry->value; \
        return it; \
    } \
\
    STC_DEF cx_iter_t \
    cx_memb(_erase_at)(Self* self, cx_iter_t it) { \
        cx_node_t *node = _clist_node(Self, it.ref); \
        it.ref = (node == self->last) ? NULL : &node->next->value; \
        cx_memb(_erase_after_)(self, it.prev); \
        return it; \
    } \
\
    STC_DEF cx_iter_t \
    cx_memb(_erase_range)(Self* self, cx_iter_t it1, cx_iter_t it2) { \
        cx_node_t *node = it1.ref ? it1.prev : NULL, \
                    *done = it2.ref ? _clist_node(Self, it2.ref) : NULL; \
        while (node && node->next != done) \
            node = cx_memb(_erase_after_)(self, node); \
        return it2; \
    } \
\
    STC_DEF cx_iter_t \
    cx_memb(_find_in)(cx_iter_t it1, cx_iter_t it2, i_VALRAW val) { \
        c_foreach_4 (it, Self, it1, it2) { \
            i_VALRAW r = i_VALTO(it.ref); \
            if (i_CMP(&r, &val) == 0) return it; \
        } \
        it2.ref = NULL; return it2; \
    } \
\
    STC_DEF cx_node_t* \
    cx_memb(_erase_after_)(Self* self, cx_node_t* node) { \
        cx_node_t* del = node->next, *next = del->next; \
        node->next = next; \
        if (del == next) self->last = node = NULL; \
        else if (self->last == del) self->last = node, node = NULL; \
        i_VALDEL(&del->value); c_free(del); \
        return node; \
    } \
\
    STC_DEF size_t \
    cx_memb(_remove)(Self* self, i_VALRAW val) { \
        size_t n = 0; \
        cx_node_t* prev = self->last, *node; \
        while (prev) { \
            node = prev->next; \
            i_VALRAW r = i_VALTO(&node->value); \
            if (i_CMP(&r, &val) == 0) \
                prev = cx_memb(_erase_after_)(self, prev), ++n; \
            else \
                prev = (node == self->last ? NULL : node); \
        } \
        return n; \
    } \
\
    STC_DEF cx_iter_t \
    cx_memb(_splice)(Self* self, cx_iter_t it, Self* other) { \
        if (!self->last) \
            self->last = other->last; \
        else if (other->last) { \
            cx_node_t *p = it.ref ? it.prev : self->last, *next = p->next; \
            it.prev = other->last; \
            p->next = it.prev->next; \
            it.prev->next = next; \
            if (!it.ref) self->last = it.prev; \
        } \
        other->last = NULL; return it; \
    } \
\
    STC_DEF Self \
    cx_memb(_split_off)(Self* self, cx_iter_t it1, cx_iter_t it2) { \
        Self cx = {NULL}; \
        if (it1.ref == it2.ref) return cx; \
        cx_node_t *p1 = it1.prev, \
                    *p2 = it2.ref ? it2.prev : self->last; \
        p1->next = p2->next, p2->next = _clist_node(Self, it1.ref); \
        if (self->last == p2) self->last = (p1 == p2) ? NULL : p1; \
        cx.last = p2; \
        return cx; \
    } \
\
    STC_DEF int \
    cx_memb(_sort_cmp_)(const clist_VOID_node_t* x, const clist_VOID_node_t* y) { \
        i_VALRAW a = i_VALTO(&((const cx_node_t *) x)->value); \
        i_VALRAW b = i_VALTO(&((const cx_node_t *) y)->value); \
        return i_CMP(&a, &b); \
    } \
    STC_DEF void \
    cx_memb(_sort)(Self* self) { \
        if (self->last) \
            self->last = (cx_node_t *) _clist_mergesort((clist_VOID_node_t *) self->last->next, cx_memb(_sort_cmp_)); \
    }


#define _c_clist_insert_after(self, Self, node, val) \
    cx_node_t *entry = c_new (cx_node_t); \
    if (node) entry->next = node->next, node->next = entry; \
    else      entry->next = entry; \
    entry->value = val
    /* +: set self->last based on node */

STC_DEF size_t
_clist_count(const clist_VOID* self) {
    const clist_VOID_node_t *node = self->last;
    if (!node) return 0;
    size_t n = 1;
    while ((node = node->next) != self->last) ++n;
    return n;
}

/* Singly linked list Mergesort implementation by Simon Tatham. O(n*log n).
 * https://www.chiark.greenend.org.uk/~sgtatham/algorithms/listsort.html
 */
STC_DEF clist_VOID_node_t *
_clist_mergesort(clist_VOID_node_t *list, int (*cmp)(const clist_VOID_node_t*, const clist_VOID_node_t*)) {
    clist_VOID_node_t *p, *q, *e, *tail, *oldhead;
    int insize = 1, nmerges, psize, qsize, i;

    while (1) {
        p = oldhead = list;
        list = tail = NULL;
        nmerges = 0;

        while (p) {
            ++nmerges;
            q = p, psize = 0;
            for (i = 0; i < insize; ++i) {
                ++psize;
                q = (q->next == oldhead ? NULL : q->next);
                if (!q) break;
            }
            qsize = insize;

            while (psize > 0 || (qsize > 0 && q)) {
                if (psize == 0) {
                    e = q, q = q->next, --qsize;
                    if (q == oldhead) q = NULL;
                } else if (qsize == 0 || !q) {
                    e = p, p = p->next, --psize;
                    if (p == oldhead) p = NULL;
                } else if (cmp(p, q) <= 0) {
                    e = p, p = p->next, --psize;
                    if (p == oldhead) p = NULL;
                } else {
                    e = q, q = q->next, --qsize;
                    if (q == oldhead) q = NULL;
                }
                if (tail) tail->next = e; else list = e;
                tail = e;
            }
            p = q;
        }
        tail->next = list;

        if (nmerges <= 1)
            return tail;

        insize *= 2;
    }
}


#endif
#endif
