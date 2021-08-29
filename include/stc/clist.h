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

#define forward_clist(X, Value) _c_clist_types(clist_##X, Value)

#define using_clist(...)   c_MACRO_OVERLOAD(using_clist, __VA_ARGS__)

#define using_clist_2(X, Value) \
            using_clist_3(X, Value, c_default_compare)
#define using_clist_3(X, Value, valueCompare) \
            using_clist_5(X, Value, valueCompare, c_default_del, c_default_fromraw)
#define using_clist_4(X, Value, valueCompare, valueDel) \
            using_clist_5(X, Value, valueCompare, valueDel, c_no_clone)
#define using_clist_5(X, Value, valueCompare, valueDel, valueClone) \
            using_clist_7(X, Value, valueCompare, valueDel, valueClone, c_default_toraw, Value)
#define using_clist_7(X, Value, valueCompareRaw, valueDel, valueFromRaw, valueToRaw, RawValue) \
            _c_using_clist(clist_##X, Value, valueCompareRaw, valueDel, valueFromRaw, valueToRaw, RawValue, c_true)
#define using_clist_8(X, Value, valueCompareRaw, valueDel, valueFromRaw, valueToRaw, RawValue, defTypes) \
            _c_using_clist(clist_##X, Value, valueCompareRaw, valueDel, valueFromRaw, valueToRaw, RawValue, defTypes)

#define using_clist_str() \
            using_clist_7(str, cstr, c_rawstr_compare, cstr_del, cstr_from, cstr_str, const char*)


#define _c_clist_types(CX, Value) \
    typedef Value CX##_value_t; \
    typedef struct CX##_node_t CX##_node_t; \
\
    typedef struct { \
        CX##_node_t *const *_last, *prev; \
        CX##_value_t *ref; \
    } CX##_iter_t; \
\
    typedef struct { \
        CX##_node_t *last; \
    } CX

#define _c_clist_complete_types(CX) \
    struct CX##_node_t { \
        struct CX##_node_t *next; \
        CX##_value_t value; \
    }

_c_clist_types(clist_VOID, int);
_c_clist_complete_types(clist_VOID);
STC_API size_t _clist_count(const clist_VOID* self);
#define _clist_node(CX, vp) c_container_of(vp, CX##_node_t, value)


#define _c_using_clist(CX, Value, valueCompareRaw, valueDel, valueFromRaw, valueToRaw, RawValue, defTypes) \
\
    defTypes( _c_clist_types(CX, Value); ) \
    _c_clist_complete_types(CX); \
    typedef RawValue CX##_rawvalue_t; \
\
    STC_API CX              CX##_clone(CX cx); \
    STC_API void            CX##_del(CX* self); \
    STC_API void            CX##_push_back(CX* self, Value value); \
    STC_API void            CX##_push_front(CX* self, Value value); \
    STC_API CX##_iter_t     CX##_insert(CX* self, CX##_iter_t it, Value value); \
    STC_API void            CX##_emplace_items(CX *self, const CX##_rawvalue_t arr[], size_t n); \
    STC_API CX##_iter_t     CX##_erase_at(CX* self, CX##_iter_t it); \
    STC_API CX##_iter_t     CX##_erase_range(CX* self, CX##_iter_t it1, CX##_iter_t it2); \
    STC_API size_t          CX##_remove(CX* self, RawValue val); \
    STC_API CX##_iter_t     CX##_splice(CX* self, CX##_iter_t it, CX* other); \
    STC_API CX              CX##_split_off(CX* self, CX##_iter_t it1, CX##_iter_t it2); \
    STC_API void            CX##_sort(CX* self); \
    STC_API CX##_iter_t     CX##_find_in(CX##_iter_t it1, CX##_iter_t it2, RawValue val); \
    STC_API CX##_node_t*    CX##_erase_after_(CX* self, CX##_node_t* node); \
\
    STC_INLINE CX           CX##_init(void) {return c_make(CX){NULL};} \
    STC_INLINE bool         CX##_empty(CX cx) {return cx.last == NULL;} \
    STC_INLINE size_t       CX##_count(CX cx) \
                                {return _clist_count((const clist_VOID*) &cx);} \
    STC_INLINE void         CX##_clear(CX* self) {CX##_del(self);} \
    STC_INLINE Value        CX##_value_fromraw(RawValue raw) {return valueFromRaw(raw);} \
    STC_INLINE RawValue     CX##_value_toraw(CX##_value_t* pval) {return valueToRaw(pval);} \
    STC_INLINE Value        CX##_value_clone(Value val) \
                                {return valueFromRaw(valueToRaw(&val));} \
    STC_INLINE void         CX##_pop_front(CX* self) \
                                {CX##_erase_after_(self, self->last);} \
    STC_INLINE CX##_iter_t  CX##_erase(CX* self, CX##_iter_t it) \
                                {return CX##_erase_at(self, it);} \
    STC_INLINE void         CX##_emplace_back(CX* self, RawValue raw) \
                                {CX##_push_back(self, valueFromRaw(raw));} \
    STC_INLINE void         CX##_emplace_front(CX* self, RawValue raw) \
                                {CX##_push_front(self, valueFromRaw(raw));} \
    STC_INLINE CX##_iter_t  CX##_emplace(CX* self, CX##_iter_t it, RawValue raw) \
                                {return CX##_insert(self, it, valueFromRaw(raw));} \
    STC_INLINE CX##_value_t* CX##_front(const CX* self) {return &self->last->next->value;} \
    STC_INLINE CX##_value_t* CX##_back(const CX* self) {return &self->last->value;} \
\
    STC_INLINE CX##_iter_t \
    CX##_iter(const CX* self, CX##_node_t* prev) { \
        return c_make(CX##_iter_t){&self->last, prev, &prev->next->value}; \
    } \
\
    STC_INLINE CX##_iter_t \
    CX##_begin(const CX* self) { \
        CX##_value_t* head = self->last ? &self->last->next->value : NULL; \
        return c_make(CX##_iter_t){&self->last, self->last, head}; \
    } \
\
    STC_INLINE CX##_iter_t \
    CX##_end(const CX* self) { \
        return c_make(CX##_iter_t){NULL}; \
    } \
\
    STC_INLINE void \
    CX##_next(CX##_iter_t* it) { \
        CX##_node_t* node = it->prev = _clist_node(CX, it->ref); \
        it->ref = (node == *it->_last ? NULL : &node->next->value); \
    } \
\
    STC_INLINE CX##_iter_t \
    CX##_fwd(CX##_iter_t it, size_t n) { \
        while (n-- && it.ref) CX##_next(&it); \
        return it; \
    } \
\
    STC_INLINE CX##_iter_t \
    CX##_splice_range(CX* self, CX##_iter_t it, \
                      CX* other, CX##_iter_t it1, CX##_iter_t it2) { \
        CX tmp = CX##_split_off(other, it1, it2); \
        return CX##_splice(self, it, &tmp); \
    } \
\
    STC_INLINE CX##_iter_t \
    CX##_find(const CX* self, RawValue val) { \
        return CX##_find_in(CX##_begin(self), CX##_end(self), val); \
    } \
\
    STC_INLINE CX##_value_t* \
    CX##_get(const CX* self, RawValue val) { \
        return CX##_find_in(CX##_begin(self), CX##_end(self), val).ref; \
    } \
\
    _c_implement_clist(CX, Value, valueCompareRaw, valueDel, valueFromRaw, valueToRaw, RawValue) \
    struct stc_trailing_semicolon

/* -------------------------- IMPLEMENTATION ------------------------- */

#if !defined(STC_HEADER) || defined(STC_IMPLEMENTATION)
#define _c_implement_clist(CX, Value, valueCompareRaw, valueDel, valueFromRaw, valueToRaw, RawValue) \
\
    STC_DEF CX \
    CX##_clone(CX cx) { \
        CX out = CX##_init(); \
        c_foreach_3 (it, CX, cx) CX##_emplace_back(&out, valueToRaw(it.ref)); \
        return out; \
    } \
