/* MIT License
 *
 * Copyright (c) 2025 Tyge Løvset
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
    #include <stc/random.h>

    #define T List, long, (c_use_cmp) // enable sorting, uses default *x < *y.
    #include <stc/list.h>

    int main(void)
    {
        List list = {0};

        for (int i = 0; i < 5000000; ++i) // five million
            List_push_back(&list, crand64_uint() & (1<<24) - 1;

        int n = 0;
        for (c_each(i, List, list))
            if (++n % 100000 == 0) printf("%8d: %10zu\n", n, *i.ref);

        // Sort them...
        List_sort(&list); // sort.h quicksort

        n = 0;
        puts("sorted");
        for (c_each(i, List, list))
            if (++n % 100000 == 0) printf("%8d: %10zu\n", n, *i.ref);

        List_drop(&list);
    }
*/
#include "priv/linkage.h"
#include "types.h"

#ifndef STC_LIST_H_INCLUDED
#define STC_LIST_H_INCLUDED
#include "common.h"
#include <stdlib.h>

#define _c_list_complete_types(SELF, dummy) \
    struct SELF##_node { \
        SELF##_value value; /* must be first! */ \
        struct SELF##_node *next; \
    }

#define _clist_tonode(vp) c_safe_cast(_m_node*, _m_value*, vp)

#define _c_list_insert_entry_after(ref, val) \
    _m_node *entry = _i_new_n(_m_node, 1); entry->value = val; \
    _c_list_insert_after_node(ref, entry)

#define _c_list_insert_after_node(ref, entry) \
    if (ref) entry->next = ref->next, ref->next = entry; \
    else     entry->next = entry
    // +: set self->last based on node

#endif // STC_LIST_H_INCLUDED

#ifndef _i_prefix
  #define _i_prefix list_
#endif
#include "priv/template.h"

#define _i_is_list
#ifndef i_declared
  _c_DEFTYPES(_declare_list, Self, i_key, _i_aux_def);
#endif
_c_DEFTYPES(_c_list_complete_types, Self, dummy);
typedef i_keyraw _m_raw;

STC_API void            _c_MEMB(_drop)(const Self* cself);
STC_API _m_value*       _c_MEMB(_push_back)(Self* self, _m_value value);
STC_API _m_value*       _c_MEMB(_push_front)(Self* self, _m_value value);
STC_API _m_iter         _c_MEMB(_insert_at)(Self* self, _m_iter it, _m_value value);
STC_API _m_iter         _c_MEMB(_erase_at)(Self* self, _m_iter it);
STC_API _m_iter         _c_MEMB(_erase_range)(Self* self, _m_iter it1, _m_iter it2);
#if defined _i_has_eq
STC_API _m_iter         _c_MEMB(_find_in)(const Self* self, _m_iter it1, _m_iter it2, _m_raw val);
STC_API isize           _c_MEMB(_remove)(Self* self, _m_raw val);
#endif
#if defined _i_has_cmp
STC_API bool            _c_MEMB(_sort)(Self* self);
#endif
STC_API void            _c_MEMB(_reverse)(Self* self);
STC_API _m_iter         _c_MEMB(_splice)(Self* self, _m_iter it, Self* other);
STC_API Self            _c_MEMB(_split_off)(Self* self, _m_iter it1, _m_iter it2);
STC_API _m_value*       _c_MEMB(_push_back_node)(Self* self, _m_node* node);
STC_API _m_value*       _c_MEMB(_insert_after_node)(Self* self, _m_node* ref, _m_node* node);
STC_API _m_node*        _c_MEMB(_unlink_after_node)(Self* self, _m_node* ref);
STC_API void            _c_MEMB(_erase_after_node)(Self* self, _m_node* ref);
STC_INLINE _m_node*     _c_MEMB(_get_node)(_m_value* pval) { return _clist_tonode(pval); }
STC_INLINE _m_node*     _c_MEMB(_unlink_front_node)(Self* self)
                            { return _c_MEMB(_unlink_after_node)(self, self->last); }
