/* MIT License
 *
 * Copyright (c) 2023 Tyge Løvset
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
    #include <stc/crand.h>

    #define i_key int64_t
    #define i_tag ix
    #include <stc/clist.h>

    int main(void)
    {
        c_auto (clist_ix, list)
        {
            int n;
            for (int i = 0; i < 1000000; ++i) // one million
                clist_ix_push_back(&list, crand() >> 32);
            n = 0;
            c_foreach (i, clist_ix, list)
                if (++n % 10000 == 0) printf("%8d: %10zu\n", n, *i.ref);
            // Sort them...
            clist_ix_sort(&list); // qsort O(n*log n)
            n = 0;
            puts("sorted");
            c_foreach (i, clist_ix, list)
                if (++n % 10000 == 0) printf("%8d: %10zu\n", n, *i.ref);
        }
    }
*/
#include "priv/linkage.h"

#ifndef CLIST_H_INCLUDED
#include "ccommon.h"
#include "forward.h"
#include <stdlib.h>
#include <string.h>

#define _c_clist_complete_types(SELF, dummy) \
    struct SELF##_node { \
        struct SELF##_node *next; \
        SELF##_value value; \
    }

#define _clist_tonode(vp) c_container_of(vp, _cx_node, value)
    
#define _c_clist_insert_entry_after(ref, val) \
    _cx_node *entry = (_cx_node *)i_malloc(c_sizeof *entry); entry->value = val; \
    _c_clist_insert_after_node(ref, entry)

#define _c_clist_insert_after_node(ref, entry) \
    if (ref) entry->next = ref->next, ref->next = entry; \
    else     entry->next = entry
    // +: set self->last based on node 

#endif // CLIST_H_INCLUDED

#define _i_prefix clist_
#include "priv/template.h"

#ifndef i_is_forward
  _cx_DEFTYPES(_c_clist_types, _cx_Self, i_key);
#endif
_cx_DEFTYPES(_c_clist_complete_types, _cx_Self, dummy);
typedef i_keyraw _cx_raw;

STC_API void            _cx_MEMB(_drop)(_cx_Self* self);
STC_API _cx_value*      _cx_MEMB(_push_back)(_cx_Self* self, i_key value);
STC_API _cx_value*      _cx_MEMB(_push_front)(_cx_Self* self, i_key value);
STC_API _cx_iter        _cx_MEMB(_insert_at)(_cx_Self* self, _cx_iter it, i_key value);
STC_API _cx_iter        _cx_MEMB(_erase_at)(_cx_Self* self, _cx_iter it);
STC_API _cx_iter        _cx_MEMB(_erase_range)(_cx_Self* self, _cx_iter it1, _cx_iter it2);
#if defined _i_has_eq || defined _i_has_cmp
STC_API _cx_iter        _cx_MEMB(_find_in)(_cx_iter it1, _cx_iter it2, _cx_raw val);
STC_API intptr_t        _cx_MEMB(_remove)(_cx_Self* self, _cx_raw val);
#endif
#if defined _i_has_cmp
STC_API bool            _cx_MEMB(_sort_with)(_cx_Self* self, int(*cmp)(const _cx_value*, const _cx_value*));
STC_API int             _cx_MEMB(_sort_cmp_)(const _cx_value*, const _cx_value*);
STC_INLINE bool         _cx_MEMB(_sort)(_cx_Self* self)
                            { return _cx_MEMB(_sort_with)(self, _cx_MEMB(_sort_cmp_)); }
#endif
STC_API void            _cx_MEMB(_reverse)(_cx_Self* self);
STC_API _cx_iter        _cx_MEMB(_splice)(_cx_Self* self, _cx_iter it, _cx_Self* other);
STC_API _cx_Self        _cx_MEMB(_split_off)(_cx_Self* self, _cx_iter it1, _cx_iter it2);
STC_API _cx_value*      _cx_MEMB(_push_back_node)(_cx_Self* self, _cx_node* node);
STC_API _cx_value*      _cx_MEMB(_insert_after_node)(_cx_Self* self, _cx_node* ref, _cx_node* node);
STC_API _cx_node*       _cx_MEMB(_unlink_after_node)(_cx_Self* self, _cx_node* ref);
STC_API void            _cx_MEMB(_erase_after_node)(_cx_Self* self, _cx_node* ref);
STC_INLINE _cx_node*    _cx_MEMB(_get_node)(_cx_value* pval) { return _clist_tonode(pval); }
STC_INLINE _cx_node*    _cx_MEMB(_unlink_front_node)(_cx_Self* self)
                            { return _cx_MEMB(_unlink_after_node)(self, self->last); }
