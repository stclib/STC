/* MIT License
 *
 * Copyright (c) 2022 Tyge Løvset, NORCE, www.norceresearch.no
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
        stc64_t rng = stc64_new(12345);
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
        clist_ix_drop(&list);
    }
*/
#include <stc/ccommon.h>
#include <stdlib.h>



_c_clist_types(clist_VOID, int);
STC_API size_t _clist_count(const clist_VOID* self);
#define _clist_node(_cx_self, vp) c_container_of(vp, _cx_node, value)

#define _c_using_clist(_cx_self, i_val, i_cmp, i_valdrop, i_valfrom, i_valto, i_valraw, defTypes) \
\
    defTypes( _c_clist_types(_cx_self, i_val); ) \
    typedef i_valraw _cx_raw; \
\
    STC_API _cx_self          _cx_memb(_clone)(_cx_self lst); \
    STC_API void        _cx_memb(_drop)(_cx_self* self); \
    STC_API void        _cx_memb(_push_back)(_cx_self* self, i_val value); \
    STC_API void        _cx_memb(_push_front)(_cx_self* self, i_val value); \
    STC_API void        _cx_memb(_emplace_items)(_cx_self *self, const _cx_raw arr[], size_t n); \
    STC_API _cx_self          _cx_memb(_split_after)(_cx_self* self, _cx_iter pos1, _cx_iter pos2); \
    STC_API void        _cx_memb(_splice_after)(_cx_self* self, _cx_iter pos, _cx_self* other); \
    STC_DEF void        _cx_memb(_splice_after_range)(_cx_self* self, _cx_iter pos, _cx_self* other, _cx_iter i1, _cx_iter i2); \
    STC_API _cx_iter _cx_memb(_find)(const _cx_self* self, i_valraw val); \
    STC_API _cx_iter _cx_memb(_find_before)(const _cx_self* self, i_valraw val); \
    STC_API _cx_iter _cx_memb(_find_before_in)(_cx_iter it1, _cx_iter it2, i_valraw val); \
    STC_API void        _cx_memb(_sort)(_cx_self* self); \
    STC_API size_t      _cx_memb(_remove)(_cx_self* self, i_valraw val); \
    STC_API _cx_iter _cx_memb(_insert_after)(_cx_self* self, _cx_iter pos, i_val value); \
    STC_API _cx_iter _cx_memb(_erase_after)(_cx_self* self, _cx_iter pos); \
    STC_API _cx_iter _cx_memb(_erase_range_after)(_cx_self* self, _cx_iter pos, _cx_iter it2); \
    STC_API _cx_node* _cx_memb(_erase_after_)(_cx_self* self, _cx_node* node); \
\
    STC_INLINE _cx_self       _cx_memb(_init)(void) {_cx_self lst = {NULL}; return lst; } \
    STC_INLINE bool     _cx_memb(_empty)(_cx_self lst) { return lst.last == NULL; } \
    STC_INLINE size_t   _cx_memb(_count)(_cx_self lst) { return _clist_count((const clist_VOID*) &lst); } \
    STC_INLINE i_val    _cx_memb(_value_fromraw)(i_valraw raw) { return i_valfrom(raw); } \
    STC_INLINE i_val    _cx_memb(_value_clone)(i_val val) { i_valraw _r = i_valto((&val)); return i_valfrom(_r); } \
    STC_INLINE void     _cx_memb(_clear)(_cx_self* self) { _cx_memb(_drop)(self); } \
    STC_INLINE void     _cx_memb(_emplace_back)(_cx_self* self, i_valraw raw) \
                            { _cx_memb(_push_back)(self, i_valfrom(raw)); } \
    STC_INLINE void     _cx_memb(_emplace_front)(_cx_self* self, i_valraw raw) \
                            { _cx_memb(_push_front)(self, i_valfrom(raw)); } \
    STC_INLINE _cx_value* \
                        _cx_memb(_front)(const _cx_self* self) { return &self->last->next->value; } \
    STC_INLINE _cx_value* \
                        _cx_memb(_back)(const _cx_self* self) { return &self->last->value; } \
    STC_INLINE void     _cx_memb(_pop_front)(_cx_self* self) { _cx_memb(_erase_after_)(self, self->last); } \
    STC_INLINE void     _cx_memb(_splice_front)(_cx_self* self, _cx_self* other) \
                            { _cx_memb(_splice_after)(self, _cx_memb(_before_begin)(self), other); } \
    STC_INLINE void     _cx_memb(_splice_back)(_cx_self* self, _cx_self* other) \
                            { _cx_memb(_splice_after)(self, _cx_memb(_last)(self), other); } \
