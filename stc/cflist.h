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
#ifndef CFLIST__H__
#define CFLIST__H__

#include "cdefs.h"

/*  Circular Singly-linked Lists.
    
    This implements a std::forward_list-like class (hence the name),
    but because it is circular, it also support push and splice at
    both ends of the list. This makes it ideal to be used as a queue, 
    unlike forward_list. As with forward_list, it supports popFront
    and eraseAfter. Basic usage is very similar to CVector:
   
    #include "stc/cflist.h"
    #omclude "stc/cstring.h"
    declare_CFList(i, int64_t);
    declare_CFList_string(s);
 
    int main() {
        CFList_i list = cflist_init;
        CFList_s slist = cflist_init;
        int n;

        // Add one million random numbers...
        for (int i=0; i<1000000; ++i)
            cflist_i_pushBack(&list, rand() * rand());
        n = 0; 
        c_foreach (i, cflist_i, list)
            if (++n % 10000 == 0) printf("%d: %lld\n", n, i.item->value);
        // Sort them...
        cflist_i_sort(&list); // mergesort O(n*log n)
        n = 0;
        c_foreach (i, cflist_i, list)
            if (++n % 10000 == 0) printf("%d: %lld\n", n, i.item->value);
        cflist_i_destroy(&list);

        // Test CFList with CStrings 
        cflist_s_pushBack(&slist, cstring_make("Item 1"));
        cflist_s_pushBack(&slist, cstring_make("Item 2"));
        cflist_s_pushBack(&slist, cstring_make("Item X"));
        cflist_s_pushBack(&slist, cstring_make("Item 3"));
        printf("\n");
        c_foreach (i, cflist_s, slist)
            printf("%s\n", i.item->value.str);
        // Change the list...
        cflist_s_pushFront(&slist, cstring_make("Item 0"));
        cflist_s_remove(&slist, "Item X");
        printf("\n");
        c_foreach (i, cflist_s, slist)
            printf("%s\n", i.item->value.str);
        cflist_s_destroy(&slist);
    }
 */

#define declare_CFList(...)    c_MACRO_OVERLOAD(declare_CFList, __VA_ARGS__)

#define declare_CFList_2(tag, Value) \
                               declare_CFList_3(tag, Value, c_noDestroy)
#define declare_CFList_3(tag, Value, valueDestroy) \
                               declare_CFList_4(tag, Value, valueDestroy, c_defaultCompare)
#define declare_CFList_4(tag, Value, valueDestroy, valueCompare) \
                               declare_CFList_6(tag, Value, valueDestroy, Value, valueCompare, c_defaultGetRaw)
#define declare_CFList_string(tag) \
                               declare_CFList_6(tag, CString, cstring_destroy, const char*, cstring_compareRaw, cstring_getRaw)                            

#define declare_CFListTypes(tag, Value) \
    c_struct (CFListNode_##tag) { \
        CFListNode_##tag *next; \
        Value value; \
    }; \
 \
    c_struct (CFList_##tag) { \
        CFListNode_##tag* last; \
    }; \
 \
    c_struct (cflist_##tag##_iter_t) { \
        CFListNode_##tag *item, **_last; \
    }

#define cflist_init          {NULL}
#define cflist_front(list)   (list).last->next->value
#define cflist_back(list)    (list).last->value
#define cflist_empty(list)   ((list).last == NULL)


#define declare_CFList_6(tag, Value, valueDestroy, ValueRaw, valueCompareRaw, valueGetRaw) \
 \
    declare_CFListTypes(tag, Value); \
    typedef ValueRaw cflist_##tag##_raw_t; \
 \
    STC_API void \
    cflist_##tag##_destroy(CFList_##tag* self); \
 \
    STC_API void \
    cflist_##tag##_pushFront(CFList_##tag* self, Value value); \
 \
    STC_API void \
    cflist_##tag##_popFront(CFList_##tag* self); \
 \
    STC_API void \
    cflist_##tag##_pushBack(CFList_##tag* self, Value value); \
 \
    STC_API void \
    cflist_##tag##_insertAfter(CFList_##tag* self, cflist_##tag##_iter_t pos, Value value); \
 \
    STC_API void \
    cflist_##tag##_eraseAfter(CFList_##tag* self, cflist_##tag##_iter_t pos); \
 \
    STC_API void \
    cflist_##tag##_spliceFront(CFList_##tag* self, CFList_##tag* other); \
 \
    STC_API void \
    cflist_##tag##_spliceAfter(CFList_##tag* self, cflist_##tag##_iter_t pos, CFList_##tag* other); \
 \
    STC_API int \
    cflist_##tag##_remove(CFList_##tag* self, ValueRaw val); \
 \
    STC_API void \
    cflist_##tag##_sort(CFList_##tag* self); \
 \
    static inline cflist_##tag##_iter_t \
    cflist_##tag##_begin(CFList_##tag* lst) { \
        CFListNode_##tag *head = lst->last ? lst->last->next : NULL; \
        cflist_##tag##_iter_t it = {head, &lst->last}; return it; \
    } \
    static inline cflist_##tag##_iter_t \
    cflist_##tag##_next(cflist_##tag##_iter_t it) { \
        it.item = it.item == *it._last ? NULL : it.item->next; return it; \
    } \
    static inline cflist_##tag##_iter_t \
    cflist_##tag##_last(CFList_##tag* lst) { \
        cflist_##tag##_iter_t it = {lst->last, &lst->last}; return it; \
    } \
 \
    implement_CFList_6(tag, Value, valueDestroy, ValueRaw, valueCompareRaw, valueGetRaw) \
 \
    typedef Value cflist_##tag##_value_t

    
