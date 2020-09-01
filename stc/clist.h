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
#ifndef CLIST__H__
#define CLIST__H__

#include <stdlib.h>
#include "cdefs.h"

/*  Circular Singly-linked Lists.
    
    This implements a std::forward_list-like class in C, but because it is circular,
    it also support push* and splice* at both ends of the list. This makes it ideal
    for being used as a queue, unlike std::forward_list. Basic usage is similar to cvec:
   
    #include <stdio.h>
    #include <stc/clist.h>
    #include <stc/crandom.h>
    declare_clist(ix, int64_t);
 
    int main() {
        clist_ix list = clist_init;
        crandom_eng32_t pcg = crandom_eng32_init(12345);
        int n;
        for (int i=0; i<1000000; ++i) // one million
            clist_ix_push_back(&list, crandom_i32(&pcg));
        n = 0; 
        c_foreach (i, clist_ix, list)
            if (++n % 10000 == 0) printf("%8d: %10zd\n", n, i.item->value);
        // Sort them...
        clist_ix_sort(&list); // mergesort O(n*log n)
        n = 0;
        puts("sorted");
        c_foreach (i, clist_ix, list)
            if (++n % 10000 == 0) printf("%8d: %10zd\n", n, i.item->value);
        clist_ix_destroy(&list);
    }
*/

#define declare_clist(...)   c_MACRO_OVERLOAD(declare_clist, __VA_ARGS__)

#define declare_clist_2(tag, Value) \
                             declare_clist_3(tag, Value, c_default_destroy)
#define declare_clist_3(tag, Value, valueDestroy) \
                             declare_clist_4(tag, Value, valueDestroy, c_default_compare)
#define declare_clist_4(tag, Value, valueDestroy, valueCompare) \
                             declare_clist_7(tag, Value, valueDestroy, Value, \
                                             valueCompare, c_default_to_raw, c_default_from_raw)
#define declare_clist_str()  declare_clist_7(str, cstr_t, cstr_destroy, const char*, \
                                             cstr_compare_raw, cstr_to_raw, cstr_make)

#define declare_clist_types(tag, Value) \
    typedef struct clist_##tag##_node { \
        struct clist_##tag##_node *next; \
        Value value; \
    } clist_##tag##_node_t; \
 \
    typedef struct clist_##tag { \
        clist_##tag##_node_t *last; \
    } clist_##tag; \
 \
    typedef struct { \
        clist_##tag##_node_t *item, *end, **_last; \
    } clist_##tag##_iter_t

#define clist_init          {NULL}
#define clist_front(list)   (list).last->next->value
#define clist_back(list)    (list).last->value
#define clist_empty(list)   ((list).last == NULL)


