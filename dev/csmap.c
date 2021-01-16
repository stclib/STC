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
#ifndef CSMAP__H__
#define CSMAP__H__

// Sorted/Ordered set and map - implemented as an AA-tree.
/*
#include <stdio.h>
#include <stc/csmap.h>
using_csset(i, int);        // Set of int
using_csmap(ic, int, char); // Map of int -> char

int main(void) {
    csset_sx s = csset_inits;
    csset_sx_insert(&s, 5);
    csset_sx_insert(&s, 8);
    c_foreach (i, csset_sx, s)
        printf("set %d\n", i.ref->second);
    csset_sx_del(&s);
}
*/
//#include "ccommon.h"
#include <stc/ccommon.h>
#include <stc/cstr.h>
#include <stdlib.h>
#include <string.h>

#define csmap_inits  {NULL, 0}
#define csset_inits  csmap_inits

#define using_csmap(...) \
    c_MACRO_OVERLOAD(using_csmap, __VA_ARGS__)

#define using_csmap_3(X, Key, Mapped) \
    using_csmap_5(X, Key, Mapped, c_default_del, c_default_clone)

#define using_csmap_5(X, Key, Mapped, mappedDel, mappedClone) \
    using_csmap_6(X, Key, Mapped, mappedDel, mappedClone, c_default_compare)

#define using_csmap_6(X, Key, Mapped, mappedDel, mappedClone, keyCompare) \
    using_csmap_8(X, Key, Mapped, mappedDel, mappedClone, keyCompare, c_default_del, c_default_clone)

#define using_csmap_8(X, Key, Mapped, mappedDel, mappedClone, keyCompare, keyDel, keyClone) \
    using_csmap_10(X, Key, Mapped, mappedDel, mappedClone, keyCompare, keyDel, \
                      keyClone, c_default_to_raw, Key)

#define using_csmap_10(X, Key, Mapped, mappedDel, mappedClone, keyCompareRaw, keyDel, \
                          keyFromRaw, keyToRaw, RawKey) \
    _using_CBST(X, csmap, Key, Mapped, mappedDel, keyCompareRaw, keyDel, \
                   keyFromRaw, keyToRaw, RawKey, mappedClone, c_default_to_raw, Mapped)

/* csset: */
#define using_csset(...) \
    c_MACRO_OVERLOAD(using_csset, __VA_ARGS__)

#define using_csset_2(X, Key) \
    using_csset_3(X, Key, c_default_compare)

#define using_csset_3(X, Key, keyCompare) \
    using_csset_5(X, Key, keyCompare, c_default_del, c_default_clone)

#define using_csset_5(X, Key, keyCompare, keyDel, keyClone) \
    using_csset_7(X, Key, keyCompare, keyDel, keyClone, c_default_to_raw, Key)

#define using_csset_7(X, Key, keyCompareRaw, keyDel, keyFromRaw, keyToRaw, RawKey) \
    _using_CBST(X, csset, Key, Key, _UNUSED_, keyCompareRaw, keyDel, \
                   keyFromRaw, keyToRaw, RawKey, _UNUSED_, _UNUSED_, void)

/* csset_str, csmap_str, csmap_strkey, csmap_strval: */
#define using_csset_str() \
    _using_CBST_strkey(str, csset, cstr_t, _UNUSED_, _UNUSED_)
#define using_csmap_str() \
    _using_CBST(str, csmap, cstr_t, cstr_t, cstr_del, cstr_compare_raw, cstr_del, \
                     cstr_from, cstr_to_raw, const char*, cstr_from, cstr_to_raw, const char*)

#define using_csmap_strkey(...) \
    c_MACRO_OVERLOAD(using_csmap_strkey, __VA_ARGS__)

#define using_csmap_strkey_2(X, Mapped) \
    _using_CBST_strkey(X, csmap, Mapped, c_default_del, c_default_clone)

#define using_csmap_strkey_4(X, Mapped, mappedDel, mappedClone) \
    _using_CBST_strkey(X, csmap, Mapped, mappedDel, mappedClone)

#define _using_CBST_strkey(X, C, Mapped, mappedDel, mappedClone) \
    _using_CBST(X, C, cstr_t, Mapped, mappedDel, cstr_compare_raw, cstr_del, \
                   cstr_from, cstr_to_raw, const char*, mappedClone, c_default_to_raw, Mapped)

