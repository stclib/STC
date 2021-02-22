/* MIT License
 *
 * Copyright (c) 2021 Tyge Løvset, NORCE, www.norceresearch.no
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
#ifndef CVEC__H__
#define CVEC__H__

#include "ccommon.h"
#include <stdlib.h>
#include <string.h>

#define using_cvec(...) c_MACRO_OVERLOAD(using_cvec, __VA_ARGS__)
#define using_cvec_2(X, Value) \
                    using_cvec_3(X, Value, c_default_compare)
#define using_cvec_3(X, Value, valueCompare) \
                    using_cvec_7(X, Value, valueCompare, c_plain_del, c_plain_fromraw, c_plain_toraw, Value)
#define using_cvec_5(X, Value, valueCompare, valueDel, valueClone) \
                    using_cvec_7(X, Value, valueCompare, valueDel, valueClone, c_plain_toraw, Value)
#define using_cvec_str() \
                    using_cvec_7(str, cstr_t, cstr_compare_raw, cstr_del, cstr_from, cstr_c_str, const char*)

#define typedefs_cvec(X, Value, RawValue) \
    typedef Value cvec_##X##_value_t; \
    typedef RawValue cvec_##X##_rawvalue_t; \
    typedef struct { cvec_##X##_value_t *ref; } cvec_##X##_iter_t; \
    typedef struct { \
        cvec_##X##_value_t* data; \
    } cvec_##X
    
struct cvec_rep { size_t size, cap; void* data[]; };
#define cvec_rep_(self) c_container_of((self)->data, struct cvec_rep, data)
typedef int (*c_cmp_fn)(const void*, const void*);

#define using_cvec_7(X, Value, valueCompareRaw, valueDel, valueFromRaw, valueToRaw, RawValue) \
    typedefs_cvec(X, Value, RawValue); \
\
    STC_API cvec_##X \
    cvec_##X##_init(void); \
    STC_INLINE size_t \
    cvec_##X##_size(cvec_##X vec) { return cvec_rep_(&vec)->size; } \
    STC_INLINE size_t \
    cvec_##X##_capacity(cvec_##X vec) { return cvec_rep_(&vec)->cap; } \
    STC_INLINE bool \
    cvec_##X##_empty(cvec_##X vec) {return !cvec_rep_(&vec)->size;} \
    STC_INLINE Value \
    cvec_##X##_value_fromraw(RawValue raw) {return valueFromRaw(raw);} \
    STC_INLINE cvec_##X##_value_t \
    cvec_##X##_value_clone(cvec_##X##_value_t val) {return valueFromRaw(valueToRaw(&val));} \
    STC_INLINE void \
    cvec_##X##_clear(cvec_##X* self); \
    STC_API void \
    cvec_##X##_del(cvec_##X* self); \
    STC_API void \
    cvec_##X##_reserve(cvec_##X* self, size_t cap); \
    STC_API void \
    cvec_##X##_resize(cvec_##X* self, size_t size, Value fill_val); \
    STC_INLINE void \
    cvec_##X##_swap(cvec_##X* a, cvec_##X* b) {c_swap(cvec_##X##_value_t*, a->data, b->data);} \
\
    STC_INLINE cvec_##X \
    cvec_##X##_with_size(size_t size, Value null_val) { \
        cvec_##X x = cvec_##X##_init(); \
        cvec_##X##_resize(&x, size, null_val); \
        return x; \
    } \
    STC_INLINE cvec_##X \
    cvec_##X##_with_capacity(size_t size) { \
        cvec_##X x = cvec_##X##_init(); \
        cvec_##X##_reserve(&x, size); \
        return x; \
    } \
    STC_API cvec_##X \
    cvec_##X##_clone(cvec_##X vec); \
\
    STC_INLINE void \
    cvec_##X##_shrink_to_fit(cvec_##X *self) { \
        cvec_##X x = cvec_##X##_clone(*self); \
        cvec_##X##_del(self); *self = x; \
    } \
    STC_API void \
    cvec_##X##_emplace_n(cvec_##X *self, const cvec_##X##_rawvalue_t arr[], size_t size); \
    STC_API void \
    cvec_##X##_push_back(cvec_##X* self, Value value); \
    STC_INLINE void \
    cvec_##X##_emplace_back(cvec_##X* self, RawValue raw) { \
        cvec_##X##_push_back(self, valueFromRaw(raw)); \
    } \
    STC_INLINE void \
    cvec_##X##_pop_back(cvec_##X* self) { \
        valueDel(&self->data[--cvec_rep_(self)->size]); \
    } \
\
    STC_API cvec_##X##_iter_t \
    cvec_##X##_insert_range_p(cvec_##X* self, cvec_##X##_value_t* pos, const cvec_##X##_value_t* pfirst, const cvec_##X##_value_t* pfinish); \
\
    STC_INLINE cvec_##X##_iter_t \
    cvec_##X##_insert_range(cvec_##X* self, cvec_##X##_iter_t pos, cvec_##X##_iter_t first, cvec_##X##_iter_t finish) { \
        return cvec_##X##_insert_range_p(self, pos.ref, first.ref, finish.ref); \
    } \
    STC_INLINE cvec_##X##_iter_t \
    cvec_##X##_insert_at(cvec_##X* self, cvec_##X##_iter_t pos, Value value) { \
        return cvec_##X##_insert_range_p(self, pos.ref, &value, &value + 1); \
    } \
    STC_INLINE cvec_##X##_iter_t \
    cvec_##X##_insert(cvec_##X* self, size_t idx, Value value) { \
        return cvec_##X##_insert_range_p(self, self->data + idx, &value, &value + 1); \
    } \
    STC_INLINE cvec_##X##_iter_t \
    cvec_##X##_emplace_at(cvec_##X* self, cvec_##X##_iter_t pos, RawValue raw) { \
        return cvec_##X##_insert_at(self, pos, valueFromRaw(raw)); \
    } \
    STC_INLINE cvec_##X##_iter_t \
    cvec_##X##_emplace(cvec_##X* self, size_t idx, RawValue raw) { \
        return cvec_##X##_insert(self, idx, valueFromRaw(raw)); \
    } \
\
    STC_API cvec_##X##_iter_t \
    cvec_##X##_erase_range_p(cvec_##X* self, cvec_##X##_value_t* first, cvec_##X##_value_t* finish); \
\
    STC_INLINE cvec_##X##_iter_t \
    cvec_##X##_erase_range(cvec_##X* self, cvec_##X##_iter_t first, cvec_##X##_iter_t finish) { \
        return cvec_##X##_erase_range_p(self, first.ref, finish.ref); \
    } \
    STC_INLINE cvec_##X##_iter_t \
    cvec_##X##_erase_at(cvec_##X* self, cvec_##X##_iter_t pos) { \
        return cvec_##X##_erase_range_p(self, pos.ref, pos.ref + 1); \
    } \
    STC_INLINE cvec_##X##_iter_t \
    cvec_##X##_erase(cvec_##X* self, size_t idx, size_t n) { \
        return cvec_##X##_erase_range_p(self, self->data + idx, self->data + idx + n); \
    } \
\
    STC_INLINE cvec_##X##_value_t* \
    cvec_##X##_front(cvec_##X* self) {return self->data;} \
    STC_INLINE cvec_##X##_value_t* \
    cvec_##X##_back(cvec_##X* self) {return self->data + cvec_rep_(self)->size - 1;} \
    STC_INLINE cvec_##X##_value_t* \
     cvec_##X##_at(cvec_##X* self, size_t i) { \
        assert(i < cvec_rep_(self)->size); \
        return self->data + i; \
    } \
\
    STC_INLINE cvec_##X##_iter_t \
    cvec_##X##_begin(const cvec_##X* self) { \
        cvec_##X##_iter_t it = {self->data}; return it; \
    } \
    STC_INLINE cvec_##X##_iter_t \
    cvec_##X##_end(const cvec_##X* self) { \
        cvec_##X##_iter_t it = {self->data + cvec_rep_(self)->size}; return it; \
    } \
    STC_INLINE void \
    cvec_##X##_next(cvec_##X##_iter_t* it) {++it->ref;} \
    STC_INLINE cvec_##X##_value_t* \
    cvec_##X##_itval(cvec_##X##_iter_t it) {return it.ref;} \
    STC_INLINE size_t \
    cvec_##X##_index(cvec_##X vec, cvec_##X##_iter_t it) {return it.ref - vec.data;} \
\
    STC_API cvec_##X##_iter_t \
    cvec_##X##_find_in_range(cvec_##X##_iter_t first, cvec_##X##_iter_t finish, RawValue raw); \
    STC_INLINE cvec_##X##_iter_t \
    cvec_##X##_find(const cvec_##X* self, RawValue raw) { \
        return cvec_##X##_find_in_range(cvec_##X##_begin(self), cvec_##X##_end(self), raw); \
    } \
    STC_API int \
    cvec_##X##_value_compare(const cvec_##X##_value_t* x, const cvec_##X##_value_t* y); \
    STC_API cvec_##X##_iter_t \
    cvec_##X##_bsearch_in_range(cvec_##X##_iter_t i1, cvec_##X##_iter_t i2, RawValue raw); \
    STC_INLINE cvec_##X##_iter_t \
    cvec_##X##_bsearch(const cvec_##X* self, RawValue raw) { \
        return cvec_##X##_bsearch_in_range(cvec_##X##_begin(self), cvec_##X##_end(self), raw); \
    } \
    STC_INLINE void \
    cvec_##X##_sort_range(cvec_##X##_iter_t i1, cvec_##X##_iter_t i2, \
                          int(*cmp)(const cvec_##X##_value_t*, const cvec_##X##_value_t*)) { \
        qsort(i1.ref, i2.ref - i1.ref, sizeof(cvec_##X##_value_t), (c_cmp_fn) cmp); \
    } \
    STC_INLINE void \
    cvec_##X##_sort(cvec_##X* self) { \
        cvec_##X##_sort_range(cvec_##X##_begin(self), cvec_##X##_end(self), cvec_##X##_value_compare); \
    } \
\
    _c_implement_cvec_7(X, Value, valueCompareRaw, valueDel, valueFromRaw, valueToRaw, RawValue) \
    typedef cvec_##X cvec_##X##_t

/* -------------------------- IMPLEMENTATION ------------------------- */