/* -------------------------- IMPLEMENTATION ------------------------- */

#if !defined(STC_HEADER) || defined(STC_IMPLEMENTATION)
#define implement_CFList_6(tag, Value, valueDestroy, ValueRaw, valueCompareRaw, valueGetRaw) \
 \
    STC_API void \
    cflist_##tag##_destroy(CFList_##tag* self) { \
        while (self->last) \
            cflist_##tag##_popFront(self); \
    } \
 \
    STC_API void \
    cflist_##tag##_pushFront(CFList_##tag* self, Value value) { \
        _cflist_insertAfter(tag, self->last, value); \
        if (!self->last) self->last = entry; \
    } \
    STC_API void \
    cflist_##tag##_popFront(CFList_##tag* self) { \
        _cflist_eraseAfter(tag, self->last, valueDestroy); \
    } \
 \
    STC_API void \
    cflist_##tag##_pushBack(CFList_##tag* self, Value value) { \
        _cflist_insertAfter(tag, self->last, value); \
        self->last = entry; \
    } \
 \
    STC_API void \
    cflist_##tag##_insertAfter(CFList_##tag* self, cflist_##tag##_iter_t pos, Value value) { \
        _cflist_insertAfter(tag, pos.item, value); \
        if (!self->last || pos.item == self->last) self->last = entry; \
    } \
 \
    STC_API void \
    cflist_##tag##_eraseAfter(CFList_##tag* self, cflist_##tag##_iter_t pos) { \
        _cflist_eraseAfter(tag, pos.item, valueDestroy); \
    } \
 \
    static inline void \
    _cflist_##tag##_splice(CFList_##tag* self, cflist_##tag##_iter_t pos, CFList_##tag* other, bool bottom) { \
        if (!pos.item) \
            self->last = pos.item = other->last; \
        else if (other->last) { \
            CFListNode_##tag *next = pos.item->next; \
            pos.item->next = other->last->next; \
            other->last->next = next; \
            if (bottom && pos.item == self->last) self->last = other->last; \
        } \
        other->last = NULL; \
    } \
    STC_API void \
    cflist_##tag##_spliceFront(CFList_##tag* self, CFList_##tag* other) { \
        _cflist_##tag##_splice(self, cflist_##tag##_last(self), other, false); \
    } \
    STC_API void \
    cflist_##tag##_spliceAfter(CFList_##tag* self, cflist_##tag##_iter_t pos, CFList_##tag* other) { \
        _cflist_##tag##_splice(self, pos, other, true); \
    } \
 \
    STC_API int \
    cflist_##tag##_remove(CFList_##tag* self, ValueRaw val) { \
        cflist_##tag##_iter_t prev = {self->last}; int n = 0; \
        ValueRaw r; \
        c_foreach (i, cflist_##tag, *self) { \
            if (valueCompareRaw((r = valueGetRaw(&i.item->value), &r), &val) == 0) { \
                cflist_##tag##_eraseAfter(self, prev), ++n; \
                if (prev.item == i.item) break; \
            } \
            prev = i; \
        } \
        return n; \
    } \
 \
    static inline int \
    cflist_##tag##_sortCmp(const void* x, const void* y) { \
        ValueRaw a = valueGetRaw(&((CFListNode_##tag *) x)->value); \
        ValueRaw b = valueGetRaw(&((CFListNode_##tag *) y)->value); \
        return valueCompareRaw(&a, &b); \
    } \
    STC_API void \
    cflist_##tag##_sort(CFList_##tag* self) { \
        CFListNode__base* last = _cflist_mergesort((CFListNode__base *) self->last, cflist_##tag##_sortCmp); \
        self->last = (CFListNode_##tag *) last; \
    }

#define _cflist_insertAfter(tag, node, val) \
    CFListNode_##tag *entry = c_new_1(CFListNode_##tag), \
                       *next = self->last ? node->next : entry; \
    entry->value = val; \
    entry->next = next; \
    if (node) node->next = entry
    /* +: set self->last based on node */

#define _cflist_eraseAfter(tag, node, valueDestroy) \
    CFListNode_##tag* del = node->next, *next = del->next; \
    node->next = next; \
    if (del == next) self->last = NULL; \
    else if (self->last == del) self->last = node; \
    valueDestroy(&del->value); \
    free(del)

declare_CFListTypes(_base, int);

/* Singly linked list Mergesort implementation by Simon Tatham. O(n*log(n)).
 * https://www.chiark.greenend.org.uk/~sgtatham/algorithms/listsort.html
 */
static inline CFListNode__base *
_cflist_mergesort(CFListNode__base *list, int (*cmp)(const void*, const void*)) {
    CFListNode__base *p, *q, *e, *tail, *oldhead;
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
#define implement_CFList_6(tag, Value, valueDestroy, ValueRaw, valueCompareRaw, valueGetRaw)
#endif

#endif