#define using_csmap_strval(...) \
    c_MACRO_OVERLOAD(using_csmap_strval, __VA_ARGS__)

#define using_csmap_strval_2(X, Key) \
    using_csmap_strval_3(X, Key, c_default_compare)

#define using_csmap_strval_3(X, Key, keyCompare) \
    using_csmap_strval_5(X, Key, keyCompare, c_default_del, c_default_clone)

#define using_csmap_strval_5(X, Key, keyCompare, keyDel, keyClone) \
    using_csmap_strval_7(X, Key, keyCompare, keyDel, keyClone, c_default_to_raw, Key)

#define using_csmap_strval_7(X, Key, keyCompare, keyDel, keyFromRaw, keyToRaw, RawKey) \
    _using_CBST(X, csmap, Key, cstr_t, cstr_del, keyCompare, keyDel, \
                   keyFromRaw, keyToRaw, RawKey, cstr_from, cstr_to_raw, const char*)

#define SET_ONLY_csset(...) __VA_ARGS__
#define SET_ONLY_csmap(...)
#define MAP_ONLY_csset(...)
#define MAP_ONLY_csmap(...) __VA_ARGS__
#define KEY_REF_csset(vp)   (vp)
#define KEY_REF_csmap(vp)   (&(vp)->first)

#define _using_CBST_types(X, C, Key, Mapped) \
    typedef Key C##_##X##_key_t; \
    typedef Mapped C##_##X##_mapped_t; \
\
    typedef SET_ONLY_##C( C##_##X##_key_t ) \
            MAP_ONLY_##C( struct {C##_##X##_key_t first; \
                                  C##_##X##_mapped_t second;} ) \
    C##_##X##_value_t; \
\
    typedef struct C##_##X##_node { \
        struct C##_##X##_node *link[2]; \
        int level; \
        C##_##X##_value_t value; \
    } C##_##X##_node_t; \
\
    typedef struct { \
        C##_##X##_node_t* root; \
        size_t size; \
    } C##_##X; \
\
    typedef struct { \
        C##_##X##_value_t *ref; \
        int top; \
        C##_##X##_node_t *tn, *stk[34]; \
    } C##_##X##_iter_t


#define _using_CBST(X, C, Key, Mapped, mappedDel, keyCompareRaw, keyDel, \
                        keyFromRaw, keyToRaw, RawKey, mappedFromRaw, mappedToRaw, RawMapped) \
    _using_CBST_types(X, C, Key, Mapped); \
\
    typedef RawKey C##_##X##_rawkey_t; \
    typedef RawMapped C##_##X##_rawmapped_t; \
    typedef SET_ONLY_##C( C##_##X##_rawkey_t ) \
            MAP_ONLY_##C( struct {C##_##X##_rawkey_t first; \
                                  C##_##X##_rawmapped_t second;} ) \
    C##_##X##_rawvalue_t; \
\
    typedef struct { \
        C##_##X##_value_t *first; \
        bool second; \
    } C##_##X##_result_t; \
\
    STC_INLINE C##_##X \
    C##_##X##_init(void) {C##_##X m = csmap_inits; return m;} \
    STC_INLINE bool \
    C##_##X##_empty(C##_##X m) {return m.size == 0;} \
    STC_INLINE size_t \
    C##_##X##_size(C##_##X m) {return m.size;} \
\
    STC_API void \
    C##_##X##_del_priv_(C##_##X##_node_t* tn); \
\
    STC_INLINE void \
    C##_##X##_del(C##_##X* self) {C##_##X##_del_priv_(self->root);} \
    STC_INLINE void \
    C##_##X##_clear(C##_##X* self) {C##_##X##_del(self); self->size = 0;} \