#ifndef i_no_clone
STC_API Self            _c_MEMB(_clone)(Self cx);
STC_INLINE _m_value     _c_MEMB(_value_clone)(const Self* self, _m_value val)
                            { (void)self; return i_keyclone(val); }

STC_INLINE void
_c_MEMB(_copy)(Self *self, const Self* other) {
    if (self->last == other->last) return;
    _c_MEMB(_drop)(self);
    *self = _c_MEMB(_clone)(*other);
}
#endif // !i_no_clone

#ifndef i_no_emplace
STC_INLINE _m_value*    _c_MEMB(_emplace_back)(Self* self, _m_raw raw)
                            { return _c_MEMB(_push_back)(self, i_keyfrom(raw)); }
STC_INLINE _m_value*    _c_MEMB(_emplace_front)(Self* self, _m_raw raw)
                            { return _c_MEMB(_push_front)(self, i_keyfrom(raw)); }
STC_INLINE _m_iter      _c_MEMB(_emplace_at)(Self* self, _m_iter it, _m_raw raw)
                            { return _c_MEMB(_insert_at)(self, it, i_keyfrom(raw)); }
STC_INLINE _m_value*    _c_MEMB(_emplace)(Self* self, _m_raw raw)
                            { return _c_MEMB(_push_back)(self, i_keyfrom(raw)); }
#endif // !i_no_emplace

#ifndef _i_no_put
STC_INLINE void         _c_MEMB(_put_n)(Self* self, const _m_raw* raw, isize n)
                            { while (n--) _c_MEMB(_push_back)(self, i_keyfrom((*raw))), ++raw; }
#endif

#ifndef _i_aux_alloc
    STC_INLINE Self         _c_MEMB(_init)(void) { return c_literal(Self){0}; }
    #ifndef _i_no_put
    STC_INLINE Self         _c_MEMB(_from_n)(const _m_raw* raw, isize n)
                                { Self cx = {0}; _c_MEMB(_put_n)(&cx, raw, n); return cx; }
    #endif
#endif

STC_INLINE bool         _c_MEMB(_reserve)(Self* self, isize n) { (void)(self + n); return true; }
STC_INLINE bool         _c_MEMB(_is_empty)(const Self* self) { return self->last == NULL; }
STC_INLINE void         _c_MEMB(_clear)(Self* self) { _c_MEMB(_drop)(self); }
STC_INLINE _m_value*    _c_MEMB(_push)(Self* self, _m_value value)
                            { return _c_MEMB(_push_back)(self, value); }
STC_INLINE void         _c_MEMB(_pop_front)(Self* self)
                            { c_assert(!_c_MEMB(_is_empty)(self)); _c_MEMB(_erase_after_node)(self, self->last); }
STC_INLINE const _m_value* _c_MEMB(_front)(const Self* self) { return &self->last->next->value; }
STC_INLINE _m_value*       _c_MEMB(_front_mut)(Self* self) { return &self->last->next->value; }
STC_INLINE const _m_value* _c_MEMB(_back)(const Self* self) { return &self->last->value; }
STC_INLINE _m_value*       _c_MEMB(_back_mut)(Self* self) { return &self->last->value; }
STC_INLINE _m_raw       _c_MEMB(_value_toraw)(const _m_value* pval) { return i_keytoraw(pval); }
STC_INLINE void         _c_MEMB(_value_drop)(const Self* self, _m_value* pval) { (void)self; i_keydrop(pval); }

STC_INLINE Self _c_MEMB(_move)(Self *self) {
    Self m = *self;
    self->last = NULL;
    return m;
}

STC_INLINE void _c_MEMB(_take)(Self *self, Self unowned) {
    _c_MEMB(_drop)(self);
    *self = unowned;
}

STC_INLINE isize
_c_MEMB(_count)(const Self* self) {
    isize n = 1; const _m_node *node = self->last;
    if (node == NULL) return 0;
    while ((node = node->next) != self->last) ++n;
    return n;
}

STC_INLINE _m_iter
_c_MEMB(_begin)(const Self* self) {
    _m_value* head = self->last ? &self->last->next->value : NULL;
    return c_literal(_m_iter){head, &self->last, self->last};
}

