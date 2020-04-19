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
#ifndef CFWDLIST__H__
#define CFWDLIST__H__

#include "cdefs.h"

/* Circular Singly-linked Lists */

#define cfwdlist_init          {NULL}

#define declare_CFwdList(...)  c_MACRO_OVERLOAD(declare_CFwdList, __VA_ARGS__)

#define declare_CFwdList_2(tag, Value) \
                               declare_CFwdList_3(tag, Value, c_defaultDestroy)
#define declare_CFwdList_3(tag, Value, valueDestroy) \
                               declare_CFwdList_4(tag, Value, valueDestroy, c_defaultCompare)
#define declare_CFwdList_4(tag, Value, valueDestroy, valueCompare) \
                               declare_CFwdList_6(tag, Value, valueDestroy, valueCompare, Value, c_defaultGetRaw)
#define declare_CFwdList_string(tag) \
                               declare_CFwdList_6(tag, CString, cstring_destroy, cstring_compareRaw, const char*, cstring_getRaw)                            


#define declare_CFwdListTypes(tag, Value) \
    c_struct (CFwdListNode_##tag) { \
        CFwdListNode_##tag *next; \
        Value value; \
    }; \
 \
    c_struct (CFwdList_##tag) { \
        CFwdListNode_##tag* last; \
    }; \
 \
    c_struct (cfwdlist_##tag##_iter_t) { \
        CFwdListNode_##tag *item, *head; \
    }


#define declare_CFwdList_6(tag, Value, valueDestroy, valueCompare, ValueRaw, valueGetRaw) \
 \
    declare_CFwdListTypes(tag, Value); \
 \
    static inline void \
    cfwdlist_##tag##_pushFront(CFwdList_##tag* self, Value value) { \
        _cfwdlist_insertAfter(tag, self->last, value); \
         if (!self->last) self->last = entry; \
    } \
    static inline void \
    cfwdlist_##tag##_pushBack(CFwdList_##tag* self, Value value) { \
        _cfwdlist_insertAfter(tag, self->last, value); \
        self->last = entry; \
    } \
    static inline void \
    cfwdlist_##tag##_insertAfter(CFwdList_##tag* self, cfwdlist_##tag##_iter_t pos, Value value) { \
        _cfwdlist_insertAfter(tag, pos.item, value); \
        if (!self->last || pos.item == self->last) self->last = entry; \
    } \
    static inline void \
    cfwdlist_##tag##_eraseAfter(CFwdList_##tag* self, cfwdlist_##tag##_iter_t pos) { \
        _cfwdlist_eraseAfter(tag, pos.item, valueDestroy); \
    } \
 \
    static inline void \
    cfwdlist_##tag##_popFront(CFwdList_##tag* self) { \
        _cfwdlist_eraseAfter(tag, self->last, valueDestroy); \
    } \
 \
    static inline void \
    cfwdlist_##tag##_destroy(CFwdList_##tag* self) { \
        while (self->last) \
            cfwdlist_##tag##_popFront(self); \
    } \
 \
    static inline cfwdlist_##tag##_iter_t \
    cfwdlist_##tag##_begin(CFwdList_##tag lst) { \
        CFwdListNode_##tag *head = lst.last ? lst.last->next : NULL; \
        return (cfwdlist_##tag##_iter_t) {head, head}; \
    } \
 \
    static inline cfwdlist_##tag##_iter_t \
    cfwdlist_##tag##_next(cfwdlist_##tag##_iter_t it) { \
        CFwdListNode_##tag *next = it.item->next; \
        it.item = next != it.head ? next : NULL; \
        return it; \
    } \
 \
    static inline cfwdlist_##tag##_iter_t \
    cfwdlist_##tag##_end(CFwdList_##tag lst) { \
        return (cfwdlist_##tag##_iter_t) {NULL}; \
    } \
 \
    static inline int \
    cfwdlist_##tag##_sortCmp(const void* x, const void* y) { \
        CFwdListNode_##tag *a = (CFwdListNode_##tag *)x, *b = (CFwdListNode_##tag *)y; \
        return valueCompare(valueGetRaw(&a->value), valueGetRaw(&b->value)); \
    } \
     \
    static inline void \
    cfwdlist_##tag##_sort(CFwdList_##tag* self) { \
        CFwdListNode__base* last = cfwdlist_sort_base((CFwdListNode__base *) self->last, cfwdlist_##tag##_sortCmp); \
        self->last = (CFwdListNode_##tag *) last; \
    } \
 \
    typedef Value cfwdlist_##tag##_value_t
    

#define _cfwdlist_insertAfter(tag, node, val) \
    CFwdListNode_##tag *entry = c_new_1(CFwdListNode_##tag), \
                       *next = self->last ? node->next : entry; \
    entry->value = val; \
    entry->next = next; \
    if (node) node->next = entry \
    /* +: set self->last based on node */


#define _cfwdlist_eraseAfter(tag, node, valueDestroy) \
    CFwdListNode_##tag* del = node->next, *next = del->next; \
    node->next = next; \
    if (del == next) self->last = NULL; \
    else if (self->last == del) self->last = node; \
    valueDestroy(&del->value); \
    free(del)


declare_CFwdListTypes(_base, int);

/* 
 * Singly linked list Mergesort implementation by Simon Tatham. O(n*log(n)).
 * https://www.chiark.greenend.org.uk/~sgtatham/algorithms/listsort.html
 */
static CFwdListNode__base *
cfwdlist_sort_base(CFwdListNode__base *list, int (*cmp)(const void*, const void*)) {
    CFwdListNode__base *p, *q, *e, *tail, *oldhead;
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
