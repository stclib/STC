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

#include "cdefs.h"

/* Circular Singly-linked Lists */

#define clist_init          {NULL, 0}
#define clist_size(list)    ((size_t) (list).size)

#define declare_CList(...)  c_MACRO_OVERLOAD(declare_CList, __VA_ARGS__)

#define declare_CList_2(tag, Value) \
                            declare_CList_3(tag, Value, c_defaultDestroy)
#define declare_CList_3(tag, Value, valueDestroy) \
                            declare_CList_4(tag, Value, valueDestroy, c_defaultCompare)
#define declare_CList_4(tag, Value, valueDestroy, valueCompare) \
                            declare_CList_6(tag, Value, valueDestroy, valueCompare, Value, c_defaultGetRaw)
#define declare_CList_string(tag) \
                            declare_CList_6(tag, CString, cstring_destroy, cstring_compareRaw, const char*, cstring_getRaw)                            


#define declare_CListTypes(tag, Value) \
    c_struct (CListNode_##tag) { \
        CListNode_##tag *next; \
        Value value; \
    }; \
 \
    c_struct (CList_##tag) { \
        CListNode_##tag* last; \
        size_t size; \
    }; \
 \
    c_struct (clist_##tag##_iter_t) { \
        CListNode_##tag *item, *head; \
    }


#define declare_CList_6(tag, Value, valueDestroy, valueCompare, ValueRaw, valueGetRaw) \
 \
    declare_CListTypes(tag, Value); \
 \
    static inline void \
    clist_##tag##_pushFront(CList_##tag* self, Value value) { \
        _clist_pushAfter(tag, self->last, value); \
         if (!self->last) self->last = entry; \
    } \
    static inline void \
    clist_##tag##_pushBack(CList_##tag* self, Value value) { \
        _clist_pushAfter(tag, self->last, value); \
        self->last = entry; \
    } \
    static inline void \
    clist_##tag##_pushAfter(CList_##tag* self, clist_##tag##_iter_t pos, Value value) { \
        _clist_pushAfter(tag, pos.item, value); \
        if (!self->last || pos.item == self->last) self->last = entry; \
    } \
 \
    static inline void \
    clist_##tag##_popFront(CList_##tag* self) { \
        CListNode_##tag* del = self->last->next, *next = del->next; \
        --self->size; \
        if (next == del) self->last = NULL; \
        else self->last->next = next; \
        valueDestroy(&del->value); \
        free(del); \
    } \
 \
    static inline void \
    clist_##tag##_destroy(CList_##tag* self) { \
        while (clist_size(*self)) \
            clist_##tag##_popFront(self); \
    } \
 \
    static inline clist_##tag##_iter_t \
    clist_##tag##_begin(CList_##tag lst) { \
        CListNode_##tag *head = lst.last ? lst.last->next : NULL; \
        return (clist_##tag##_iter_t) {head, head}; \
    } \
 \
    static inline clist_##tag##_iter_t \
    clist_##tag##_next(clist_##tag##_iter_t it) { \
        CListNode_##tag *next = it.item->next; \
        it.item = next != it.head ? next : NULL; \
        return it; \
    } \
 \
    static inline clist_##tag##_iter_t \
    clist_##tag##_end(CList_##tag lst) { \
        return (clist_##tag##_iter_t) {NULL}; \
    } \
 \
    static inline int \
    clist_##tag##_sortCompare(const void* x, const void* y) { \
        CListNode_##tag *a = (CListNode_##tag *)x, *b = (CListNode_##tag *)y; \
        return valueCompare(valueGetRaw(&a->value), valueGetRaw(&b->value)); \
    } \
     \
    static inline void \
    clist_##tag##_sort(CList_##tag* self) { \
        CListNode__i* last = clist_sort((CListNode__i *) self->last, clist_##tag##_sortCompare); \
        self->last = (CListNode_##tag *) last; \
    } \
 \
    typedef Value clist_##tag##_value_t
    

#define _clist_pushAfter(tag, node, val) \
    CListNode_##tag *entry = c_new_1(CListNode_##tag), \
                    *next = self->last ? node->next : entry; \
    entry->value = val; \
    entry->next = next; \
    ++self->size; \
    if (node) node->next = entry


declare_CListTypes(_i, int);

/* 
 * Singly linked list Mergesort implementation by Simon Tatham. O(n*log(n)).
 * https://www.chiark.greenend.org.uk/~sgtatham/algorithms/listsort.html
 */
static CListNode__i *
clist_sort(CListNode__i *list, int (*cmp)(const void*, const void*)) {
    CListNode__i *p, *q, *e, *tail, *oldhead;
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