\
    STC_INLINE _cx_iter \
    _cx_memb(_emplace_after)(_cx_self* self, _cx_iter pos, i_valraw raw) { \
        return _cx_memb(_insert_after)(self, pos, i_valfrom(raw)); \
    } \
\
    STC_INLINE _cx_iter \
    _cx_memb(_before_begin)(const _cx_self* self) { \
        _cx_value *last = self->last ? &self->last->value : NULL; \
        _cx_iter it = {&self->last, last, -1}; return it; \
    } \
\
    STC_INLINE _cx_iter \
    _cx_memb(_begin)(const _cx_self* self) { \
        _cx_value* head = self->last ? &self->last->next->value : NULL; \
        _cx_iter it = {&self->last, head, 0}; return it; \
    } \
\
    STC_INLINE _cx_iter \
    _cx_memb(_last)(const _cx_self* self) { \
        _cx_value *last = self->last ? &self->last->value : NULL; \
        _cx_iter it = {&self->last, last, 0}; return it; \
    } \
\
    STC_INLINE _cx_iter \
    _cx_memb(_end)(const _cx_self* self) { \
        _cx_iter it = {NULL, NULL}; return it; \
    } \
\
    STC_INLINE void \
    _cx_memb(_next)(_cx_iter* it) { \
        _cx_node* node = _clist_node(_cx_self, it->ref); \
        it->ref = ((it->_state += node == *it->_last) == 1) ? NULL : &node->next->value; \
    } \
\
    STC_INLINE _cx_iter \
    _cx_memb(_advance)(_cx_iter it, size_t n) { \
        while (n-- && it.ref) _cx_memb(_next)(&it); return it; \
    } \
 \
    _c_implement_clist(_cx_self, i_val, i_cmp, i_valdrop, i_valfrom, i_valto, i_valraw) \
    struct stc_trailing_semicolon

/* -------------------------- IMPLEMENTATION ------------------------- */

#if defined(_i_implement)
#define _c_implement_clist(_cx_self, i_val, i_cmp, i_valdrop, i_valfrom, i_valto, i_valraw) \
\
    STC_DEF _cx_self \
    _cx_memb(_clone)(_cx_self lst) { \
        _cx_self out = _cx_memb(_init)(); \
        c_foreach_3 (i, _cx_self, lst) \
            _cx_memb(_emplace_back)(&out, i_valto(i.ref)); \
        return out; \
    } \
\
    STC_DEF void \
    _cx_memb(_drop)(_cx_self* self) { \
        while (self->last) _cx_memb(_erase_after_)(self, self->last); \
    } \
\
    STC_DEF void \
    _cx_memb(_push_back)(_cx_self* self, i_val value) { \
        _c_clist_insert_after(self, _cx_self, self->last, value); \
        self->last = entry; \
    } \
    STC_DEF void \
    _cx_memb(_push_front)(_cx_self* self, i_val value) { \
        _c_clist_insert_after(self, _cx_self, self->last, value); \
        if (!self->last) self->last = entry; \
    } \
\
    STC_DEF void \
    _cx_memb(_emplace_items)(_cx_self *self, const _cx_raw arr[], size_t n) { \
        for (size_t i=0; i<n; ++i) _cx_memb(_push_back)(self, i_valfrom(arr[i])); \
    } \
\
    STC_DEF _cx_iter \
    _cx_memb(_insert_after)(_cx_self* self, _cx_iter pos, i_val value) { \
        _cx_node* node = pos.ref ? _clist_node(_cx_self, pos.ref) : NULL; \
        _c_clist_insert_after(self, _cx_self, node, value); \
        if (!node || node == self->last && pos._state == 0) self->last = entry; \
        pos.ref = &entry->value, pos._state = 0; return pos; \
    } \
\
    STC_DEF _cx_iter \
    _cx_memb(_erase_after)(_cx_self* self, _cx_iter pos) { \
        _cx_memb(_erase_after_)(self, _clist_node(_cx_self, pos.ref)); \
        _cx_memb(_next)(&pos); return pos; \
    } \
\
    STC_DEF _cx_iter \
    _cx_memb(_erase_range_after)(_cx_self* self, _cx_iter it1, _cx_iter it2) { \
        _cx_node* node = _clist_node(_cx_self, it1.ref), *done = it2.ref ? _clist_node(_cx_self, it2.ref) : NULL; \
        while (node && node->next != done) \
            node = _cx_memb(_erase_after_)(self, node); \
        _cx_memb(_next)(&it1); return it1; \
    } \
\
    STC_DEF _cx_iter \
    _cx_memb(_find_before_in)(_cx_iter it1, _cx_iter it2, i_valraw val) { \
        _cx_iter i = it1; \
        for (_cx_memb(_next)(&i); i.ref != it2.ref; _cx_memb(_next)(&i)) { \
            i_valraw r = i_valto(i.ref); \
            if (i_eq((&r), (&val))) return it1; \
            it1 = i; \
        } \
        it1.ref = NULL; return it1; \
    } \
