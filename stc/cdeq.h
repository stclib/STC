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
#ifndef CDEQ__H__
#define CDEQ__H__

#include "ccommon.h"
#include <stdlib.h>
#include <string.h>

#define cdeq_inits          {NULL, NULL}
#define cdeq_size(v)        _cdeq_safe_size((v).base)
#define cdeq_empty(v)       (cdeq_size(v) == 0)

#define using_cdeq(...) c_MACRO_OVERLOAD(using_cdeq, __VA_ARGS__)
#define using_cdeq_2(X, Value) \
                    using_cdeq_3(X, Value, c_default_del)
#define using_cdeq_3(X, Value, valueDestroy) \
                    using_cdeq_4(X, Value, valueDestroy, c_default_compare)
#define using_cdeq_4(X, Value, valueDestroy, valueCompare) \
                    using_cdeq_7(X, Value, valueDestroy, valueCompare, Value, c_default_to_raw, c_default_from_raw)
#define using_cdeq_str() \
                    using_cdeq_7(str, cstr_t, cstr_del, cstr_compare_raw, const char*, cstr_to_raw, cstr_from)


#define using_cdeq_7(X, Value, valueDestroy, valueCompareRaw, RawValue, valueToRaw, valueFromRaw) \
\
    typedef Value cdeq_##X##_value_t; \
    typedef RawValue cdeq_##X##_rawvalue_t; \
    typedef cdeq_##X##_rawvalue_t cdeq_##X##_input_t; \
    typedef struct { cdeq_##X##_value_t *ref; } cdeq_##X##_iter_t; \
\
    typedef struct { \
        cdeq_##X##_value_t* data, *base; \
    } cdeq_##X; \
\
    STC_INLINE cdeq_##X \
    cdeq_##X##_init(void) {cdeq_##X v = cdeq_inits; return v;} \
    STC_INLINE bool \
    cdeq_##X##_empty(cdeq_##X v) {return cdeq_empty(v);} \
    STC_INLINE size_t \
    cdeq_##X##_size(cdeq_##X v) {return cdeq_size(v);} \
    STC_INLINE Value \
    cdeq_##X##_value_from_raw(RawValue rawValue) {return valueFromRaw(rawValue);} \
    STC_INLINE void \
    cdeq_##X##_clear(cdeq_##X* self); \
    STC_API void \
    cdeq_##X##_del(cdeq_##X* self); \
    STC_API void \
    cdeq_##X##_resize(cdeq_##X* self, size_t size, Value fill_val); \
    STC_API void \
    _cdeq_##X##_expand(cdeq_##X* self, size_t n, bool front); \
    STC_INLINE void \
    cdeq_##X##_swap(cdeq_##X* a, cdeq_##X* b) {c_swap(cdeq_##X, *a, *b);} \
\
    STC_INLINE cdeq_##X \
    cdeq_##X##_with_size(size_t size, Value null_val) { \
        cdeq_##X x = cdeq_inits; \
        cdeq_##X##_resize(&x, size, null_val); \
        return x; \
    } \
    STC_INLINE cdeq_##X \
    cdeq_##X##_with_capacity(size_t size) { \
        cdeq_##X x = cdeq_inits; \
        _cdeq_##X##_expand(&x, size, false); \
        return x; \
    } \
    STC_API cdeq_##X \
    cdeq_##X##_clone(cdeq_##X vec); \
\
    STC_INLINE void \
    cdeq_##X##_shrink_to_fit(cdeq_##X *self) { \
        cdeq_##X x = cdeq_##X##_clone(*self); \
        cdeq_##X##_del(self); *self = x; \
    } \
\
    STC_API void \
    cdeq_##X##_push_n(cdeq_##X *self, const cdeq_##X##_input_t arr[], size_t n); \
    STC_API void \
    cdeq_##X##_push_back(cdeq_##X* self, Value value); \
    STC_INLINE void \
    cdeq_##X##_emplace_back(cdeq_##X* self, RawValue rawValue) { \
        cdeq_##X##_push_back(self, valueFromRaw(rawValue)); \
    } \
    STC_INLINE void \
    cdeq_##X##_pop_back(cdeq_##X* self) { \
        valueDestroy(&self->data[--_cdeq_size(self)]); \
    } \