\
    STC_INLINE void \
    C##_##X##_value_del(C##_##X##_value_t* val) { \
        keyDel(KEY_REF_##C(val)); \
        MAP_ONLY_##C( mappedDel(&val->second); ) \
    } \
    STC_INLINE C##_##X##_value_t \
    C##_##X##_value_clone(C##_##X##_value_t val) { \
        *KEY_REF_##C(&val) = keyFromRaw(keyToRaw(KEY_REF_##C(&val))); \
        MAP_ONLY_##C( val.second = mappedFromRaw(mappedToRaw(&val.second)); ) \
        return val; \
    } \
    STC_INLINE int \
    C##_##X##_node_compare_rkey(const C##_##X##_node_t* tn, const C##_##X##_rawkey_t* ry) { \
        C##_##X##_rawkey_t rx = keyToRaw(KEY_REF_##C(&tn->value)); \
        return keyCompareRaw(&rx, ry); \
    } \
\
    STC_INLINE void \
    C##_##X##_swap(C##_##X* a, C##_##X* b) {c_swap(C##_##X, *a, *b);} \
    STC_API C##_##X \
    C##_##X##_clone(C##_##X m); \
    STC_API void \
    C##_##X##_push_n(C##_##X* self, const C##_##X##_rawvalue_t arr[], size_t size); \
\
    STC_API C##_##X##_value_t* \
    C##_##X##_find_priv_(C##_##X##_node_t *tn, const C##_##X##_rawkey_t* rkey, C##_##X##_iter_t* it); \
\
    STC_INLINE C##_##X##_value_t* \
    C##_##X##_find(const C##_##X* self, RawKey rkey, C##_##X##_iter_t* it) { \
        return C##_##X##_find_priv_(self->root, &rkey, it); \
    } \
    STC_INLINE bool \
    C##_##X##_contains(const C##_##X* self, RawKey rkey) { \
        C##_##X##_iter_t it; \
        return C##_##X##_find_priv_(self->root, &rkey, &it) != NULL; \
    } \
\
    STC_API C##_##X##_result_t \
    C##_##X##_insert_key_(C##_##X* self, RawKey rkey); \
\
    STC_INLINE C##_##X##_result_t \
    C##_##X##_emplace(C##_##X* self, RawKey rkey MAP_ONLY_##C(, RawMapped rmapped) ) { \
        C##_##X##_result_t res = C##_##X##_insert_key_(self, rkey); \
        MAP_ONLY_##C( if (res.second) res.first->second = mappedFromRaw(rmapped); ) \
        return res; \
    } \
    STC_INLINE C##_##X##_result_t \
    C##_##X##_insert(C##_##X* self, C##_##X##_rawvalue_t raw) { \
        return SET_ONLY_##C( C##_##X##_insert_key_(self, raw) ) \
               MAP_ONLY_##C( C##_##X##_emplace(self, raw.first, raw.second) ); \
    } \
\
    MAP_ONLY_##C( \
    STC_INLINE C##_##X##_result_t \
    C##_##X##_put(C##_##X* self, RawKey rkey, RawMapped rmapped) { \
        C##_##X##_result_t res = C##_##X##_insert_key_(self, rkey); \
        if (!res.second) mappedDel(&res.first->second); \
        res.first->second = mappedFromRaw(rmapped); return res; \
    } \
    STC_INLINE C##_##X##_result_t \
    C##_##X##_insert_or_assign(C##_##X* self, RawKey rkey, RawMapped rmapped) { \
        return C##_##X##_put(self, rkey, rmapped); \
    } \
    STC_INLINE C##_##X##_result_t \
    C##_##X##_put_mapped(C##_##X* self, RawKey rkey, Mapped mapped) { \
        C##_##X##_result_t res = C##_##X##_insert_key_(self, rkey); \
        if (!res.second) mappedDel(&res.first->second); \
        res.first->second = mapped; return res; \
    } \
    STC_INLINE C##_##X##_mapped_t* \
    C##_##X##_at(const C##_##X* self, RawKey rkey) { \
        return NULL; \
    }) \
\
    STC_INLINE C##_##X##_value_t* \
    C##_##X##_front(C##_##X* self) { \
        C##_##X##_node_t *tn = self->root; \
        while (tn->link[0]->level) tn = tn->link[0]; \
        return &tn->value; \
    } \
    STC_INLINE C##_##X##_value_t* \
    C##_##X##_back(C##_##X* self) { \
        C##_##X##_node_t *tn = self->root; \
        while (tn->link[1]->level) tn = tn->link[1]; \
        return &tn->value; \
    } \