#define declare_clist_7(tag, Value, valueDestroy, RawValue, valueCompareRaw, valueToRaw, valueFromRaw) \
 \
    declare_clist_types(tag, Value); \
    typedef RawValue clist_##tag##_rawvalue_t; \
    typedef clist_##tag##_rawvalue_t clist_##tag##_input_t; \
 \
    STC_INLINE clist_##tag \
    clist_##tag##_init(void) {clist_##tag x = clist_init; return x;} \
    STC_API void \
    clist_##tag##_destroy(clist_##tag* self); \
    STC_INLINE void \
    clist_##tag##_clear(clist_##tag* self) {clist_##tag##_destroy(self);} \
 \
    STC_API void \
    clist_##tag##_push_n(clist_##tag *self, const clist_##tag##_input_t in[], size_t size); \
    STC_API void \
    clist_##tag##_push_back_v(clist_##tag* self, Value value); \
    STC_INLINE void \
    clist_##tag##_push_back(clist_##tag* self, RawValue rawValue) { \
        clist_##tag##_push_back_v(self, valueFromRaw(rawValue)); \
    } \
    STC_API void \
    clist_##tag##_push_front_v(clist_##tag* self, Value value); \
    STC_INLINE void \
    clist_##tag##_push_front(clist_##tag* self, RawValue rawValue) { \
        clist_##tag##_push_front_v(self, valueFromRaw(rawValue)); \
    } \
    STC_API void \
    clist_##tag##_pop_front(clist_##tag* self); \
 \
    STC_INLINE clist_##tag##_iter_t \
    clist_##tag##_before_begin(clist_##tag* self) { \
        clist_##tag##_iter_t it = {self->last, self->last, &self->last}; return it; \
    } \
    STC_INLINE clist_##tag##_iter_t \
    clist_##tag##_begin(clist_##tag* self) { \
        clist_##tag##_node_t *head = self->last ? self->last->next : NULL; \
        clist_##tag##_iter_t it = {head, NULL, &self->last}; return it; \
    } \
    STC_INLINE clist_##tag##_iter_t \
    clist_##tag##_last(clist_##tag* self) { \
        clist_##tag##_iter_t it = {self->last, NULL, &self->last}; return it; \
    } \
    STC_INLINE void \
    clist_##tag##_next(clist_##tag##_iter_t* it) { \
        it->end = it->item == *it->_last ? it->item->next : NULL; \
        it->item = it->item->next; \
    } \
 \
    STC_API clist_##tag##_iter_t \
    clist_##tag##_insert_after_v(clist_##tag* self, clist_##tag##_iter_t pos, Value value); \
    STC_INLINE clist_##tag##_iter_t \
    clist_##tag##_insert_after(clist_##tag* self, clist_##tag##_iter_t pos, RawValue rawValue) { \
        return clist_##tag##_insert_after_v(self, pos, valueFromRaw(rawValue)); \
    } \
    STC_API clist_##tag##_iter_t \
    clist_##tag##_erase_after(clist_##tag* self, clist_##tag##_iter_t pos); \
 \
    STC_INLINE void \
    clist_##tag##_splice_after(clist_##tag* self, clist_##tag##_iter_t pos, clist_##tag* other) { \
        _clist_splice_after((clist_void *) self, *(clist_void_iter_t *) &pos, (clist_void *) other); \
    } \
    STC_INLINE void \
    clist_##tag##_splice_front(clist_##tag* self, clist_##tag* other) { \
        clist_##tag##_splice_after(self, clist_##tag##_before_begin(self), other); \
    } \
    STC_INLINE void \
    clist_##tag##_splice_back(clist_##tag* self, clist_##tag* other) { \
        clist_##tag##_splice_after(self, clist_##tag##_last(self), other); \
    } \
  \
    STC_API clist_##tag##_iter_t \
    clist_##tag##_find_before(clist_##tag* self, clist_##tag##_iter_t prev, RawValue val); \
    STC_API Value* \
    clist_##tag##_find(clist_##tag* self, RawValue val); \
    STC_API size_t \
    clist_##tag##_remove(clist_##tag* self, RawValue val); \
    STC_API void \
    clist_##tag##_sort(clist_##tag* self); \
 \
    STC_INLINE Value* \
    clist_##tag##_front(clist_##tag* self) {return &self->last->next->value;} \
    STC_INLINE Value* \
    clist_##tag##_back(clist_##tag* self) {return &self->last->value;} \
 \
    implement_clist_7(tag, Value, valueDestroy, RawValue, valueCompareRaw, valueToRaw, valueFromRaw) \
    typedef Value clist_##tag##_value_t

    
/* -------------------------- IMPLEMENTATION ------------------------- */

