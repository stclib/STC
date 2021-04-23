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
        clist_ix list = clist_ix_init();
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
        clist_ix_del(&list);
    }
*/
#include <stc/ccommon.h>
#include <stdlib.h>

#define using_clist(...) c_MACRO_OVERLOAD(using_clist, __VA_ARGS__)

#define using_clist_2(X, Value) \
            using_clist_3(X, Value, c_default_compare)
#define using_clist_3(X, Value, valueCompare) \
            using_clist_5(X, Value, valueCompare, c_trivial_del, c_trivial_fromraw)
#define using_clist_4(X, Value, valueCompare, valueDel) \
            using_clist_5(X, Value, valueCompare, valueDel, c_no_clone)
#define using_clist_5(X, Value, valueCompare, valueDel, valueClone) \
            _c_using_clist(clist_##X, Value, valueCompare, valueDel, valueClone, c_trivial_toraw, Value)
#define using_clist_7(X, Value, valueCompare, valueDel, valueFromRaw, valueToRaw, RawValue) \
            _c_using_clist(clist_##X, Value, valueCompare, valueDel, valueFromRaw, valueToRaw, RawValue)

#define using_clist_str() \
            _c_using_clist(clist_str, cstr_t, cstr_compare_raw, cstr_del, cstr_from, cstr_c_str, const char*)


#define _c_using_clist_types(CX, Value) \
    typedef Value CX##_value_t; \
\
    typedef struct CX##_node { \
        struct CX##_node* next; \
        CX##_value_t value; \
    } CX##_node_t; \
\
    typedef struct { \
        CX##_node_t* last; \
    } CX; \
\
    typedef struct { \
        CX##_node_t* const* _last; \
        CX##_value_t* ref; \
        int _state; \
    } CX##_iter_t


_c_using_clist_types(clist_VOID, int);
STC_API size_t _clist_count(const clist_VOID* self);
#define _clist_node(CX, vp) c_container_of(vp, CX##_node_t, value)

#define _c_using_clist(CX, Value, valueCompareRaw, valueDel, valueFromRaw, valueToRaw, RawValue) \
\
    _c_using_clist_types(CX, Value); \
    typedef RawValue CX##_rawvalue_t; \
\
    STC_API CX          CX##_clone(CX lst); \
    STC_API void        CX##_del(CX* self); \
    STC_API void        CX##_push_back(CX* self, Value value); \
    STC_API void        CX##_push_front(CX* self, Value value); \
    STC_API void        CX##_emplace_n(CX *self, const CX##_rawvalue_t arr[], size_t n); \
    STC_API CX          CX##_split_after(CX* self, CX##_iter_t pos1, CX##_iter_t pos2); \
    STC_API void        CX##_splice_after(CX* self, CX##_iter_t pos, CX* other); \
    STC_DEF void        CX##_splice_after_range(CX* self, CX##_iter_t pos, CX* other, CX##_iter_t i1, CX##_iter_t i2); \
    STC_API CX##_iter_t CX##_find_before_in(const CX* self, CX##_iter_t first, CX##_iter_t finish, RawValue val); \
    STC_API CX##_iter_t CX##_find_before(const CX* self, RawValue val); \
    STC_API CX##_iter_t CX##_find(const CX* self, RawValue val); \
    STC_API void        CX##_sort(CX* self); \
    STC_API size_t      CX##_remove(CX* self, RawValue val); \
    STC_API CX##_iter_t CX##_insert_after(CX* self, CX##_iter_t pos, Value value); \
    STC_API CX##_iter_t CX##_erase_after(CX* self, CX##_iter_t pos); \
    STC_API CX##_iter_t CX##_erase_range_after(CX* self, CX##_iter_t pos, CX##_iter_t finish); \
    STC_API CX##_node_t* CX##_erase_after_(CX* self, CX##_node_t* node); \
\
    STC_INLINE CX       CX##_init(void) {CX lst = {NULL}; return lst;} \
    STC_INLINE bool     CX##_empty(CX lst) {return lst.last == NULL;} \
    STC_INLINE size_t   CX##_count(CX lst) {return _clist_count((const clist_VOID*) &lst);} \
    STC_INLINE Value    CX##_value_fromraw(RawValue raw) {return valueFromRaw(raw);} \
    STC_INLINE Value    CX##_value_clone(Value val) {return valueFromRaw(valueToRaw(&val));} \
    STC_INLINE void     CX##_clear(CX* self) {CX##_del(self);} \
    STC_INLINE void     CX##_emplace_back(CX* self, RawValue raw) \
                            {CX##_push_back(self, valueFromRaw(raw));} \
    STC_INLINE void     CX##_emplace_front(CX* self, RawValue raw) \
                            {CX##_push_front(self, valueFromRaw(raw));} \
    STC_INLINE CX##_value_t* \
                        CX##_front(const CX* self) {return &self->last->next->value;} \
    STC_INLINE CX##_value_t* \
                        CX##_back(const CX* self) {return &self->last->value;} \
    STC_INLINE void     CX##_pop_front(CX* self) {CX##_erase_after_(self, self->last);} \
    STC_INLINE void     CX##_splice_front(CX* self, CX* other) \
                            {CX##_splice_after(self, CX##_before_begin(self), other);} \
    STC_INLINE void     CX##_splice_back(CX* self, CX* other) \
                            {CX##_splice_after(self, CX##_last(self), other);} \
\
    STC_INLINE CX##_iter_t \
    CX##_emplace_after(CX* self, CX##_iter_t pos, RawValue raw) { \
        return CX##_insert_after(self, pos, valueFromRaw(raw)); \
    } \
\
    STC_INLINE CX##_iter_t \
    CX##_before_begin(const CX* self) { \
        CX##_value_t *last = self->last ? &self->last->value : NULL; \
        CX##_iter_t it = {&self->last, last, -1}; return it; \
    } \
\
    STC_INLINE CX##_iter_t \
    CX##_begin(const CX* self) { \
        CX##_value_t* head = self->last ? &self->last->next->value : NULL; \
        CX##_iter_t it = {&self->last, head, 0}; return it; \
    } \
\
    STC_INLINE CX##_iter_t \
    CX##_last(const CX* self) { \
        CX##_value_t *last = self->last ? &self->last->value : NULL; \
        CX##_iter_t it = {&self->last, last, 0}; return it; \
    } \
\
    STC_INLINE CX##_iter_t \
    CX##_end(const CX* self) { \
        CX##_iter_t it = {NULL, NULL}; return it; \
    } \