\
    STC_DEF void \
    CX##_del(CX* self) { \
        while (self->last) CX##_erase_after_(self, self->last); \
    } \
\
    STC_DEF void \
    CX##_push_back(CX* self, Value value) { \
        _c_clist_insert_after(self, CX, self->last, value); \
        self->last = entry; \
    } \
\
    STC_DEF void \
    CX##_push_front(CX* self, Value value) { \
        _c_clist_insert_after(self, CX, self->last, value); \
        if (!self->last) self->last = entry; \
    } \
\
    STC_DEF void \
    CX##_emplace_items(CX *self, const CX##_rawvalue_t arr[], size_t n) { \
        for (size_t i=0; i<n; ++i) CX##_push_back(self, valueFromRaw(arr[i])); \
    } \
\
    STC_DEF CX##_iter_t \
    CX##_insert(CX* self, CX##_iter_t it, Value value) { \
        CX##_node_t* node = it.ref ? it.prev : self->last; \
        _c_clist_insert_after(self, CX, node, value); \
        if (!self->last || !it.ref) { \
            it.prev = self->last ? self->last : entry; \
            self->last = entry; \
        } \
        it.ref = &entry->value; \
        return it; \
    } \
\
    STC_DEF CX##_iter_t \
    CX##_erase_at(CX* self, CX##_iter_t it) { \
        CX##_node_t *node = _clist_node(CX, it.ref); \
        it.ref = (node == self->last) ? NULL : &node->next->value; \
        CX##_erase_after_(self, it.prev); \
        return it; \
    } \