#if !defined(STC_HEADER) || defined(STC_IMPLEMENTATION)
static struct cvec_rep _cvec_inits = {0, 0};

#define _c_implement_cvec_7(X, Value, valueCompareRaw, valueDel, valueFromRaw, valueToRaw, RawValue) \
\
    STC_DEF cvec_##X \
    cvec_##X##_init(void) { \
        cvec_##X vec = {(cvec_##X##_value_t *) _cvec_inits.data}; return vec; \
    } \
\
    STC_DEF void \
    cvec_##X##_emplace_n(cvec_##X *self, const cvec_##X##_rawvalue_t arr[], size_t n) { \
        if (!n) return; \
        cvec_##X##_reserve(self, cvec_rep_(self)->size + n); \
        cvec_##X##_value_t* p = self->data + cvec_rep_(self)->size; \
        for (size_t i=0; i < n; ++i) *p++ = valueFromRaw(arr[i]); \
        cvec_rep_(self)->size += n; \
    } \
\
    STC_DEF void \
    cvec_##X##_clear(cvec_##X* self) { \
        struct cvec_rep* rep = cvec_rep_(self); if (rep->cap) { \
            for (cvec_##X##_value_t *p = self->data, *q = p + rep->size; p != q; ++p) \
                valueDel(p); \
            rep->size = 0; \
        } \
    } \
    STC_DEF void \
    cvec_##X##_del(cvec_##X* self) { \
        cvec_##X##_clear(self); \
        if (cvec_rep_(self)->cap) \
            c_free(cvec_rep_(self)); \
    } \