STC_INLINE _m_iter
_c_MEMB(_end)(const Self* self)
    { (void)self; return c_literal(_m_iter){0}; }

STC_INLINE void
_c_MEMB(_next)(_m_iter* it) {
    _m_node* node = it->prev = _clist_tonode(it->ref);
    it->ref = (node == *it->_last ? NULL : &node->next->value);
}

STC_INLINE _m_iter
_c_MEMB(_advance)(_m_iter it, size_t n) {
    while (n-- && it.ref) _c_MEMB(_next)(&it);
    return it;
}

STC_INLINE _m_iter
_c_MEMB(_splice_range)(Self* self, _m_iter it,
                       Self* other, _m_iter it1, _m_iter it2) {
    Self tmp = _c_MEMB(_split_off)(other, it1, it2);
    return _c_MEMB(_splice)(self, it, &tmp);
}

#if defined _i_has_eq
STC_INLINE _m_iter
_c_MEMB(_find)(const Self* self, _m_raw val) {
    return _c_MEMB(_find_in)(self, _c_MEMB(_begin)(self), _c_MEMB(_end)(self), val);
}

STC_INLINE bool _c_MEMB(_eq)(const Self* self, const Self* other) {
    _m_iter i = _c_MEMB(_begin)(self), j = _c_MEMB(_begin)(other);
    for (; i.ref && j.ref; _c_MEMB(_next)(&i), _c_MEMB(_next)(&j)) {
        const _m_raw _rx = i_keytoraw(i.ref), _ry = i_keytoraw(j.ref);
        if (!(i_eq((&_rx), (&_ry)))) return false;
    }
    return !(i.ref || j.ref);
}
#endif

// -------------------------- IMPLEMENTATION -------------------------
#if defined i_implement

#ifndef i_no_clone
STC_DEF Self
_c_MEMB(_clone)(Self lst) {
    Self out = lst, *self = &out; (void)self; // may be used by i_keyclone via i_aux
    out.last = NULL;
    for (c_each(it, Self, lst))
        _c_MEMB(_push_back)(&out, i_keyclone((*it.ref)));
    return out;
}
#endif

STC_DEF void
_c_MEMB(_drop)(const Self* cself) {
    Self* self = (Self*)cself;
    while (self->last)
       _c_MEMB(_erase_after_node)(self, self->last);
}

STC_DEF _m_value*
_c_MEMB(_push_back)(Self* self, _m_value value) {
    _c_list_insert_entry_after(self->last, value);
    self->last = entry;
    return &entry->value;
}

STC_DEF _m_value*
_c_MEMB(_push_front)(Self* self, _m_value value) {
    _c_list_insert_entry_after(self->last, value);
    if (self->last == NULL)
        self->last = entry;
    return &entry->value;
}

STC_DEF _m_value*
_c_MEMB(_push_back_node)(Self* self, _m_node* node) {
    _c_list_insert_after_node(self->last, node);
    self->last = node;
    return &node->value;
}

STC_DEF _m_value*
_c_MEMB(_insert_after_node)(Self* self, _m_node* ref, _m_node* node) {
    _c_list_insert_after_node(ref, node);
    if (self->last == NULL)
        self->last = node;
    return &node->value;
}

STC_DEF _m_iter
_c_MEMB(_insert_at)(Self* self, _m_iter it, _m_value value) {
    _m_node* node = it.ref ? it.prev : self->last;
    _c_list_insert_entry_after(node, value);
    if (self->last == NULL || it.ref == NULL) {
        it.prev = self->last ? self->last : entry;
        self->last = entry;
    }
    it.ref = &entry->value;
    return it;
}

STC_DEF _m_iter
_c_MEMB(_erase_at)(Self* self, _m_iter it) {
    _m_node *node = _clist_tonode(it.ref);
    it.ref = (node == self->last) ? NULL : &node->next->value;
    _c_MEMB(_erase_after_node)(self, it.prev);
    return it;
}