\
    STC_INLINE void \
    C##_##X##_next(C##_##X##_iter_t* it) { \
        cbst_next((csmap___iter_t*) it); \
    } \
    STC_INLINE C##_##X##_iter_t \
    C##_##X##_begin(C##_##X* self) { \
        C##_##X##_iter_t it = {NULL, 0, self->root}; \
        cbst_next((csmap___iter_t*) &it); \
        return it; \
    } \
    STC_INLINE C##_##X##_iter_t \
    C##_##X##_end(C##_##X* self) {\
        C##_##X##_iter_t it = {NULL}; return it; \
    } \
    STC_INLINE C##_##X##_mapped_t* \
    C##_##X##_itval(C##_##X##_iter_t it) {return MAP_ONLY_##C( &it.ref->second ) \
                                                 SET_ONLY_##C( it.ref );} \
\
    STC_INLINE size_t \
    C##_##X##_erase(C##_##X* self, RawKey rkey) { \
        return 0; \
    } \
    STC_INLINE C##_##X##_iter_t \
    C##_##X##_erase_at(C##_##X* self, C##_##X##_iter_t pos) { \
        /*C##_##X##_erase_entry(self, pos.ref);*/ \
        C##_##X##_next(&pos); return pos; \
    } \
\
    _implement_CBST(X, C, Key, Mapped, mappedDel, keyCompareRaw, keyDel, \
                       keyFromRaw, keyToRaw, RawKey, mappedFromRaw, mappedToRaw, RawMapped) \
    typedef C##_##X C##_##X##_t

/* -------------------------- IMPLEMENTATION ------------------------- */

#if !defined(STC_HEADER) || defined(STC_IMPLEMENTATION)
#define _implement_CBST(X, C, Key, Mapped, mappedDel, keyCompareRaw, keyDel, \
                           keyFromRaw, keyToRaw, RawKey, mappedFromRaw, mappedToRaw, RawMapped) \
    STC_DEF void \
    C##_##X##_push_n(C##_##X* self, const C##_##X##_rawvalue_t arr[], size_t n) { \
        for (size_t i=0; i<n; ++i) MAP_ONLY_##C( C##_##X##_put(self, arr[i].first, arr[i].second) ) \
                                   SET_ONLY_##C( C##_##X##_insert(self, arr[i]) ) ; \
    } \
    STC_DEF void \
    C##_##X##_del_priv_(C##_##X##_node_t* tn) { \
        if (tn->level != 0) { \
            C##_##X##_del_priv_(tn->link[0]); \
            C##_##X##_del_priv_(tn->link[1]); \
            C##_##X##_value_del(&tn->value); \
            c_free(tn); \
        } \
    } \
\
    STC_DEF C##_##X##_node_t* \
    C##_##X##_insert_priv_(C##_##X##_node_t* tn, C##_##X##_rawvalue_t* rval, bool overwrite) { \
        if (tn->level == 0) { \
            tn = c_new_1(C##_##X##_node_t); \
            tn->link[0] = tn->link[1] = (C##_##X##_node_t*) &cbst_nil, tn->level = 1; \
            *KEY_REF_##C(&tn->value) = keyFromRaw(*KEY_REF_##C(rval)); \
            MAP_ONLY_##C( tn->value.second = mappedFromRaw(rval->second); ) \
            return tn; \
        } \
        C##_##X##_rawkey_t rkey = keyToRaw(KEY_REF_##C(&tn->value)); \
        int cmp = keyCompareRaw(&rkey, KEY_REF_##C(rval)); \
        if (cmp == 0) { \
            MAP_ONLY_##C( if (overwrite) { mappedDel(&tn->value.second); \
                                           tn->value.second = mappedFromRaw(rval->second); }) \
        } else { \
            tn->link[cmp == -1] = C##_##X##_insert_priv_(tn->link[cmp == -1], rval, overwrite); \
            tn = (C##_##X##_node_t*) cbst_skew((csmap___node_t*) tn); \
            tn = (C##_##X##_node_t*) cbst_split((csmap___node_t*) tn); \
        } \
        return tn; \
    } \
\
    STC_DEF C##_##X##_value_t* \
    C##_##X##_find_priv_(C##_##X##_node_t *tn, const C##_##X##_rawkey_t* rkey, C##_##X##_iter_t* it) { \
        it->top = 0; \
        while (tn->level) \
            switch (C##_##X##_node_compare_rkey(tn, rkey)) { \
                case -1: tn = tn->link[1]; break; \
                case 1: it->stk[it->top++] = tn; tn = tn->link[0]; break; \
                case 0: it->ref = &tn->value; it->tn = tn->link[1]; return it->ref; \
            } \
        return (it->ref = NULL); \
    } \
\
    STC_DEF C##_##X##_result_t \
    C##_##X##_insert_key_(C##_##X* self, RawKey rkey) { \
        C##_##X##_result_t res = {NULL, false}; \
        if (res.second) { \
            *KEY_REF_##C(res.first) = keyFromRaw(rkey); \
            ++self->size; \
        } \
        return res; \
    } \
