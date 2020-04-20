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
#ifndef CSLIST__H__
#define CSLIST__H__

#include "cdefs.h"

/* Circular Singly-linked Lists */


#define declare_CSList(...)    c_MACRO_OVERLOAD(declare_CSList, __VA_ARGS__)

#define declare_CSList_2(tag, Value) \
                               declare_CSList_3(tag, Value, c_defaultDestroy)
#define declare_CSList_3(tag, Value, valueDestroy) \
                               declare_CSList_4(tag, Value, valueDestroy, c_defaultCompare)
#define declare_CSList_4(tag, Value, valueDestroy, valueCompare) \
                               declare_CSList_6(tag, Value, valueDestroy, valueCompare, Value, c_defaultGetRaw)
#define declare_CSList_string(tag) \
                               declare_CSList_6(tag, CString, cstring_destroy, cstring_compareRaw, const char*, cstring_getRaw)                            


#define declare_CSListTypes(tag, Value) \
    c_struct (CSListNode_##tag) { \
        CSListNode_##tag *next; \
        Value value; \
    }; \
 \
    c_struct (CSList_##tag) { \
        CSListNode_##tag* last; \
    }; \
 \
    c_struct (cslist_##tag##_iter_t) { \
        CSListNode_##tag *item, **_last; \
    }


#define cslist_init          {NULL}
#define cslist_front(list)   (list).last->next->value
#define cslist_back(list)    (list).last->value
#define cslist_empty(list)   ((list).last == NULL)

#define declare_CSList_6(tag, Value, valueDestroy, valueCompare, ValueRaw, valueGetRaw) \
 \
    declare_CSListTypes(tag, Value); \
 \
    static inline void \
    cslist_##tag##_pushFront(CSList_##tag* self, Value value) { \
        _cslist_insertAfter(tag, self->last, value); \
         if (!self->last) self->last = entry; \
    } \
    static inline void \
    cslist_##tag##_pushBack(CSList_##tag* self, Value value) { \
        _cslist_insertAfter(tag, self->last, value); \
        self->last = entry; \
    } \
    static inline void \
    cslist_##tag##_insertAfter(CSList_##tag* self, cslist_##tag##_iter_t pos, Value value) { \
        _cslist_insertAfter(tag, pos.item, value); \
        if (!self->last || pos.item == self->last) self->last = entry; \
    } \
    static inline void \
    cslist_##tag##_eraseAfter(CSList_##tag* self, cslist_##tag##_iter_t pos) { \
        _cslist_eraseAfter(tag, pos.item, valueDestroy); \
    } \
 \
    static inline void \
    cslist_##tag##_popFront(CSList_##tag* self) { \
        _cslist_eraseAfter(tag, self->last, valueDestroy); \
    } \
 \
    static inline void \
    cslist_##tag##_destroy(CSList_##tag* self) { \
        while (self->last) \
            cslist_##tag##_popFront(self); \
    } \
 \
    static inline int \
    cslist_##tag##_sortCmp(const void* x, const void* y) { \
        CSListNode_##tag *a = (CSListNode_##tag *)x, *b = (CSListNode_##tag *)y; \
        return valueCompare(valueGetRaw(&a->value), valueGetRaw(&b->value)); \
    } \
     \
    static inline void \
    cslist_##tag##_sort(CSList_##tag* self) { \
        CSListNode__base* last = cslist_mergesort((CSListNode__base *) self->last, cslist_##tag##_sortCmp); \
        self->last = (CSListNode_##tag *) last; \
    } \
 \
    static inline cslist_##tag##_iter_t \
    cslist_##tag##_begin(CSList_##tag* lst) { \
        CSListNode_##tag *head = lst->last ? lst->last->next : NULL; \
        return (cslist_##tag##_iter_t) {head, &lst->last}; \
    } \
 \
    static inline cslist_##tag##_iter_t \
    cslist_##tag##_next(cslist_##tag##_iter_t it) { \
        it.item = it.item == *it._last ? NULL : it.item->next; \
        return it; \
    } \
 \
    static inline int \
    cslist_##tag##_remove(CSList_##tag* self, ValueRaw val) { \
        cslist_##tag##_iter_t prev = {self->last}; int n = 0; \
        c_foreach (i, cslist_##tag, *self) { \
            if (valueCompare(valueGetRaw(&i.item->value), &val) == 0) { \
                cslist_##tag##_eraseAfter(self, prev), ++n; \
                if (prev.item == i.item) break; \
            } \
            prev = i; \
        } \
        return n; \
    } \
 \
    typedef Value cslist_##tag##_value_t
    

#define _cslist_insertAfter(tag, node, val) \
    CSListNode_##tag *entry = c_new_1(CSListNode_##tag), \
                       *next = self->last ? node->next : entry; \
    entry->value = val; \
    entry->next = next; \
    if (node) node->next = entry \
    /* +: set self->last based on node */


#define _cslist_eraseAfter(tag, node, valueDestroy) \
    CSListNode_##tag* del = node->next, *next = del->next; \
    node->next = next; \
    if (del == next) self->last = NULL; \
    else if (self->last == del) self->last = node; \
    valueDestroy(&del->value); \
    free(del)


declare_CSListTypes(_base, int);

/* 
 * Singly linked list Mergesort implementation by Simon Tatham. O(n*log(n)).
 * https://www.chiark.greenend.org.uk/~sgtatham/algorithms/listsort.html
 */
static CSListNode__base *
cslist_mergesort(CSListNode__base *list, int (*cmp)(const void*, const void*)) {
    CSListNode__base *p, *q, *e, *tail, *oldhead;
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

#endif
