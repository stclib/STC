// MIT License
//
// Copyright (c) 2020 Tyge Løvset, NORCE, www.norceresearch.no
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#ifndef C_HASHMAP_H_
#define C_HASHMAP_H_

#include "c_vector.h"

#define c_hashmap_initializer  {c_vector_initializer, 0, 0.8f}
#define c_hashmap_size(map)     ((size_t) (map)._size)
#define c_hashmap_buckets(map)  c_vector_capacity((map)._vec)


// c_HashmapEntry:
enum { c_HashmapEntry_VACANT = 0, 
       c_HashmapEntry_INUSE = 1,
       c_HashmapEntry_ERASED = 2
};
#define c_declare_HashmapEntry(tag, Key, Value, valueDestroy, keyDestroy) \
struct c_HashmapEntry_##tag { \
    Key key; \
    Value value; \
    uint8_t state, changed; \
}; \
 \
static inline void c_hashmapentry_##tag##_destroy(struct c_HashmapEntry_##tag* e) { \
    keyDestroy(&e->key); \
    valueDestroy(&e->value); \
    e->state = c_HashmapEntry_VACANT; \
} \
typedef struct c_HashmapEntry_##tag c_HashmapEntry_##tag


// c_Hashmap:
#define c_declare_Hashmap(...)  c_defs_MACRO_OVERLOAD(c_declare_Hashmap, __VA_ARGS__)

#define c_declare_Hashmap_3(tag, Key, Value) \
    c_declare_Hashmap_4(tag, Key, Value, c_defs_destroy)

#define c_declare_Hashmap_4(tag, Key, Value, valueDestroy) \
    c_declare_Hashmap_6(tag, Key, Value, valueDestroy, memcmp, c_defs_murmurHash)
    
#define c_declare_Hashmap_6(tag, Key, Value, valueDestroy, keyCompare, keyHash) \
    c_declare_Hashmap_10(tag, Key, Value, valueDestroy, keyCompare, keyHash, c_defs_destroy, Key, c_defs_getRaw, c_defs_initRaw)


// c_Hashmap<c_String, Value>:
#define c_declare_Hashmap_stringkey(...)  c_defs_MACRO_OVERLOAD(c_declare_Hashmap_stringkey, __VA_ARGS__)

#define c_declare_Hashmap_stringkey_2(tag, Value) \
    c_declare_Hashmap_stringkey_3(tag, Value, c_defs_destroy)

#define c_declare_Hashmap_stringkey_3(tag, Value, valueDestroy) \
    c_declare_Hashmap_10(tag, c_String, Value, valueDestroy, c_string_compareRaw, c_string_hashRaw, c_string_destroy, const char*, c_string_getRaw, c_string_make)


// c_Hashmap full:
#define c_declare_Hashmap_10(tag, Key, Value, valueDestroy, keyCompareRaw, keyHashRaw, keyDestroy, KeyRaw, keyGetRaw, keyInitRaw) \
  c_declare_HashmapEntry(tag, Key, Value, valueDestroy, keyDestroy); \
  c_declare_Vector_3(map_##tag, c_HashmapEntry_##tag, c_hashmapentry_##tag##_destroy); \
 \
typedef struct c_Hashmap_##tag { \
    c_Vector_map_##tag _vec; \
    size_t _size; \
    float maxLoadFactor; \
} c_Hashmap_##tag; \
 \
typedef struct c_hashmap_##tag##_iter_t { \
    c_HashmapEntry_##tag *item, *_end; \
} c_hashmap_##tag##_iter_t; \
 \
static inline c_Hashmap_##tag c_hashmap_##tag##_init(void) { \
    c_Hashmap_##tag map = c_hashmap_initializer; \
    return map; \
} \
 \
static inline void c_hashmap_##tag##_destroy(c_Hashmap_##tag* self) { \
    if (c_hashmap_size(*self)) { \
        size_t cap = _c_vector_capacity(self->_vec); \
        c_HashmapEntry_##tag* e = self->_vec.data, *end = e + cap; \
        for (; e != end; ++e) if (e->state == c_HashmapEntry_INUSE) c_hashmapentry_##tag##_destroy(e); \
    } \
    c_vector_map_##tag##_destroy(&self->_vec); \
} \
 \
static inline size_t c_hashmap_##tag##_reserve(c_Hashmap_##tag* self, size_t size); /* predeclared */ \
 \
static inline void c_hashmap_##tag##_clear(c_Hashmap_##tag* self) { \
    c_Hashmap_##tag map = c_hashmap_initializer; \
    c_hashmap_##tag##_destroy(self); \
    *self = map; \
} \
 \
static inline void c_hashmap_##tag##_swap(c_Hashmap_##tag* a, c_Hashmap_##tag* b) { \
    c_vector_map_##tag##_swap(&a->_vec, &b->_vec); \
    c_defs_swap(size_t, a->_size, b->_size); \
} \
 \
static inline void c_hashmap_##tag##_setMaxLoadFactor(c_Hashmap_##tag* self, float fac) { \
    self->maxLoadFactor = fac; \
    if (c_hashmap_size(*self) > c_hashmap_buckets(*self) * fac) \
        c_hashmap_##tag##_reserve(self, 1 + (size_t) (c_hashmap_size(*self) / fac)); \
} \
 \
