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

/*  Circular Singly-linked Lists.
    This implements a std::forward_list-like class in C. Because it is circular,
    it also support both push_back() and push_front(), unlike std::forward_list:

    #include <stdio.h>
    #include <stc/crandom.h>

    #define i_key int64_t
    #define i_tag ix
    #include <stc/clist.h>

    int main()
    {
        c_auto (clist_ix, list)
        {
            int n;
            for (int i = 0; i < 1000000; ++i) // one million
                clist_ix_push_back(&list, crandom() >> 32);
            n = 0;
            c_foreach (i, clist_ix, list)
                if (++n % 10000 == 0) printf("%8d: %10zu\n", n, *i.ref);
            // Sort them...
            clist_ix_sort(&list); // mergesort O(n*log n)
            n = 0;
            puts("sorted");
            c_foreach (i, clist_ix, list)
                if (++n % 10000 == 0) printf("%8d: %10zu\n", n, *i.ref);
        }
    }
*/
#include "ccommon.h"

#ifndef CLIST_H_INCLUDED
#include "forward.h"
#include <stdlib.h>
#include <string.h>

#define _c_clist_complete_types(SELF, dummy) \
    struct SELF##_node { \
        struct SELF##_node *next; \
        SELF##_value value; \
    }

#define clist_node_(vp) c_unchecked_container_of(vp, _cx_node, value)
    
_c_clist_types(clist_VOID, int);
_c_clist_complete_types(clist_VOID, dummy);

#define _c_clist_insert_after(self, _cx_self, node, val) \
    _cx_node *entry = c_alloc(_cx_node); \
    if (node) entry->next = node->next, node->next = entry; \
    else      entry->next = entry; \
    entry->value = val
    // +: set self->last based on node 

#define _c_clist_insert_node_after(self, _cx_self, node, entry) \
    if (node) entry->next = node->next, node->next = entry; \
    else      entry->next = entry

#endif // CLIST_H_INCLUDED

#ifndef _i_prefix
#define _i_prefix clist_
#endif
#include "template.h"

#if !c_option(c_is_fwd)
  _cx_deftypes(_c_clist_types, _cx_self, i_key);
#endif
_cx_deftypes(_c_clist_complete_types, _cx_self, dummy);
typedef i_keyraw _cx_raw;

STC_API void            _cx_memb(_drop)(_cx_self* self);
STC_API _cx_value*      _cx_memb(_push_back)(_cx_self* self, i_key value);
STC_API _cx_value*      _cx_memb(_push_front)(_cx_self* self, i_key value);
STC_API _cx_value*      _cx_memb(_push_node_back)(_cx_self* self, _cx_node* node);
STC_API _cx_value*      _cx_memb(_push_node_front)(_cx_self* self, _cx_node* node);
STC_API _cx_iter        _cx_memb(_insert_at)(_cx_self* self, _cx_iter it, i_key value);
STC_API _cx_iter        _cx_memb(_erase_at)(_cx_self* self, _cx_iter it);
STC_API _cx_iter        _cx_memb(_erase_range)(_cx_self* self, _cx_iter it1, _cx_iter it2);
#if !c_option(c_no_cmp)
STC_API size_t          _cx_memb(_remove)(_cx_self* self, _cx_raw val);
STC_API _cx_iter        _cx_memb(_find_in)(_cx_iter it1, _cx_iter it2, _cx_raw val);
STC_API int             _cx_memb(_value_cmp)(const _cx_value* x, const _cx_value* y);
STC_API int             _cx_memb(_sort_cmp_)(const clist_VOID_node* x, const clist_VOID_node* y);
#endif
STC_API _cx_iter        _cx_memb(_splice)(_cx_self* self, _cx_iter it, _cx_self* other);
STC_API _cx_self        _cx_memb(_split_off)(_cx_self* self, _cx_iter it1, _cx_iter it2);
STC_API _cx_node*       _cx_memb(_erase_after_)(_cx_self* self, _cx_node* node);

#if !defined _i_no_clone
STC_API _cx_self        _cx_memb(_clone)(_cx_self cx);
STC_INLINE i_key        _cx_memb(_value_clone)(i_key val)
                            { return i_keyclone(val); }
STC_INLINE void
_cx_memb(_copy)(_cx_self *self, _cx_self other) {
    if (self->last == other.last) return;
    _cx_memb(_drop)(self); *self = _cx_memb(_clone)(other);
}
#if !defined _i_no_emplace
STC_INLINE _cx_value*   _cx_memb(_emplace_back)(_cx_self* self, _cx_raw raw)
                            { return _cx_memb(_push_back)(self, i_keyfrom(raw)); }
STC_INLINE _cx_value*   _cx_memb(_emplace_front)(_cx_self* self, _cx_raw raw)
                            { return _cx_memb(_push_front)(self, i_keyfrom(raw)); }