STC_DEF _m_iter
_c_MEMB(_erase_range)(Self* self, _m_iter it1, _m_iter it2) {
    _m_node *end = it2.ref ? _clist_tonode(it2.ref) : self->last->next;
    if (it1.ref != it2.ref) do {
        _c_MEMB(_erase_after_node)(self, it1.prev);
        if (self->last == NULL) break;
    } while (it1.prev->next != end);
    return it2;
}

STC_DEF void
_c_MEMB(_erase_after_node)(Self* self, _m_node* ref) {
    _m_node* node = _c_MEMB(_unlink_after_node)(self, ref);
    i_keydrop((&node->value));
    _i_free_n(node, 1);
}

STC_DEF _m_node*
_c_MEMB(_unlink_after_node)(Self* self, _m_node* ref) {
    _m_node* node = ref->next, *next = node->next;
    ref->next = next;
    if (node == next)
        self->last = NULL;
    else if (node == self->last)
        self->last = ref;
    return node;
}

STC_DEF void
_c_MEMB(_reverse)(Self* self) {
    Self rev = *self;
    rev.last = NULL;
    while (self->last) {
        _m_node* node = _c_MEMB(_unlink_after_node)(self, self->last);
        _c_MEMB(_insert_after_node)(&rev, rev.last, node);
    }
    *self = rev;
}

STC_DEF _m_iter
_c_MEMB(_splice)(Self* self, _m_iter it, Self* other) {
    if (self->last == NULL)
        self->last = other->last;
    else if (other->last) {
        _m_node *p = it.ref ? it.prev : self->last, *next = p->next;
        it.prev = other->last;
        p->next = it.prev->next;
        it.prev->next = next;
        if (it.ref == NULL) self->last = it.prev;
    }
    other->last = NULL;
    return it;
}

STC_DEF Self
_c_MEMB(_split_off)(Self* self, _m_iter it1, _m_iter it2) {
    Self lst = *self;
    lst.last = NULL;
    if (it1.ref == it2.ref)
        return lst;
    _m_node *p1 = it1.prev,
            *p2 = it2.ref ? it2.prev : self->last;
    p1->next = p2->next;
    p2->next = _clist_tonode(it1.ref);
    if (self->last == p2)
        self->last = (p1 == p2) ? NULL : p1;
    lst.last = p2;
    return lst;
}

#if defined _i_has_eq
STC_DEF _m_iter
_c_MEMB(_find_in)(const Self* self, _m_iter it1, _m_iter it2, _m_raw val) {
    (void)self;
    for (c_each(it, Self, it1, it2)) {
        _m_raw r = i_keytoraw(it.ref);
        if (i_eq((&r), (&val)))
            return it;
    }
    it2.ref = NULL; return it2;
}

STC_DEF isize
_c_MEMB(_remove)(Self* self, _m_raw val) {
    isize n = 0;
    _m_node *prev = self->last, *node;
    if (prev) do {
        node = prev->next;
        _m_raw r = i_keytoraw((&node->value));
        if (i_eq((&r), (&val))) {
            _c_MEMB(_erase_after_node)(self, prev), ++n;
            if (self->last == NULL) break;
        } else
            prev = node;
    } while (node != self->last);
    return n;
}
#endif

#if defined _i_has_cmp
#include "priv/sort_prv.h"

STC_DEF bool _c_MEMB(_sort)(Self* self) {
    isize len = 0, cap = 0;
    _m_value *arr = NULL, *p = NULL;
    _m_node* keep;
    for (c_each(i, Self, *self)) {
        if (len == cap) {
            isize cap_n = cap + cap/2 + 8;
            if ((p = (_m_value *)_i_realloc_n(arr, cap, cap_n)) == NULL)
                goto done;
            arr = p, cap = cap_n;
        }
        arr[len++] = *i.ref;
    }
    keep = self->last;
    self->last = (_m_node *)arr;
    _c_MEMB(_sort_lowhigh)(self, 0, len - 1);
    self->last = keep;
    for (c_each(i, Self, *self))
        *i.ref = *p++;
    done: _i_free_n(arr, cap);
    return p != NULL;
}
#endif // _i_has_cmp
#endif // i_implement
#undef _i_is_list
#include "sys/finalize.h"