\
    STC_DEF C##_##X##_node_t * \
    C##_##X##_clone_nodes(C##_##X##_node_t *tn) { \
        if (! tn->level) return tn; \
        C##_##X##_node_t *x = c_new_1(C##_##X##_node_t); \
        x->link[0] = C##_##X##_clone_nodes(tn->link[0]); \
        x->link[1] = C##_##X##_clone_nodes(tn->link[1]); \
        x->level = tn->level; \
        x->value = C##_##X##_value_clone(tn->value); \
        return x; \
    } \
\
    STC_DEF C##_##X \
    C##_##X##_clone(C##_##X bst) { \
        C##_##X clone = {C##_##X##_clone_nodes(bst.root), bst.size}; \
        return clone; \
    }

_using_CBST_types(_, csmap, int, int);
static csmap___node_t cbst_nil = {&cbst_nil, &cbst_nil, 0};

/*
STC_DEF csmap___node_t *
cbst_remove(csmap___node_t *tn, csmap___value_t value) {
    if (tn->level == 0)
        return tn;
    int cmp = c_default_compare(&tn->value, &value);
    if (cmp != 0)
        tn->link[cmp == -1] = cbst_remove(tn->link[cmp == -1], value);
    else { // found
        if (tn->link[0]->level && tn->link[1]->level) {
            csmap___node_t *h = tn->link[0];
            while (h->link[1]->level)
                h = h->link[1];
            tn->value = h->value;
            tn->link[0] = cbst_remove(tn->link[0], tn->value);
        } else {
            csmap___node_t *tmp = tn;
            tn = tn->link[tn->link[0]->level == 0];
            free(tmp);
        }
    }
    if (tn->link[0]->level < tn->level - 1 || tn->link[1]->level < tn->level - 1) {
        if (tn->link[1]->level > --tn->level)
            tn->link[1]->level = tn->level;
        tn = cbst_skew(tn);
        tn = cbst_split(tn);
    }
    return tn;
}

*/

STC_DEF csmap___node_t *
cbst_skew(csmap___node_t *tn) {
    if (tn->link[0]->level == tn->level && tn->level) {
        csmap___node_t *tmp = tn->link[0];
        tn->link[0] = tmp->link[1];
        tmp->link[1] = tn;
        tn = tmp;
    }
    return tn;
}

STC_DEF csmap___node_t *
cbst_split(csmap___node_t *tn) {
    if (tn->link[1]->link[1]->level == tn->level && tn->level) {
        csmap___node_t *tmp = tn->link[1];
        tn->link[1] = tmp->link[0];
        tmp->link[0] = tn;
        tn = tmp;
        ++tn->level;
    }
    return tn;
}

STC_DEF void
cbst_next(csmap___iter_t *it) {
    csmap___node_t *tn = it->tn;
    if (tn->level == 0 && it->top == 0)
        it->ref = NULL;
    else {
        while (tn->level) {
            it->stk[it->top++] = tn;
            tn = tn->link[0];
        }
        tn = it->stk[--it->top];
        it->ref = &tn->value;
        it->tn = tn->link[1];
    }
}


#else
#define _implement_CBST(X, C, Key, Mapped, mappedDel, keyCompareRaw, keyDel, \
                           keyFromRaw, keyToRaw, RawKey, mappedFromRaw, mappedToRaw, RawMapped)
#endif

#endif

using_csset(i, int);
using_csmap(ii, int, int);
using_csmap(ss, cstr, cstr, cstr_del, cstr_clone, cstr_compare_ref, cstr_del, cstr_clone);

int main()
{

}