static inline size_t c_hashmap_##tag##_bucket(c_Hashmap_##tag map, KeyRaw rawKey) { \
    size_t cap = c_vector_capacity(map._vec); \
    size_t idx = c_hashmap_reduce(keyHashRaw(&rawKey, sizeof(Key)), cap); \
    size_t first = idx, erased_idx = cap; \
    FIBONACCI_DECL; \
    do { \
        switch (map._vec.data[idx].state) { \
            case c_HashmapEntry_VACANT: \
                return erased_idx != cap ? erased_idx : idx; \
            case c_HashmapEntry_INUSE: \
                if (keyCompareRaw(&map._vec.data[idx].key, &rawKey, sizeof(Key)) != 0) \
                    break; \
                if (erased_idx != cap) { \
                    c_defs_swap(c_HashmapEntry_##tag, map._vec.data[erased_idx], map._vec.data[idx]); \
                    return erased_idx; \
                } \
                return idx; \
            case c_HashmapEntry_ERASED: \
                if (erased_idx == cap) erased_idx = idx; \
                break; \
        } \
        idx = first + FIBONACCI_NEXT; \
        if (idx >= cap) idx %= cap; \
    } while (1); \
} \
 \
static inline c_HashmapEntry_##tag* c_hashmap_##tag##_get(c_Hashmap_##tag map, KeyRaw rawKey) { \
    if (c_hashmap_size(map) == 0) return NULL; \
    size_t idx = c_hashmap_##tag##_bucket(map, rawKey); \
    return map._vec.data[idx].state == c_HashmapEntry_INUSE ? &map._vec.data[idx] : NULL; \
} \
 \
static inline c_HashmapEntry_##tag* c_hashmap_##tag##_put(c_Hashmap_##tag* self, KeyRaw rawKey, Value value) { \
    size_t cap = c_vector_capacity(self->_vec); \
    if (c_hashmap_size(*self) >= cap * self->maxLoadFactor) \
        cap = c_hashmap_##tag##_reserve(self, (size_t) 7 + (cap * 1.8)); \
    size_t idx = c_hashmap_##tag##_bucket(*self, rawKey); \
    c_HashmapEntry_##tag* e = &self->_vec.data[idx]; \
    e->value = value; \
    e->changed = (e->state == c_HashmapEntry_INUSE); \
    if (e->state != c_HashmapEntry_INUSE) { \
        e->key = keyInitRaw(rawKey); \
        e->state = c_HashmapEntry_INUSE; \
        ++self->_size; \
    } \
    return e; \
} \
 \
static inline size_t c_hashmap_##tag##_reserve(c_Hashmap_##tag* self, size_t size) { \
    size_t oldcap = c_vector_capacity(self->_vec), newcap = 1 + (size / 2) * 2; \
    if (oldcap >= newcap) return oldcap; \
    c_Vector_map_##tag vec = c_vector_initializer; \
    c_vector_map_##tag##_swap(&self->_vec, &vec); \
    c_vector_map_##tag##_reserve(&self->_vec, newcap); \
    self->_size = 0; \
    memset(self->_vec.data, 0, sizeof(c_HashmapEntry_##tag) * newcap); \
    c_HashmapEntry_##tag* e = vec.data; \
    for (size_t i = 0; i < oldcap; ++i, ++e) \
        if (e->state == c_HashmapEntry_INUSE) c_hashmap_##tag##_put(self, keyGetRaw(e->key), e->value); \
    return newcap; \
} \
 \
static inline bool c_hashmap_##tag##_erase(c_Hashmap_##tag* self, KeyRaw rawKey) { \
    size_t idx = c_hashmap_##tag##_bucket(*self, rawKey); \
    c_HashmapEntry_##tag* e = &self->_vec.data[idx]; \
    if (e->state == c_HashmapEntry_INUSE) { \
        c_hashmapentry_##tag##_destroy(e); \
        e->state = c_HashmapEntry_ERASED; \
        --self->_size; \
        return true; \
    } \
    return false; \
} \
 \
static inline c_hashmap_##tag##_iter_t c_hashmap_##tag##_begin(c_Hashmap_##tag map) { \
    c_hashmap_##tag##_iter_t null = {NULL, NULL}; \
    if (c_hashmap_size(map) == 0) return null; \
    c_HashmapEntry_##tag* e = map._vec.data, *end = e + _c_vector_capacity(map._vec); \
    while (e != end && e->state != c_HashmapEntry_INUSE) ++e; \
    c_hashmap_##tag##_iter_t it = {e, end}; return it; \
} \
 \
static inline c_hashmap_##tag##_iter_t c_hashmap_##tag##_next(c_hashmap_##tag##_iter_t it) { \
    do { ++it.item; } while (it.item != it._end && it.item->state != c_HashmapEntry_INUSE); \
    return it; \
} \
 \
static inline c_hashmap_##tag##_iter_t c_hashmap_##tag##_end(c_Hashmap_##tag map) { \
    c_HashmapEntry_##tag* end = (c_hashmap_size(map) == 0) ? NULL : map._vec.data + _c_vector_capacity(map._vec); \
    c_hashmap_##tag##_iter_t it = {end, end}; \
    return it; \
} \
typedef Key c_hashmap_##tag##_key_t; \
typedef Value c_hashmap_##tag##_value_t


#define FIBONACCI_DECL  size_t fib1 = 0, fib2 = 1, fibx
#define FIBONACCI_NEXT  (fibx = fib1 + fib2, fib1 = fib2, fib2 = fibx)

// https://lemire.me/blog/2016/06/27/a-fast-alternative-to-the-modulo-reduction/
static inline uint32_t c_hashmap_reduce(uint32_t x, uint32_t N) {
    return ((uint64_t) x * (uint64_t) N) >> 32 ;
}


#endif