\
    STC_INLINE void \
    cdeq_##X##_push_front(cdeq_##X* self, Value value); \
    STC_INLINE void \
    cdeq_##X##_emplace_front(cdeq_##X* self, RawValue rawValue) { \
        cdeq_##X##_push_front(self, valueFromRaw(rawValue)); \
    } \
    STC_INLINE void \
    cdeq_##X##_pop_front(cdeq_##X* self) { \
        valueDestroy(self->data++); \
        --_cdeq_size(self); \
    } \
\
    STC_API cdeq_##X##_iter_t \
    cdeq_##X##_insert_range_p(cdeq_##X* self, cdeq_##X##_value_t* pos, const cdeq_##X##_value_t* pfirst, const cdeq_##X##_value_t* pfinish); \
\
    STC_INLINE cdeq_##X##_iter_t \
    cdeq_##X##_insert_range(cdeq_##X* self, cdeq_##X##_iter_t pos, cdeq_##X##_iter_t first, cdeq_##X##_iter_t finish) { \
        return cdeq_##X##_insert_range_p(self, pos.ref, first.ref, finish.ref); \
    } \
    STC_INLINE cdeq_##X##_iter_t \
    cdeq_##X##_insert(cdeq_##X* self, cdeq_##X##_iter_t pos, Value value) { \
        return cdeq_##X##_insert_range_p(self, pos.ref, &value, &value + 1); \
    } \
    STC_INLINE cdeq_##X##_iter_t \
    cdeq_##X##_insert_at(cdeq_##X* self, size_t idx, Value value) { \
        return cdeq_##X##_insert_range_p(self, self->data + idx, &value, &value + 1); \
    } \
    STC_INLINE cdeq_##X##_iter_t \
    cdeq_##X##_emplace(cdeq_##X* self, cdeq_##X##_iter_t pos, RawValue rawValue) { \
        return cdeq_##X##_insert(self, pos, valueFromRaw(rawValue)); \
    } \
    STC_INLINE cdeq_##X##_iter_t \
    cdeq_##X##_emplace_at(cdeq_##X* self, size_t idx, RawValue rawValue) { \
        return cdeq_##X##_insert_at(self, idx, valueFromRaw(rawValue)); \
    } \
\
    STC_API cdeq_##X##_iter_t \
    cdeq_##X##_erase_range_p(cdeq_##X* self, cdeq_##X##_value_t* first, cdeq_##X##_value_t* finish); \
\
    STC_INLINE cdeq_##X##_iter_t \
    cdeq_##X##_erase_range(cdeq_##X* self, cdeq_##X##_iter_t first, cdeq_##X##_iter_t finish) { \
        return cdeq_##X##_erase_range_p(self, first.ref, finish.ref); \
    } \
    STC_INLINE cdeq_##X##_iter_t \
    cdeq_##X##_erase(cdeq_##X* self, cdeq_##X##_iter_t pos) { \
        return cdeq_##X##_erase_range_p(self, pos.ref, pos.ref + 1); \
    } \
    STC_INLINE cdeq_##X##_iter_t \
    cdeq_##X##_erase_n(cdeq_##X* self, size_t idx, size_t n) { \
        return cdeq_##X##_erase_range_p(self, self->data + idx, self->data + idx + n); \
    } \
\
    STC_API cdeq_##X##_iter_t \
    cdeq_##X##_find(const cdeq_##X* self, RawValue rawValue); \
    STC_API cdeq_##X##_iter_t \
    cdeq_##X##_find_in_range(const cdeq_##X* self, cdeq_##X##_iter_t first, cdeq_##X##_iter_t finish, RawValue rawValue); \
\
    STC_INLINE cdeq_##X##_value_t* \
    cdeq_##X##_front(cdeq_##X* self) {return self->data;} \
    STC_INLINE cdeq_##X##_value_t* \
    cdeq_##X##_back(cdeq_##X* self) {return self->data + _cdeq_size(self) - 1;} \
    STC_INLINE cdeq_##X##_value_t* \
    cdeq_##X##_at(cdeq_##X* self, size_t i) { \
        assert(i < cdeq_size(*self)); \
        return self->data + i; \
    } \
\
    STC_API int \
    cdeq_##X##_value_compare(const cdeq_##X##_value_t* x, const cdeq_##X##_value_t* y); \
    STC_INLINE void \
    cdeq_##X##_sort_with(cdeq_##X* self, size_t ifirst, size_t ifinish, int(*cmp)(const cdeq_##X##_value_t*, const cdeq_##X##_value_t*)) { \
        qsort(self->data + ifirst, ifinish - ifirst, sizeof(Value), (_cdeq_cmp) cmp); \
    } \
    STC_INLINE void \
    cdeq_##X##_sort(cdeq_##X* self) { \
        cdeq_##X##_sort_with(self, 0, cdeq_size(*self), cdeq_##X##_value_compare); \
    } \
