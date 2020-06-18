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

/*  Circular Singly-linked Lists.
    
    This implements a std::forward_list-like class in C, but because it is circular,
    it also support push* and splice* at both ends of the list. This makes it ideal
    for being used as a queue, unlike std::forward_list's push_front/pop_front only.
    Basic usage is similar to CVector:
   
    #include "stc/clist.h"
    #omclude "stc/cstring.h"
    declare_CList(i, int64_t);
    declare_CList_string(s);
 
    int main() {
        CList_i list = clist_init;
        CList_s slist = clist_init;
        int n;
        for (int i=0; i<1000000; ++i) // one million
            clist_i_pushBack(&list, rand() * rand());
        n = 0; 
        c_foreach (i, clist_i, list)
            if (++n % 10000 == 0) printf("%d: %zd\n", n, i.item->value);
        // Sort them...
        clist_i_sort(&list); // mergesort O(n*log n)
        n = 0;
        c_foreach (i, clist_i, list)
            if (++n % 10000 == 0) printf("%d: %zd\n", n, i.item->value);
        clist_i_destroy(&list);

        // Test CList with CStrings 
        clist_s_pushBack(&slist, cstring_make("Item 1"));
        clist_s_pushBack(&slist, cstring_make("Item 2"));
        clist_s_pushBack(&slist, cstring_make("Item X"));
        clist_s_pushBack(&slist, cstring_make("Item 3"));
        puts("");
        c_foreach (i, clist_s, slist)
            printf("%s\n", i.item->value.str);
        // Modify
        clist_s_pushFront(&slist, cstring_make("Item 0"));
        clist_s_remove(&slist, "Item X");
        puts("");
        c_foreach (i, clist_s, slist)
            printf("%s\n", i.item->value.str);
        clist_s_destroy(&slist);
    }
 */

#define declare_CList(...)    c_MACRO_OVERLOAD(declare_CList, __VA_ARGS__)

#define declare_CList_2(tag, Value) \
                               declare_CList_3(tag, Value, c_noDestroy)
#define declare_CList_3(tag, Value, valueDestroy) \
                               declare_CList_4(tag, Value, valueDestroy, c_defaultCompare)
#define declare_CList_4(tag, Value, valueDestroy, valueCompare) \
                               declare_CList_6(tag, Value, valueDestroy, Value, valueCompare, c_defaultGetRaw)
#define declare_CList_string(tag) \
                               declare_CList_6(tag, CString, cstring_destroy, const char*, cstring_compareRaw, cstring_getRaw)                            

#define declare_CListTypes(tag, Value) \
    typedef struct CListNode_##tag { \
        struct CListNode_##tag *next; \
        Value value; \
    } CListNode_##tag; \
 \
    typedef struct CList_##tag { \
        CListNode_##tag *last; \
    } CList_##tag; \
 \
    typedef struct { \
        CListNode_##tag *item, **_last; \
    } CListIter_##tag, clist_##tag##_iter_t

#define clist_init          {NULL}
#define clist_front(list)   (list).last->next->value
#define clist_back(list)    (list).last->value
#define clist_empty(list)   ((list).last == NULL)


#define declare_CList_6(tag, Value, valueDestroy, RawValue, valueCompareRaw, valueGetRaw) \
 \
    declare_CListTypes(tag, Value); \
 \
    STC_API void \
    clist_##tag##_destroy(CList_##tag* self); \
    STC_API void \
    clist_##tag##_pushBack(CList_##tag* self, Value value); \
    STC_API void \
    clist_##tag##_pushFront(CList_##tag* self, Value value); \
    STC_API void \
    clist_##tag##_popFront(CList_##tag* self); \
    STC_API void \
    clist_##tag##_insertAfter(CList_##tag* self, clist_##tag##_iter_t pos, Value value); \
    STC_API void \
    clist_##tag##_eraseAfter(CList_##tag* self, clist_##tag##_iter_t pos); \
    STC_API void \
    clist_##tag##_spliceFront(CList_##tag* self, CList_##tag* other); \
    STC_API void \
    clist_##tag##_spliceAfter(CList_##tag* self, clist_##tag##_iter_t pos, CList_##tag* other); \
    STC_API clist_##tag##_iter_t \
    clist_##tag##_findBefore(CList_##tag* self, RawValue val); \
    STC_API Value* \
    clist_##tag##_find(CList_##tag* self, RawValue val); \
    STC_API clist_##tag##_iter_t \
    clist_##tag##_remove(CList_##tag* self, RawValue val); \
    STC_API void \
    clist_##tag##_sort(CList_##tag* self); \
 \
    static inline clist_##tag##_iter_t \
    clist_##tag##_begin(CList_##tag* self) { \
        CListNode_##tag *head = self->last ? self->last->next : NULL; \
        clist_##tag##_iter_t it = {head, &self->last}; return it; \
    } \
    static inline clist_##tag##_iter_t \
    clist_##tag##_next(clist_##tag##_iter_t it) { \
        it.item = it.item == *it._last ? NULL : it.item->next; return it; \
    } \
    static inline clist_##tag##_iter_t \
    clist_##tag##_last(CList_##tag* self) { \
        clist_##tag##_iter_t it = {self->last, &self->last}; return it; \
    } \
 \
    implement_CList_6(tag, Value, valueDestroy, RawValue, valueCompareRaw, valueGetRaw) \
    typedef RawValue CListRawValue_##tag; \
    typedef Value CListValue_##tag

    