\
    STC_INLINE void \
    CX##_next(CX##_iter_t* it) { \
        CX##_node_t* node = _clist_node(CX, it->ref); \
        it->ref = ((it->_state += node == *it->_last) == 1) ? NULL : &node->next->value; \
    } \
\
    STC_INLINE CX##_iter_t \
    CX##_fwd(CX##_iter_t it, size_t n) { \
        while (n-- && it.ref) CX##_next(&it); return it; \
    } \
 \
    _c_implement_clist(CX, Value, valueCompareRaw, valueDel, valueFromRaw, valueToRaw, RawValue) \
    struct stc_trailing_semicolon

/* -------------------------- IMPLEMENTATION ------------------------- */

#if !defined(STC_HEADER) || defined(STC_IMPLEMENTATION)
#define _c_implement_clist(CX, Value, valueCompareRaw, valueDel, valueFromRaw, valueToRaw, RawValue) \
\
    STC_DEF CX \
    CX##_clone(CX lst) { \
        CX out = CX##_init(); \
        c_foreach_3 (i, CX, lst) \
            CX##_emplace_back(&out, valueToRaw(i.ref)); \
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
    STC_DEF void \
    CX##_push_front(CX* self, Value value) { \
        _c_clist_insert_after(self, CX, self->last, value); \
        if (!self->last) self->last = entry; \
    } \
\
    STC_DEF void \
    CX##_emplace_n(CX *self, const CX##_rawvalue_t arr[], size_t n) { \
        for (size_t i=0; i<n; ++i) CX##_push_back(self, valueFromRaw(arr[i])); \
    } \
\
    STC_DEF CX##_iter_t \
    CX##_insert_after(CX* self, CX##_iter_t pos, Value value) { \
        CX##_node_t* node = pos.ref ? _clist_node(CX, pos.ref) : NULL; \
        _c_clist_insert_after(self, CX, node, value); \
        if (!node || node == self->last && pos._state == 0) self->last = entry; \
        pos.ref = &entry->value, pos._state = 0; return pos; \
    } \