\
    STC_INLINE cdeq_##X##_iter_t \
    cdeq_##X##_begin(const cdeq_##X* self) { \
        cdeq_##X##_iter_t it = {self->data}; return it; \
    } \
    STC_INLINE cdeq_##X##_iter_t \
    cdeq_##X##_end(const cdeq_##X* self) { \
        cdeq_##X##_iter_t it = {self->data + cdeq_size(*self)}; return it; \
    } \
    STC_INLINE void \
    cdeq_##X##_next(cdeq_##X##_iter_t* it) {++it->ref;} \
    STC_INLINE cdeq_##X##_value_t* \
    cdeq_##X##_itval(cdeq_##X##_iter_t it) {return it.ref;} \
    STC_INLINE size_t \
    cdeq_##X##_index(cdeq_##X v, cdeq_##X##_iter_t it) {return it.ref - v.data;} \
\
    _c_implement_cdeq_7(X, Value, valueDestroy, RawValue, valueCompareRaw, valueToRaw, valueFromRaw) \
    typedef cdeq_##X cdeq_##X##_t

/* -------------------------- IMPLEMENTATION ------------------------- */

#if !defined(STC_HEADER) || defined(STC_IMPLEMENTATION)
#define _c_implement_cdeq_7(X, Value, valueDestroy, RawValue, valueCompareRaw, valueToRaw, valueFromRaw) \
\
    STC_DEF void \
    cdeq_##X##_push_n(cdeq_##X *self, const cdeq_##X##_input_t arr[], size_t n) { \
        _cdeq_##X##_expand(self, n, false); \
        _cdeq_size(self) += n; \
        cdeq_##X##_value_t* p = self->data + cdeq_size(*self); \
        for (size_t i=0; i < n; ++i) *p++ = valueFromRaw(arr[i]); \
    } \
\
    STC_DEF void \
    cdeq_##X##_clear(cdeq_##X* self) { \
        cdeq_##X##_value_t* p = self->data; if (p) { \
            for (cdeq_##X##_value_t* q = p + _cdeq_size(self); p != q; ++p) \
                valueDestroy(p); \
            _cdeq_size(self) = 0; \
        } \
    } \
    STC_DEF void \
    cdeq_##X##_del(cdeq_##X* self) { \
        cdeq_##X##_clear(self); \
        if (self->base) c_free(_cdeq_alloced(self->base)); \
    } \
\
    STC_DEF void \
    _cdeq_##X##_expand(cdeq_##X* self, size_t n, bool front) { \
        size_t len = cdeq_size(*self), cap = _cdeq_capacity(self); \
        size_t nfront = self->data - self->base, nback = cap - (nfront + len); \
        if (front && nfront >= n || !front && nback >= n) \
            return; \
        if (len + n > cap) { \
            cap = (len + n + 6)*3/2; \
            size_t* rep = (size_t *) c_realloc(_cdeq_alloced(self->base), 2*sizeof(size_t) + cap*sizeof(Value)); \
            self->base = (cdeq_##X##_value_t *) (rep + 2); \
            self->data = self->base + nfront; \
            rep[0] = len; \
            rep[1] = cap; \
        } \
        size_t unused = cap - (len + n); \
        size_t pos = front ? c_maxf(unused*0.7, (float) unused - nback) + n \
                           : c_minf(unused*0.3, nfront); \
        memmove(self->base + pos, self->data, len*sizeof(Value)); \
        self->data = self->base + pos; \
    } \
\
    STC_DEF void \
    cdeq_##X##_resize(cdeq_##X* self, size_t size, Value null_val) { \
        _cdeq_##X##_expand(self, size, false); \
        for (size_t i=cdeq_size(*self); i<size; ++i) self->data[i] = null_val; \
        if (self->data) _cdeq_size(self) = size; \
    } \
\
    STC_DEF void \
    cdeq_##X##_push_front(cdeq_##X* self, Value value) { \
        if (self->data == self->base) \
            _cdeq_##X##_expand(self, 1, true); \
        _cdeq_size(self)++, --self->data; \
        *self->data = value; \
    } \
    STC_DEF void \
    cdeq_##X##_push_back(cdeq_##X* self, Value value) { \
        if ((self->data - self->base) + cdeq_size(*self) == _cdeq_capacity(self)) \
            _cdeq_##X##_expand(self, 1, false); \
        self->data[_cdeq_size(self)++] = value; \
    } \