/* -------------------------- IMPLEMENTATION ------------------------- */

#if !defined(STC_HEADER) || defined(STC_IMPLEMENTATION)
#define implement_CList_6(tag, Value, valueDestroy, RawValue, valueCompareRaw, valueGetRaw) \
 \
    STC_API void \
    clist_##tag##_destroy(CList_##tag* self) { \
        while (self->last) \
            clist_##tag##_popFront(self); \
    } \
 \
    STC_API void \
    clist_##tag##_pushBack(CList_##tag* self, Value value) { \
        _clist_insertAfter(self, tag, self->last, value); \
        self->last = entry; \
    } \
    STC_API void \
    clist_##tag##_pushFront(CList_##tag* self, Value value) { \
        _clist_insertAfter(self, tag, self->last, value); \
        if (!self->last) self->last = entry; \
    } \
    STC_API void \
    clist_##tag##_popFront(CList_##tag* self) { \
        _clist_eraseAfter(self, tag, self->last, valueDestroy); \
    } \
 \
    STC_API void \
    clist_##tag##_insertAfter(CList_##tag* self, clist_##tag##_iter_t pos, Value value) { \
        _clist_insertAfter(self, tag, pos.item, value); \
        if (!self->last || pos.item == self->last) self->last = entry; \
    } \
    STC_API void \
    clist_##tag##_eraseAfter(CList_##tag* self, clist_##tag##_iter_t pos) { \
        _clist_eraseAfter(self, tag, pos.item, valueDestroy); \
    } \
 \
    static inline void \
    _clist_##tag##_splice(CList_##tag* self, clist_##tag##_iter_t pos, CList_##tag* other, bool bottom) { \
        if (!pos.item) \
            self->last = pos.item = other->last; \
        else if (other->last) { \
            CListNode_##tag *next = pos.item->next; \
            pos.item->next = other->last->next; \
            other->last->next = next; \
            if (bottom && pos.item == self->last) self->last = other->last; \
        } \
        other->last = NULL; \
    } \
    STC_API void \
    clist_##tag##_spliceFront(CList_##tag* self, CList_##tag* other) { \
        _clist_##tag##_splice(self, clist_##tag##_last(self), other, false); \
    } \
    STC_API void \
    clist_##tag##_spliceAfter(CList_##tag* self, clist_##tag##_iter_t pos, CList_##tag* other) { \
        _clist_##tag##_splice(self, pos, other, true); \
    } \
 \
    STC_API clist_##tag##_iter_t \
    clist_##tag##_findBefore(CList_##tag* self, RawValue val) { \
        clist_##tag##_iter_t prev = {self->last, &self->last}; \
        c_foreach (i, clist_##tag, *self) { \
            RawValue r = valueGetRaw(&i.item->value); \
            if (valueCompareRaw(&r, &val) == 0) { \
                return prev; \
            } \
            prev = i; \
        } \
        prev.item = NULL; return prev; \
    } \
 \
    STC_API Value* \
    clist_##tag##_find(CList_##tag* self, RawValue val) { \
        clist_##tag##_iter_t it = clist_##tag##_findBefore(self, val); \
        return it.item ? &it.item->next->value : NULL; \
    } \
 \
    STC_API clist_##tag##_iter_t \
    clist_##tag##_remove(CList_##tag* self, RawValue val) { \
        clist_##tag##_iter_t it = clist_##tag##_findBefore(self, val); \
        if (it.item) clist_##tag##_eraseAfter(self, it); \
        return it; \
    } \
 \
    static inline int \
    clist_##tag##_sortCmp(const void* x, const void* y) { \
        RawValue a = valueGetRaw(&((CListNode_##tag *) x)->value); \
        RawValue b = valueGetRaw(&((CListNode_##tag *) y)->value); \
        return valueCompareRaw(&a, &b); \
    } \
    STC_API void \
    clist_##tag##_sort(CList_##tag* self) { \
        CListNode__base* last = _clist_mergesort((CListNode__base *) self->last, clist_##tag##_sortCmp); \
        self->last = (CListNode_##tag *) last; \
    }

#define _clist_insertAfter(self, tag, node, val) \
    CListNode_##tag *entry = c_new_1(CListNode_##tag), \
                    *next = self->last ? node->next : entry; \
    entry->value = val; \
    entry->next = next; \
    if (node) node->next = entry
    /* +: set self->last based on node */

#define _clist_eraseAfter(self, tag, node, valueDestroy) \
    CListNode_##tag* del = node->next, *next = del->next; \
    node->next = next; \
    if (del == next) self->last = NULL; \
    else if (self->last == del) self->last = node; \
    valueDestroy(&del->value); \
    free(del)

declare_CListTypes(_base, int);

/* Singly linked list Mergesort implementation by Simon Tatham. O(n*log n).
 * https://www.chiark.greenend.org.uk/~sgtatham/algorithms/listsort.html
 */
static inline CListNode__base *
_clist_mergesort(CListNode__base *list, int (*cmp)(const void*, const void*)) {
    CListNode__base *p, *q, *e, *tail, *oldhead;
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
#define implement_CList_6(tag, Value, valueDestroy, RawValue, valueCompareRaw, valueGetRaw)
#endif

#endif