#if !defined(STC_HEADER) || defined(STC_IMPLEMENTATION)
#define implement_clist_7(tag, Value, valueDestroy, RawValue, valueCompareRaw, valueToRaw, valueFromRaw) \
  \
    STC_API void \
    clist_##tag##_destroy(clist_##tag* self) { \
        while (self->last) \
            clist_##tag##_pop_front(self); \
    } \
 \
    STC_API void \
    clist_##tag##_push_back_v(clist_##tag* self, Value value) { \
        _clist_insert_after(self, tag, self->last, value); \
        self->last = entry; \
    } \
    STC_API void \
    clist_##tag##_push_front_v(clist_##tag* self, Value value) { \
        _clist_insert_after(self, tag, self->last, value); \
        if (!self->last) self->last = entry; \
    } \
    STC_API void \
    clist_##tag##_push_n(clist_##tag *self, const clist_##tag##_input_t in[], size_t size) { \
        for (size_t i=0; i<size; ++i) clist_##tag##_push_back_v(self, valueFromRaw(in[i])); \
    } \
    STC_API void \
    clist_##tag##_pop_front(clist_##tag* self) { \
        _clist_erase_after(self, tag, self->last, valueDestroy); \
    } \
 \
    STC_API clist_##tag##_iter_t \
    clist_##tag##_insert_after_v(clist_##tag* self, clist_##tag##_iter_t pos, Value value) { \
        _clist_insert_after(self, tag, pos.item, value); \
        if (pos.item == self->last && pos.item != pos.end) self->last = entry; \
        pos.item = entry; return pos; \
    } \
    STC_API clist_##tag##_iter_t \
    clist_##tag##_erase_after(clist_##tag* self, clist_##tag##_iter_t pos) { \
        _clist_erase_after(self, tag, pos.item, valueDestroy); \
        clist_##tag##_next(&pos); return pos; \
    } \
 \
    STC_API clist_##tag##_iter_t \
    clist_##tag##_find_before(clist_##tag* self, clist_##tag##_iter_t prev, RawValue val) { \
        clist_##tag##_iter_t i = prev; \
        if (i.item) i.item = i.item->next; \
        for (; i.item != i.end; clist_##tag##_next(&i)) { \
            RawValue r = valueToRaw(&i.item->value); \
            if (valueCompareRaw(&r, &val) == 0) \
                return prev; \
            prev = i; \
        } \
        prev.item = prev.end = NULL; \
        return prev; \
    } \
 \
    STC_API Value* \
    clist_##tag##_find(clist_##tag* self, RawValue val) { \
        clist_##tag##_iter_t it = clist_##tag##_find_before(self, clist_##tag##_before_begin(self), val); \
        return it.item ? &it.item->next->value : NULL; \
    } \
 \
    STC_API size_t \
    clist_##tag##_remove(clist_##tag* self, RawValue val) { \
        clist_##tag##_iter_t it; size_t n = 0; \
        while ((it = clist_##tag##_find_before(self, clist_##tag##_before_begin(self), val)).item) \
            it = clist_##tag##_erase_after(self, it), ++n; \
        return n; \
    } \
 \
    static inline int \
    clist_##tag##_sort_compare(const void* x, const void* y) { \
        RawValue a = valueToRaw(&((clist_##tag##_node_t *) x)->value); \
        RawValue b = valueToRaw(&((clist_##tag##_node_t *) y)->value); \
        return valueCompareRaw(&a, &b); \
    } \
    STC_API void \
    clist_##tag##_sort(clist_##tag* self) { \
        clist_void_node_t* last = _clist_mergesort((clist_void_node_t *) self->last->next, clist_##tag##_sort_compare); \
        self->last = (clist_##tag##_node_t *) last; \
    }

#define _clist_insert_after(self, tag, node, val) \
    clist_##tag##_node_t *entry = c_new (clist_##tag##_node_t), \
                         *next = self->last ? node->next : entry; \
    entry->value = val; \
    entry->next = next; \
    if (node) node->next = entry
    /* +: set self->last based on node */

#define _clist_erase_after(self, tag, node, valueDestroy) \
    clist_##tag##_node_t* del = node->next, *next = del->next; \
    node->next = next; \
    if (del == next) self->last = NULL; \
    else if (self->last == del) self->last = node; \
    valueDestroy(&del->value); \
    free(del)


declare_clist_types(void, int);

STC_API void \
_clist_splice_after(clist_void* self, clist_void_iter_t pos, clist_void* other) {
    if (!pos.item)
        self->last = other->last;
    else if (other->last) {
        clist_void_node_t *next = pos.item->next;
        pos.item->next = other->last->next;
        other->last->next = next;
        if (pos.item == self->last && pos.item != pos.end) self->last = other->last;
    }
    other->last = NULL;
}

/* Singly linked list Mergesort implementation by Simon Tatham. O(n*log n).
 * https://www.chiark.greenend.org.uk/~sgtatham/algorithms/listsort.html
 */
STC_API clist_void_node_t *
_clist_mergesort(clist_void_node_t *list, int (*cmp)(const void*, const void*)) {
    clist_void_node_t *p, *q, *e, *tail, *oldhead;
    int insize = 1, nmerges, psize, qsize, i;
    if (!list) return NULL;
    
    while (1) {
        p = list;
        oldhead = list;
        list = tail = NULL;
        nmerges = 0;

        while (p) {
            ++nmerges;
            q = p;
            psize = 0;
            for (i = 0; i < insize; ++i) {
                ++psize;
                q = (q->next == oldhead ? NULL : q->next);
                if (!q) break;
            }
            qsize = insize;

            while (psize > 0 || (qsize > 0 && q)) {
                if (psize == 0) {
                    e = q; q = q->next; --qsize;
                    if (q == oldhead) q = NULL;
                } else if (qsize == 0 || !q) {
                    e = p; p = p->next; --psize;
                    if (p == oldhead) p = NULL;
                } else if (cmp(p, q) <= 0) {
                    e = p; p = p->next; --psize;
                    if (p == oldhead) p = NULL;
                } else {
                    e = q; q = q->next; --qsize;
                    if (q == oldhead) q = NULL;
                }
                if (tail)
                    tail->next = e;
                else
                    list = e;
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
#define implement_clist_7(tag, Value, valueDestroy, RawValue, valueCompareRaw, valueToRaw, valueFromRaw)
#endif

#endif