STC_INLINE _cx_iter     _cx_memb(_emplace_at)(_cx_self* self, _cx_iter it, _cx_raw raw)
                            { return _cx_memb(_insert_at)(self, it, i_keyfrom(raw)); }
STC_INLINE _cx_value*   _cx_memb(_emplace)(_cx_self* self, _cx_raw raw)
                            { return _cx_memb(_push_back)(self, i_keyfrom(raw)); }
#endif // !_i_no_emplace
#endif // !_i_no_clone

STC_INLINE _cx_self     _cx_memb(_init)(void) { return c_make(_cx_self){NULL}; }
STC_INLINE bool         _cx_memb(_reserve)(_cx_self* self, size_t n) { return true; }
STC_INLINE bool         _cx_memb(_empty)(_cx_self cx) { return cx.last == NULL; }
STC_INLINE void         _cx_memb(_clear)(_cx_self* self) { _cx_memb(_drop)(self); }
STC_INLINE _cx_value*   _cx_memb(_push)(_cx_self* self, i_key value)
                            { return _cx_memb(_push_back)(self, value); }
STC_INLINE void         _cx_memb(_pop_front)(_cx_self* self)
                            { _cx_memb(_erase_after_)(self, self->last); }
STC_INLINE _cx_value*   _cx_memb(_front)(const _cx_self* self) { return &self->last->next->value; }
STC_INLINE _cx_value*   _cx_memb(_back)(const _cx_self* self) { return &self->last->value; }

STC_INLINE size_t
_cx_memb(_count)(_cx_self cx) {
    size_t n = 1; const _cx_node *node = cx.last;
    if (!node) return 0;
    while ((node = node->next) != cx.last) ++n;
    return n;
}

STC_INLINE _cx_iter
_cx_memb(_begin)(const _cx_self* self) {
    _cx_value* head = self->last ? &self->last->next->value : NULL;
    return c_make(_cx_iter){head, &self->last, self->last};
}

STC_INLINE _cx_iter
_cx_memb(_end)(const _cx_self* self) {
    return c_make(_cx_iter){NULL};
}

STC_INLINE void
_cx_memb(_next)(_cx_iter* it) {
    _cx_node* node = it->prev = clist_node_(it->ref);
    it->ref = (node == *it->_last ? NULL : &node->next->value);
}

STC_INLINE _cx_iter
_cx_memb(_advance)(_cx_iter it, size_t n) {
    while (n-- && it.ref) _cx_memb(_next)(&it);
    return it;
}

STC_INLINE _cx_iter
_cx_memb(_splice_range)(_cx_self* self, _cx_iter it,
                        _cx_self* other, _cx_iter it1, _cx_iter it2) {
    _cx_self tmp = _cx_memb(_split_off)(other, it1, it2);
    return _cx_memb(_splice)(self, it, &tmp);
}

#if !c_option(c_no_cmp)
STC_INLINE _cx_iter
_cx_memb(_find)(const _cx_self* self, _cx_raw val) {
    return _cx_memb(_find_in)(_cx_memb(_begin)(self), _cx_memb(_end)(self), val);
}

STC_INLINE const _cx_value*
_cx_memb(_get)(const _cx_self* self, _cx_raw val) {
    return _cx_memb(_find_in)(_cx_memb(_begin)(self), _cx_memb(_end)(self), val).ref;
}

STC_INLINE _cx_value*
_cx_memb(_get_mut)(_cx_self* self, _cx_raw val) {
    return _cx_memb(_find_in)(_cx_memb(_begin)(self), _cx_memb(_end)(self), val).ref;
}

STC_INLINE void
_cx_memb(_sort)(_cx_self* self) {
    extern clist_VOID_node*
    _clist_mergesort(clist_VOID_node *list, int (*cmp)(const clist_VOID_node*, const clist_VOID_node*));

    if (self->last)
        self->last = (_cx_node *)_clist_mergesort((clist_VOID_node *)self->last->next, _cx_memb(_sort_cmp_));
}
#endif