\
    STC_DEF void \
    cvec_##X##_reserve(cvec_##X* self, size_t cap) { \
        struct cvec_rep* rep = cvec_rep_(self); \
        size_t len = rep->size, oldcap = rep->cap; \
        if (cap > oldcap) { \
            rep = (struct cvec_rep*) c_realloc(oldcap ? rep : NULL, \
                                               sizeof(struct cvec_rep) + cap*sizeof(Value)); \
            self->data = (cvec_##X##_value_t*) rep->data; \
            rep->size = len; \
            rep->cap = cap; \
        } \
    } \
    STC_DEF void \
    cvec_##X##_resize(cvec_##X* self, size_t len, Value null_val) { \
        cvec_##X##_reserve(self, len); \
        struct cvec_rep* rep = cvec_rep_(self); \
        size_t i, n = rep->size; \
        for (i = len; i < n; ++i) valueDel(self->data + i); \
        for (i = n; i < len; ++i) self->data[i] = null_val; \
        if (rep->cap) rep->size = len; \
    } \
\
    STC_DEF void \
    cvec_##X##_push_back(cvec_##X* self, Value value) { \
        size_t len = cvec_rep_(self)->size; \
        if (len == cvec_##X##_capacity(*self)) \
            cvec_##X##_reserve(self, 4 + len*1.5); \
        self->data[cvec_rep_(self)->size++] = value; \
    } \