#if !defined i_no_clone
STC_API _cx_Self        _cx_MEMB(_clone)(_cx_Self cx);
STC_INLINE i_key        _cx_MEMB(_value_clone)(i_key val) { return i_keyclone(val); }
STC_INLINE void
_cx_MEMB(_copy)(_cx_Self *self, const _cx_Self* other) {
    if (self->last == other->last) return;
    _cx_MEMB(_drop)(self); *self = _cx_MEMB(_clone)(*other);
}
#endif // !i_no_clone

#if !defined i_no_emplace
STC_INLINE _cx_value*   _cx_MEMB(_emplace_back)(_cx_Self* self, _cx_raw raw)
                            { return _cx_MEMB(_push_back)(self, i_keyfrom(raw)); }
STC_INLINE _cx_value*   _cx_MEMB(_emplace_front)(_cx_Self* self, _cx_raw raw)
                            { return _cx_MEMB(_push_front)(self, i_keyfrom(raw)); }
STC_INLINE _cx_iter     _cx_MEMB(_emplace_at)(_cx_Self* self, _cx_iter it, _cx_raw raw)
                            { return _cx_MEMB(_insert_at)(self, it, i_keyfrom(raw)); }
STC_INLINE _cx_value*   _cx_MEMB(_emplace)(_cx_Self* self, _cx_raw raw)
                            { return _cx_MEMB(_push_back)(self, i_keyfrom(raw)); }
#endif // !i_no_emplace

STC_INLINE _cx_Self     _cx_MEMB(_init)(void) { return c_LITERAL(_cx_Self){NULL}; }
STC_INLINE void         _cx_MEMB(_put_n)(_cx_Self* self, const _cx_raw* raw, intptr_t n)
                            { while (n--) _cx_MEMB(_push_back)(self, i_keyfrom(*raw++)); }
STC_INLINE _cx_Self     _cx_MEMB(_from_n)(const _cx_raw* raw, intptr_t n)
                            { _cx_Self cx = {0}; _cx_MEMB(_put_n)(&cx, raw, n); return cx; }
STC_INLINE bool         _cx_MEMB(_reserve)(_cx_Self* self, intptr_t n) { (void)(self + n); return true; }
STC_INLINE bool         _cx_MEMB(_empty)(const _cx_Self* self) { return self->last == NULL; }
STC_INLINE void         _cx_MEMB(_clear)(_cx_Self* self) { _cx_MEMB(_drop)(self); }
STC_INLINE _cx_value*   _cx_MEMB(_push)(_cx_Self* self, i_key value)
                            { return _cx_MEMB(_push_back)(self, value); }
STC_INLINE void         _cx_MEMB(_pop_front)(_cx_Self* self)
                            { c_assert(!_cx_MEMB(_empty)(self)); _cx_MEMB(_erase_after_node)(self, self->last); }
STC_INLINE _cx_value*   _cx_MEMB(_front)(const _cx_Self* self) { return &self->last->next->value; }
STC_INLINE _cx_value*   _cx_MEMB(_back)(const _cx_Self* self) { return &self->last->value; }
STC_INLINE _cx_raw      _cx_MEMB(_value_toraw)(const _cx_value* pval) { return i_keyto(pval); }
STC_INLINE void         _cx_MEMB(_value_drop)(_cx_value* pval) { i_keydrop(pval); }

STC_INLINE intptr_t
_cx_MEMB(_count)(const _cx_Self* self) {
    intptr_t n = 1; const _cx_node *node = self->last;
    if (!node) return 0;
    while ((node = node->next) != self->last) ++n;
    return n;
}

STC_INLINE _cx_iter
_cx_MEMB(_begin)(const _cx_Self* self) {
    _cx_value* head = self->last ? &self->last->next->value : NULL;
    return c_LITERAL(_cx_iter){head, &self->last, self->last};
}

STC_INLINE _cx_iter
_cx_MEMB(_end)(const _cx_Self* self)
    { return c_LITERAL(_cx_iter){NULL}; }

STC_INLINE void
_cx_MEMB(_next)(_cx_iter* it) {
    _cx_node* node = it->prev = _clist_tonode(it->ref);
    it->ref = (node == *it->_last ? NULL : &node->next->value);
}