\
    STC_DEF cdeq_##X \
    cdeq_##X##_clone(cdeq_##X vec) { \
        size_t len = cdeq_size(vec); \
        cdeq_##X out = cdeq_##X##_with_capacity(len); \
        cdeq_##X##_insert_range_p(&out, out.data, vec.data, vec.data + len); \
        return out; \
    } \
\
    STC_DEF cdeq_##X##_iter_t \
    cdeq_##X##_insert_range_p(cdeq_##X* self, cdeq_##X##_value_t* pos, const cdeq_##X##_value_t* first, const cdeq_##X##_value_t* finish) { \
        size_t n = finish - first, idx = pos - self->data, size = cdeq_size(*self); \
        bool is_front = (pos == self->data); \
        _cdeq_##X##_expand(self, n, is_front); \
        if (is_front) \
            pos = (self->data -= n); \
        else { \
            pos = self->data + idx; \
            memmove(pos + n, pos, (size - idx)*sizeof(Value)); \
        } \
        cdeq_##X##_iter_t it = {pos}; \
        _cdeq_size(self) += n; \
        while (first != finish) \
            *pos++ = valueFromRaw(valueToRaw(first++)); \
        return it; \
    } \
\
    STC_DEF cdeq_##X##_iter_t \
    cdeq_##X##_erase_range_p(cdeq_##X* self, cdeq_##X##_value_t* first, cdeq_##X##_value_t* finish) { \
        intptr_t len = finish - first; \
        if (len > 0) { \
            cdeq_##X##_value_t* p = first, *end = self->data + _cdeq_size(self); \
            while (p != finish) valueDestroy(p++); \
            memmove(first, finish, (end - finish) * sizeof(Value)); \
            _cdeq_size(self) -= len; \
        } \
        cdeq_##X##_iter_t it = {first}; return it; \
    } \
\
    STC_DEF cdeq_##X##_iter_t \
    cdeq_##X##_find_in_range(const cdeq_##X* self, cdeq_##X##_iter_t first, cdeq_##X##_iter_t finish, RawValue rawValue) { \
        for (; first.ref != finish.ref; cdeq_##X##_next(&first)) { \
            RawValue r = valueToRaw(first.ref); \
            if (valueCompareRaw(&r, &rawValue) == 0) return first; \
        } \
        return cdeq_##X##_end(self); \
    } \
    STC_DEF cdeq_##X##_iter_t \
    cdeq_##X##_find(const cdeq_##X* self, RawValue rawValue) { \
        return cdeq_##X##_find_in_range(self, cdeq_##X##_begin(self), cdeq_##X##_end(self), rawValue); \
    } \
\
    STC_DEF int \
    cdeq_##X##_value_compare(const cdeq_##X##_value_t* x, const cdeq_##X##_value_t* y) { \
        RawValue rx = valueToRaw(x); \
        RawValue ry = valueToRaw(y); \
        return valueCompareRaw(&rx, &ry); \
    }

#else
#define _c_implement_cdeq_7(X, Value, valueDestroy, valueCompareRaw, RawValue, valueToRaw, valueFromRaw)
#endif

#if defined(_WIN32) && defined(_DLL)
#define STC_EXTERN_IMPORT extern __declspec(dllimport)
#else
#define STC_EXTERN_IMPORT extern
#endif

typedef int(*_cdeq_cmp)(const void*, const void*);
STC_EXTERN_IMPORT void qsort(void *start, size_t nitems, size_t size, _cdeq_cmp cmp);

#define _cdeq_size(self)      ((size_t *) (self)->base)[-2]
#define _cdeq_capacity(self)  _cdeq_safe_capacity((self)->base)

STC_INLINE size_t* _cdeq_alloced(void* base) {
    return base ? ((size_t *) base) - 2 : NULL;
}
STC_INLINE size_t _cdeq_safe_size(const void* base) {
    return base ? ((const size_t *) base)[-2] : 0;
}
STC_INLINE size_t _cdeq_safe_capacity(const void* base) {
    return base ? ((const size_t *) base)[-1] : 0;
}

static inline c_minf(double x, double y) { return x < y ? x : y; }
static inline c_maxf(double x, double y) { return x > y ? x : y; }

#endif