#if defined(i_extern)
/* Implement non-templated extern functions */
// Singly linked list Mergesort implementation by Simon Tatham. O(n*log n).
// https://www.chiark.greenend.org.uk/~sgtatham/algorithms/listsort.html
clist_VOID_node *
_clist_mergesort(clist_VOID_node *list, int (*cmp)(const clist_VOID_node*, const clist_VOID_node*)) {
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
#endif // i_extern

// -------------------------- IMPLEMENTATION -------------------------
#if defined(i_implement)

#if !defined _i_no_clone
STC_DEF _cx_self
_cx_memb(_clone)(_cx_self cx) {
    _cx_self out = _cx_memb(_init)();
    c_foreach (it, _cx_self, cx)
        _cx_memb(_push_back)(&out, i_keyclone((*it.ref)));
    return out;
}
#endif

STC_DEF void
_cx_memb(_drop)(_cx_self* self) {
    while (self->last) _cx_memb(_erase_after_)(self, self->last);
}

STC_DEF _cx_value*
_cx_memb(_push_back)(_cx_self* self, i_key value) {
    _c_clist_insert_after(self, _cx_self, self->last, value);
    self->last = entry;
    return &entry->value;
}

STC_DEF _cx_value*
_cx_memb(_push_node_back)(_cx_self* self, _cx_node* entry) {
    _c_clist_insert_node_after(self, _cx_self, self->last, entry);
    self->last = entry;
    return &entry->value;
}

STC_DEF _cx_value*
_cx_memb(_push_front)(_cx_self* self, i_key value) {
    _c_clist_insert_after(self, _cx_self, self->last, value);
    if (!self->last)
        self->last = entry;
    return &entry->value;
}

STC_DEF _cx_value*
_cx_memb(_push_node_front)(_cx_self* self, _cx_node* entry) {
    _c_clist_insert_node_after(self, _cx_self, self->last, entry);
    if (!self->last)
        self->last = entry;
    return &entry->value;
}

STC_DEF _cx_iter
_cx_memb(_insert_at)(_cx_self* self, _cx_iter it, i_key value) {
    _cx_node* node = it.ref ? it.prev : self->last;
    _c_clist_insert_after(self, _cx_self, node, value);
    if (!self->last || !it.ref) {
        it.prev = self->last ? self->last : entry;
        self->last = entry;
    }
    it.ref = &entry->value;
    return it;
}

STC_DEF _cx_iter
_cx_memb(_erase_at)(_cx_self* self, _cx_iter it) {
    _cx_node *node = clist_node_(it.ref);
    it.ref = (node == self->last) ? NULL : &node->next->value;
    _cx_memb(_erase_after_)(self, it.prev);
    return it;
}

STC_DEF _cx_iter
_cx_memb(_erase_range)(_cx_self* self, _cx_iter it1, _cx_iter it2) {
    _cx_node *node = it1.ref ? it1.prev : NULL,
             *done = it2.ref ? clist_node_(it2.ref) : NULL;
    while (node && node->next != done)
        node = _cx_memb(_erase_after_)(self, node);
    return it2;
}

STC_DEF _cx_node*
_cx_memb(_erase_after_)(_cx_self* self, _cx_node* node) {
    _cx_node* del = node->next, *next = del->next;
    node->next = next;
    if (del == next)
        self->last = node = NULL;
    else if (self->last == del)
        self->last = node, node = NULL;
    i_keydrop((&del->value)); c_free(del);
    return node;
}

STC_DEF _cx_iter
_cx_memb(_splice)(_cx_self* self, _cx_iter it, _cx_self* other) {
    if (!self->last)
        self->last = other->last;
    else if (other->last) {
        _cx_node *p = it.ref ? it.prev : self->last, *next = p->next;
        it.prev = other->last;
        p->next = it.prev->next;
        it.prev->next = next;
        if (!it.ref) self->last = it.prev;
    }
    other->last = NULL; return it;
}

STC_DEF _cx_self
_cx_memb(_split_off)(_cx_self* self, _cx_iter it1, _cx_iter it2) {
    _cx_self cx = {NULL};
    if (it1.ref == it2.ref)
        return cx;
    _cx_node *p1 = it1.prev,
             *p2 = it2.ref ? it2.prev : self->last;
    p1->next = p2->next;
    p2->next = clist_node_(it1.ref);
    if (self->last == p2)
        self->last = (p1 == p2) ? NULL : p1;
    cx.last = p2;
    return cx;
}

#if !c_option(c_no_cmp)

STC_DEF _cx_iter
_cx_memb(_find_in)(_cx_iter it1, _cx_iter it2, _cx_raw val) {
    c_foreach (it, _cx_self, it1, it2) {
        _cx_raw r = i_keyto(it.ref);
        if (i_eq((&r), (&val)))
            return it;
    }
    it2.ref = NULL; return it2;
}

STC_DEF size_t
_cx_memb(_remove)(_cx_self* self, _cx_raw val) {
    size_t n = 0;
    _cx_node* prev = self->last, *node;
    while (prev) {
        node = prev->next;
        _cx_raw r = i_keyto((&node->value));
        if (i_eq((&r), (&val)))
            prev = _cx_memb(_erase_after_)(self, prev), ++n;
        else
            prev = (node == self->last ? NULL : node);
    }
    return n;
}

STC_DEF int
_cx_memb(_sort_cmp_)(const clist_VOID_node* x, const clist_VOID_node* y) {
    const _cx_raw a = i_keyto((&((const _cx_node *) x)->value));
    const _cx_raw b = i_keyto((&((const _cx_node *) y)->value));
    return i_cmp((&a), (&b));
}

STC_DEF int
_cx_memb(_value_cmp)(const _cx_value* x, const _cx_value* y) {
    const _cx_raw rx = i_keyto(x);
    const _cx_raw ry = i_keyto(y);
    return i_cmp((&rx), (&ry));
}
#endif // !c_no_cmp
#endif // i_implement
#define CLIST_H_INCLUDED
#include "template.h"