STC_INLINE _cx_iter
_cx_MEMB(_advance)(_cx_iter it, size_t n) {
    while (n-- && it.ref) _cx_MEMB(_next)(&it);
    return it;
}

STC_INLINE _cx_iter
_cx_MEMB(_splice_range)(_cx_Self* self, _cx_iter it,
                        _cx_Self* other, _cx_iter it1, _cx_iter it2) {
    _cx_Self tmp = _cx_MEMB(_split_off)(other, it1, it2);
    return _cx_MEMB(_splice)(self, it, &tmp);
}

#if defined _i_has_eq || defined _i_has_cmp
STC_INLINE _cx_iter
_cx_MEMB(_find)(const _cx_Self* self, _cx_raw val) {
    return _cx_MEMB(_find_in)(_cx_MEMB(_begin)(self), _cx_MEMB(_end)(self), val);
}

STC_INLINE const _cx_value*
_cx_MEMB(_get)(const _cx_Self* self, _cx_raw val) {
    return _cx_MEMB(_find_in)(_cx_MEMB(_begin)(self), _cx_MEMB(_end)(self), val).ref;
}

STC_INLINE _cx_value*
_cx_MEMB(_get_mut)(_cx_Self* self, _cx_raw val) {
    return _cx_MEMB(_find_in)(_cx_MEMB(_begin)(self), _cx_MEMB(_end)(self), val).ref;
}

STC_INLINE bool _cx_MEMB(_eq)(const _cx_Self* self, const _cx_Self* other) {
    _cx_iter i = _cx_MEMB(_begin)(self), j = _cx_MEMB(_begin)(other);
    for (; i.ref && j.ref; _cx_MEMB(_next)(&i), _cx_MEMB(_next)(&j)) {
        const _cx_raw _rx = i_keyto(i.ref), _ry = i_keyto(j.ref);
        if (!(i_eq((&_rx), (&_ry)))) return false;
    }
    return !(i.ref || j.ref);
}
#endif

// -------------------------- IMPLEMENTATION -------------------------
#if defined(i_implement) || defined(i_static)

#if !defined i_no_clone
STC_DEF _cx_Self
_cx_MEMB(_clone)(_cx_Self cx) {
    _cx_Self out = _cx_MEMB(_init)();
    c_foreach (it, _cx_Self, cx)
        _cx_MEMB(_push_back)(&out, i_keyclone((*it.ref)));
    return out;
}
#endif

STC_DEF void
_cx_MEMB(_drop)(_cx_Self* self) {
    while (self->last) _cx_MEMB(_erase_after_node)(self, self->last);
}

STC_DEF _cx_value*
_cx_MEMB(_push_back)(_cx_Self* self, i_key value) {
    _c_clist_insert_entry_after(self->last, value);
    self->last = entry;
    return &entry->value;
}

STC_DEF _cx_value*
_cx_MEMB(_push_front)(_cx_Self* self, i_key value) {
    _c_clist_insert_entry_after(self->last, value);
    if (!self->last)
        self->last = entry;
    return &entry->value;
}

STC_DEF _cx_value*
_cx_MEMB(_push_back_node)(_cx_Self* self, _cx_node* node) {
    _c_clist_insert_after_node(self->last, node);
    self->last = node;
    return &node->value;
}

STC_DEF _cx_value*
_cx_MEMB(_insert_after_node)(_cx_Self* self, _cx_node* ref, _cx_node* node) {
    _c_clist_insert_after_node(ref, node);
    if (!self->last)
        self->last = node;
    return &node->value;
}

STC_DEF _cx_iter
_cx_MEMB(_insert_at)(_cx_Self* self, _cx_iter it, i_key value) {
    _cx_node* node = it.ref ? it.prev : self->last;
    _c_clist_insert_entry_after(node, value);
    if (!self->last || !it.ref) {
        it.prev = self->last ? self->last : entry;
        self->last = entry;
    }
    it.ref = &entry->value;
    return it;
}

STC_DEF _cx_iter
_cx_MEMB(_erase_at)(_cx_Self* self, _cx_iter it) {
    _cx_node *node = _clist_tonode(it.ref);
    it.ref = (node == self->last) ? NULL : &node->next->value;
    _cx_MEMB(_erase_after_node)(self, it.prev);
    return it;
}