\
    STC_DEF CX##_iter_t \
    CX##_erase_after(CX* self, CX##_iter_t pos) { \
        CX##_erase_after_(self, _clist_node(CX, pos.ref)); \
        CX##_next(&pos); return pos; \
    } \
\
    STC_DEF CX##_iter_t \
    CX##_erase_range_after(CX* self, CX##_iter_t first, CX##_iter_t finish) { \
        CX##_node_t* node = _clist_node(CX, first.ref), *done = finish.ref ? _clist_node(CX, finish.ref) : NULL; \
        while (node && node->next != done) \
            node = CX##_erase_after_(self, node); \
        CX##_next(&first); return first; \
    } \
\
    STC_DEF CX##_iter_t \
    CX##_find_before_in(const CX* self, CX##_iter_t first, CX##_iter_t finish, RawValue val) { \
        CX##_iter_t i = first; \
        for (CX##_next(&i); i.ref != finish.ref; CX##_next(&i)) { \
            RawValue r = valueToRaw(i.ref); \
            if (valueCompareRaw(&r, &val) == 0) return first; \
            first = i; \
        } \
        return CX##_end(self); \
    } \
\
    STC_DEF CX##_iter_t \
    CX##_find_before(const CX* self, RawValue val) { \
        CX##_iter_t it = CX##_find_before_in(self, CX##_before_begin(self), CX##_end(self), val); \
        return it; \
    } \
\
    STC_DEF CX##_iter_t \
    CX##_find(const CX* self, RawValue val) { \
        CX##_iter_t it = CX##_find_before_in(self, CX##_before_begin(self), CX##_end(self), val); \
        if (it.ref != CX##_end(self).ref) CX##_next(&it); \
        return it; \
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
    STC_DEF CX \
    CX##_split_after(CX* self, CX##_iter_t pos1, CX##_iter_t pos2) { \
        CX##_node_t *node1 = _clist_node(CX, pos1.ref), *next1 = node1->next, \
                    *node2 = _clist_node(CX, pos2.ref); \
        node1->next = node2->next, node2->next = next1; \
        if (self->last == node2) self->last = node1; \
        CX lst = {node2}; return lst; \
    } \
\
    STC_DEF void \
    CX##_splice_after(CX* self, CX##_iter_t pos, CX* other) { \
        if (!pos.ref) \
            self->last = other->last; \
        else if (other->last) { \
            CX##_node_t *node = _clist_node(CX, pos.ref), *next = node->next; \
            node->next = other->last->next; \
            other->last->next = next; \
            if (node == self->last && pos._state == 0) self->last = other->last; \
        } \
        other->last = NULL; \
    } \
\
    STC_DEF void \
    CX##_splice_after_range(CX* self, CX##_iter_t pos, CX* other, CX##_iter_t pos1, CX##_iter_t pos2) { \
        CX tmp = CX##_split_after(other, pos1, pos2); \
        CX##_splice_after(self, pos, &tmp); \
    } \
\
    STC_DEF int \
    CX##_sort_cmp_(const void* x, const void* y) { \
        RawValue a = valueToRaw(&((CX##_node_t *) x)->value); \
        RawValue b = valueToRaw(&((CX##_node_t *) y)->value); \
        return valueCompareRaw(&a, &b); \
    } \
\
    STC_DEF void \
    CX##_sort(CX* self) { \
        if (self->last) \
            self->last = (CX##_node_t *) _clist_mergesort((clist_VOID_node_t *) self->last->next, CX##_sort_cmp_); \
    }


#define _c_clist_insert_after(self, CX, node, val) \
    CX##_node_t *entry = c_new_1 (CX##_node_t); \
    if (node) entry->next = node->next, node->next = entry; \
    else      entry->next = entry; \
    entry->value = val
    /* +: set self->last based on node */

STC_DEF size_t
_clist_count(const clist_VOID* self) {
    const clist_VOID_node_t *nd = self->last;
    if (!nd) return 0;
    size_t n = 1;
    while ((nd = nd->next) != self->last) ++n;
    return n;
}

/* Singly linked list Mergesort implementation by Simon Tatham. O(n*log n).
 * https://www.chiark.greenend.org.uk/~sgtatham/algorithms/listsort.html
 */
STC_DEF clist_VOID_node_t *
_clist_mergesort(clist_VOID_node_t *list, int (*cmp)(const void*, const void*)) {
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
