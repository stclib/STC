# Introduction

UNDER CONSTRUCTION!

This describes the API of string type **cbitset**.

## Types

| cbitset               | Type definition                           | Used to represent...                 |
|:----------------------|:------------------------------------------|:-------------------------------------|
| `cbitset_t`           | `struct { uint64_t* _arr; size_t size; }` | The cbitset type                     |
| `cbitset_iter_t`      | `struct { cbitset_t *_bs; size_t val; }`  | The cbitset iterator type            |

## Header file

All cstr definitions and prototypes may be included in your C source file by including a single header file.

```c
#include "stc/cbitset.h"
```
## Methods

### Construction

The interfaces to create a cbitset object:
```c
cbitset_t        cbitset_init(void);
cbitset_t        cbitset_with_size(size_t size, bool value);
cbitset_t        cbitset_from_str(const char* str);
cbitset_t        cbitset_clone(cbitset_t other);
cbitset_t        cbitset_move(cbitset_t* self);

cbitset_t        cbitset_intersect(cbitset_t s1, cbitset_t s2);
cbitset_t        cbitset_union(cbitset_t s1, cbitset_t s2);
cbitset_t        cbitset_xor(cbitset_t s1, cbitset_t s2);
cbitset_t        cbitset_not(cbitset_t s1);

cbitset_t*       cbitset_take(cbitset_t* self, cbitset_t other);
cbitset_t*       cbitset_assign(cbitset_t* self, cbitset_t other);

void             cbitset_resize(cbitset_t* self, size_t size, bool value);
void             cbitset_del(cbitset_t* self);

size_t           cbitset_size(cbitset_t set);
size_t           cbitset_count(cbitset_t set);
bool             cbitset_is_disjoint(cbitset_t set, cbitset_t other);
bool             cbitset_is_subset(cbitset_t set, cbitset_t other);
bool             cbitset_is_superset(cbitset_t set, cbitset_t other);
char*            cbitset_to_str(cbitset_t set, char* str, size_t start, intptr_t stop);

void             cbitset_set(cbitset_t *self, size_t i);
void             cbitset_reset(cbitset_t *self, size_t i);
void             cbitset_set_value(cbitset_t *self, size_t i, bool value);
void             cbitset_flip(cbitset_t *self, size_t i);
bool             cbitset_test(cbitset_t set, size_t i);
void             cbitset_set_all(cbitset_t *self, bool value);
void             cbitset_set_all_64(cbitset_t *self, uint64_t pattern);
void             cbitset_flip_all(cbitset_t *self);

void             cbitset_intersect_with(cbitset_t *self, cbitset_t other);
void             cbitset_union_with(cbitset_t *self, cbitset_t other);
void             cbitset_xor_with(cbitset_t *self, cbitset_t other);

cbitset_iter_t   cbitset_begin(cbitset_t* self);
cbitset_iter_t   cbitset_end(cbitset_t* self);
void             cbitset_next(cbitset_iter_t* it);
bool             cbitset_itval(cbitset_iter_t it);
```