STC_DEF _cx_iter
_cx_MEMB(_erase_range)(_cx_Self* self, _cx_iter it1, _cx_iter it2) {
    _cx_node *end = it2.ref ? _clist_tonode(it2.ref) : self->last->next;
    if (it1.ref != it2.ref) do {
        _cx_MEMB(_erase_after_node)(self, it1.prev);
        if (!self->last) break;
    } while (it1.prev->next != end);
    return it2;
}

STC_DEF void
_cx_MEMB(_erase_after_node)(_cx_Self* self, _cx_node* ref) {
    _cx_node* node = _cx_MEMB(_unlink_after_node)(self, ref);
    i_keydrop((&node->value));
    i_free(node);
}

STC_DEF _cx_node*
_cx_MEMB(_unlink_after_node)(_cx_Self* self, _cx_node* ref) {
    _cx_node* node = ref->next, *next = node->next;
    ref->next = next;
    if (node == next)
        self->last = NULL;
    else if (node == self->last)
        self->last = ref;
    return node;
}

STC_DEF void
_cx_MEMB(_reverse)(_cx_Self* self) {
    _cx_Self rev = {NULL};
    while (self->last) {
        _cx_node* node = _cx_MEMB(_unlink_after_node)(self, self->last);
        _cx_MEMB(_insert_after_node)(&rev, rev.last, node);
    }
    *self = rev;
}

STC_DEF _cx_iter
_cx_MEMB(_splice)(_cx_Self* self, _cx_iter it, _cx_Self* other) {
    if (!self->last)
        self->last = other->last;
    else if (other->last) {
        _cx_node *p = it.ref ? it.prev : self->last, *next = p->next;
        it.prev = other->last;
        p->next = it.prev->next;
        it.prev->next = next;
        if (!it.ref) self->last = it.prev;
    }
    other->last = NULL;
    return it;
}

STC_DEF _cx_Self
_cx_MEMB(_split_off)(_cx_Self* self, _cx_iter it1, _cx_iter it2) {
    _cx_Self lst = {NULL};
    if (it1.ref == it2.ref)
        return lst;
    _cx_node *p1 = it1.prev,
             *p2 = it2.ref ? it2.prev : self->last;
    p1->next = p2->next;
    p2->next = _clist_tonode(it1.ref);
    if (self->last == p2)
        self->last = (p1 == p2) ? NULL : p1;
    lst.last = p2;
    return lst;
}

#if defined _i_has_eq || defined _i_has_cmp
STC_DEF _cx_iter
_cx_MEMB(_find_in)(_cx_iter it1, _cx_iter it2, _cx_raw val) {
    c_foreach (it, _cx_Self, it1, it2) {
        _cx_raw r = i_keyto(it.ref);
        if (i_eq((&r), (&val)))
            return it;
    }
    it2.ref = NULL; return it2;
}

STC_DEF intptr_t
_cx_MEMB(_remove)(_cx_Self* self, _cx_raw val) {
    intptr_t n = 0;
    _cx_node *prev = self->last, *node;
    if (prev) do {
        node = prev->next;
        _cx_raw r = i_keyto((&node->value));
        if (i_eq((&r), (&val))) {
            _cx_MEMB(_erase_after_node)(self, prev), ++n;
            if (!self->last) break;
        } else
            prev = node;
    } while (node != self->last);
    return n;
}
#endif

#if defined _i_has_cmp
STC_DEF int _cx_MEMB(_sort_cmp_)(const _cx_value* x, const _cx_value* y) {
    const _cx_raw a = i_keyto(x), b = i_keyto(y);
    return i_cmp((&a), (&b));
}

STC_DEF bool _cx_MEMB(_sort_with)(_cx_Self* self, int(*cmp)(const _cx_value*, const _cx_value*)) {
    intptr_t len = 0, cap = 0;
    _cx_value *a = NULL, *p = NULL;
    _cx_iter i;
    for (i = _cx_MEMB(_begin)(self); i.ref; _cx_MEMB(_next)(&i)) {
        if (len == cap) {
            if ((p = (_cx_value *)i_realloc(a, (cap += cap/2 + 4)*c_sizeof *a))) a = p; 
            else { i_free(a); return false; }
        }
        a[len++] = *i.ref;
    }
    qsort(a, (size_t)len, sizeof *a, (int(*)(const void*, const void*))cmp);
    for (i = _cx_MEMB(_begin)(self); i.ref; _cx_MEMB(_next)(&i), ++p) 
        *i.ref = *p;
    i_free(a); return true;
}
#endif // _i_has_cmp
#endif // i_implement
#define CLIST_H_INCLUDED
#include "priv/template2.h"