\
    STC_DEF CX##_iter_t \
    CX##_erase_range(CX* self, CX##_iter_t it1, CX##_iter_t it2) { \
        CX##_node_t *node = it1.ref ? it1.prev : NULL, \
                    *done = it2.ref ? _clist_node(CX, it2.ref) : NULL; \
        while (node && node->next != done) \
            node = CX##_erase_after_(self, node); \
        return it2; \
    } \
\
    STC_DEF CX##_iter_t \
    CX##_find_in(CX##_iter_t it1, CX##_iter_t it2, RawValue val) { \
        c_foreach_4 (it, CX, it1, it2) { \
            RawValue r = valueToRaw(it.ref); \
            if (valueCompareRaw(&r, &val) == 0) return it; \
        } \
        it2.ref = NULL; return it2; \
    } \
\
    STC_DEF CX##_node_t* \
    CX##_erase_after_(CX* self, CX##_node_t* node) { \
        CX##_node_t* del = node->next, *next = del->next; \
        node->next = next; \
        if (del == next) self->last = node = NULL; \
        else if (self->last == del) self->last = node, node = NULL; \
        valueDel(&del->value); c_free(del); \
        return node; \
    } \
\
    STC_DEF size_t \
    CX##_remove(CX* self, RawValue val) { \
        size_t n = 0; \
        CX##_node_t* prev = self->last, *node; \
        while (prev) { \
            node = prev->next; \
            RawValue r = valueToRaw(&node->value); \
            if (valueCompareRaw(&r, &val) == 0) \
                prev = CX##_erase_after_(self, prev), ++n; \
            else \
                prev = (node == self->last ? NULL : node); \
        } \
        return n; \
    } \
\
    STC_DEF CX##_iter_t \
    CX##_splice(CX* self, CX##_iter_t it, CX* other) { \
        if (!self->last) \
            self->last = other->last; \
        else if (other->last) { \
            CX##_node_t *p = it.ref ? it.prev : self->last, *next = p->next; \
            it.prev = other->last; \
            p->next = it.prev->next; \
            it.prev->next = next; \
            if (!it.ref) self->last = it.prev; \
        } \
        other->last = NULL; return it; \
    } \
\
    STC_DEF CX \
    CX##_split_off(CX* self, CX##_iter_t it1, CX##_iter_t it2) { \
        CX cx = {NULL}; \
        if (it1.ref == it2.ref) return cx; \
        CX##_node_t *p1 = it1.prev, \
                    *p2 = it2.ref ? it2.prev : self->last; \
        p1->next = p2->next, p2->next = _clist_node(CX, it1.ref); \
        if (self->last == p2) self->last = (p1 == p2) ? NULL : p1; \
        cx.last = p2; \
        return cx; \
    } \
\
    STC_DEF int \
    CX##_sort_cmp_(const clist_VOID_node_t* x, const clist_VOID_node_t* y) { \
        RawValue a = valueToRaw(&((const CX##_node_t *) x)->value); \
        RawValue b = valueToRaw(&((const CX##_node_t *) y)->value); \
        return valueCompareRaw(&a, &b); \
    } \
    STC_DEF void \
    CX##_sort(CX* self) { \
        if (self->last) \
            self->last = (CX##_node_t *) _clist_mergesort((clist_VOID_node_t *) self->last->next, CX##_sort_cmp_); \
    }


#define _c_clist_insert_after(self, CX, node, val) \
    CX##_node_t *entry = c_new (CX##_node_t); \
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

#else
#define _c_implement_clist(CX, Value, valueCompareRaw, valueDel, valueFromRaw, valueToRaw, RawValue)
#endif

#endif