\
    STC_DEF cvec_##X \
    cvec_##X##_clone(cvec_##X vec) { \
        size_t len = cvec_rep_(&vec)->size; \
        cvec_##X out = cvec_##X##_with_capacity(len); \
        cvec_##X##_insert_range_p(&out, out.data, vec.data, vec.data + len); \
        return out; \
    } \
\
    STC_DEF cvec_##X##_iter_t \
    cvec_##X##_insert_range_p(cvec_##X* self, cvec_##X##_value_t* pos, const cvec_##X##_value_t* first, const cvec_##X##_value_t* finish) { \
        size_t len = finish - first, idx = pos - self->data, size = cvec_rep_(self)->size; \
        cvec_##X##_iter_t it = {pos}; \
        if (len == 0) return it; \
        if (size + len > cvec_##X##_capacity(*self)) \
            cvec_##X##_reserve(self, 4 + (size + len)*1.5), \
            it.ref = pos = self->data + idx; \
        cvec_rep_(self)->size += len; \
        memmove(pos + len, pos, (size - idx) * sizeof(Value)); \
        while (first != finish) \
            *pos++ = valueFromRaw(valueToRaw(first++)); \
        return it; \
    } \
\
    STC_DEF cvec_##X##_iter_t \
    cvec_##X##_erase_range_p(cvec_##X* self, cvec_##X##_value_t* first, cvec_##X##_value_t* finish) { \
        intptr_t len = finish - first; \
        if (len > 0) { \
            cvec_##X##_value_t* p = first, *end = self->data + cvec_rep_(self)->size; \
            while (p != finish) valueDel(p++); \
            memmove(first, finish, (end - finish) * sizeof(Value)); \
            cvec_rep_(self)->size -= len; \
        } \
        cvec_##X##_iter_t it = {first}; return it; \
    } \
\
    STC_DEF cvec_##X##_iter_t \
    cvec_##X##_find_in_range(cvec_##X##_iter_t i1, cvec_##X##_iter_t i2, RawValue raw) { \
        for (; i1.ref != i2.ref; ++i1.ref) { \
            RawValue r = valueToRaw(i1.ref); \
            if (valueCompareRaw(&raw, &r) == 0) return i1; \
        } \
        return i2; \
    } \
    STC_DEF cvec_##X##_iter_t \
    cvec_##X##_bsearch_in_range(cvec_##X##_iter_t i1, cvec_##X##_iter_t i2, RawValue raw) { \
        cvec_##X##_iter_t mid, last = i2; \
        while (i1.ref != i2.ref) { \
            mid.ref = i1.ref + ((i2.ref - i1.ref)>>1); \
            RawValue m = valueToRaw(mid.ref); \
            switch (valueCompareRaw(&raw, &m)) { \
                case 0: return mid; \
                case -1: i2.ref = mid.ref; break; \
                case 1: i1.ref = mid.ref + 1; \
            } \
        } \
        return last; \
    } \
\
    STC_DEF int \
    cvec_##X##_value_compare(const cvec_##X##_value_t* x, const cvec_##X##_value_t* y) { \
        RawValue rx = valueToRaw(x); \
        RawValue ry = valueToRaw(y); \
        return valueCompareRaw(&rx, &ry); \
    }

#else
#define _c_implement_cvec_7(X, Value, valueCompareRaw, valueDel, valueFromRaw, valueToRaw, RawValue)
#endif

#endif