\
    STC_DEF _cx_iter \
    _cx_memb(_find_before)(const _cx_self* self, i_valraw val) { \
        _cx_iter it = _cx_memb(_find_before_in)(_cx_memb(_before_begin)(self), _cx_memb(_end)(self), val); \
        return it; \
    } \
\
    STC_DEF _cx_iter \
    _cx_memb(_find)(const _cx_self* self, i_valraw val) { \
        _cx_iter it = _cx_memb(_find_before_in)(_cx_memb(_before_begin)(self), _cx_memb(_end)(self), val); \
        if (it.ref != _cx_memb(_end)(self).ref) _cx_memb(_next)(&it); \
        return it; \
    } \
\
    STC_DEF _cx_node* \
    _cx_memb(_erase_after_)(_cx_self* self, _cx_node* node) { \
        _cx_node* del = node->next, *next = del->next; \
        node->next = next; \
        if (del == next) self->last = node = NULL; \
        else if (self->last == del) self->last = node, node = NULL; \
        i_valdrop((&del->value)); c_free(del); \
        return node; \
    } \
\
    STC_DEF size_t \
    _cx_memb(_remove)(_cx_self* self, i_valraw val) { \
        size_t n = 0; \
        _cx_node* prev = self->last, *node; \
        while (prev) { \
            node = prev->next; \
            i_valraw r = i_valto((&node->value)); \
            if (i_eq((&r), (&val))) \
                prev = _cx_memb(_erase_after_)(self, prev), ++n; \
            else \
                prev = (node == self->last ? NULL : node); \
        } \
        return n; \
    } \
\
    STC_DEF _cx_self \
    _cx_memb(_split_after)(_cx_self* self, _cx_iter pos1, _cx_iter pos2) { \
        _cx_node *node1 = _clist_node(_cx_self, pos1.ref), *next1 = node1->next, \
                    *node2 = _clist_node(_cx_self, pos2.ref); \
        node1->next = node2->next, node2->next = next1; \
        if (self->last == node2) self->last = node1; \
        _cx_self lst = {node2}; return lst; \
    } \
\
    STC_DEF void \
    _cx_memb(_splice_after)(_cx_self* self, _cx_iter pos, _cx_self* other) { \
        if (!pos.ref) \
            self->last = other->last; \
        else if (other->last) { \
            _cx_node *node = _clist_node(_cx_self, pos.ref), *next = node->next; \
            node->next = other->last->next; \
            other->last->next = next; \
            if (node == self->last && pos._state == 0) self->last = other->last; \
        } \
        other->last = NULL; \
    } \
\
    STC_DEF void \
    _cx_memb(_splice_after_range)(_cx_self* self, _cx_iter pos, _cx_self* other, _cx_iter pos1, _cx_iter pos2) { \
        _cx_self tmp = _cx_memb(_split_after)(other, pos1, pos2); \
        _cx_memb(_splice_after)(self, pos, &tmp); \
    } \
\
    STC_DEF int \
    _cx_memb(_sort_cmp_)(const void* x, const void* y) { \
        i_valraw a = i_valto((&((_cx_node *) x)->value)); \
        i_valraw b = i_valto((&((_cx_node *) y)->value)); \
        return i_cmp((&a), (&b)); \
    } \
\
    STC_DEF void \
    _cx_memb(_sort)(_cx_self* self) { \
        if (self->last) \
            self->last = (_cx_node *) _clist_mergesort((clist_VOID_node *) self->last->next, _cx_memb(_sort_cmp_)); \
    }


#define _c_clist_insert_after(self, _cx_self, node, val) \
    _cx_node *entry = c_alloc (_cx_node); \
    if (node) entry->next = node->next, node->next = entry; \
    else      entry->next = entry; \
    entry->value = val
    /* +: set self->last based on node */

STC_DEF size_t
_clist_count(const clist_VOID* self) {
    const clist_VOID_node *nd = self->last;
    if (!nd) return 0;
    size_t n = 1;
    while ((nd = nd->next) != self->last) ++n;
    return n;
}

/* Singly linked list Mergesort implementation by Simon Tatham. O(n*log n).
 * https://www.chiark.greenend.org.uk/~sgtatham/algorithms/listsort.html
 */
STC_DEF clist_VOID_node *
_clist_mergesort(clist_VOID_node *list, int (*cmp)(const void*, const void*)) {
    clist_VOID_node *p, *q, *e, *tail, *oldhead;
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
#define _c_implement_clist(_cx_self, i_val, i_cmp, i_valdrop, i_valfrom, i_valto, i_valraw)
#endif

#endif
