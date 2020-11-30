# Introduction

UNDER CONSTRUCTION!

This describes the API of circular singly linked list type **clist**.

## Declaration

```c
#define using_clist_str()

#define using_clist(T, Value, valueDestroy=c_default_del,
                              valueCompareRaw=c_default_compare,
                              RawValue=Value,
                              valueToRaw=c_default_to_raw,
                              valueFromRaw=c_default_from_raw)
```
The macro `using_clist()` can be instantiated with 2, 3, 4, or 7 arguments in the global scope.
Default values are given above for args not specified. `T` is a type tag name and
will affect the names of all clist types and methods. E.g. declaring `using_clist(my, int);`, `T` should
be replaced by `my` in all of the following documentation.

`using_clist_str()` is a predefined macro for `using_clist(str, cstr_t, ...)`.

## Types

| Type name             | Type definition                        | Used to represent...                |
|:----------------------|:---------------------------------------|:------------------------------------|
| `clist_T`             | `struct { clist_T_node_t* last; }`     | The clist type                      |
| `clist_T_node_t`      | `struct {`                             | clist node                          |
|                       | `  struct clist_T_node* next;`         |                                     |
|                       | `  clist_T_value_t value;`             |                                     |
|                       | `}`                                    |                                     |
| `clist_T_value_t`     | `Value`                                | The clist element type              |
| `clist_T_input_t`     | `clist_T_value_t`                      | clist input type                    |
| `clist_T_rawvalue_t`  | `RawValue`                             | clist raw value type                |
| `clist_T_iter_t`      | `struct {`                             | clist iterator                      |
|                       | `  ...;`                               |                                     |
|                       | `  clist_T_value_t* val;`              |                                     |
|                       | `}`                                    |                                     |


## Constants and macros

| Name                       | Value            |
|:---------------------------|:-----------------|
|  `clist_inits`             | `{NULL}`         |
|  `clist_empty(list)`       | `true` if empty  |

## Header file

All clist definitions and prototypes may be included in your C source file by including a single header file.

```c
#include "stc/clist.h"
```
## Methods

### Construction

The interfaces to create a clist_T object:
```c
clist_T             clist_T_init(void);

void                clist_T_clear(clist_T* self);
void                clist_T_del(clist_T* self);

bool                clist_T_empty(clist_T list);
size_t              clist_T_size(clist_T list);
Value               clist_T_value_from_raw(RawValue val);

clist_T_value_t*    clist_T_front(clist_T* self);
clist_T_value_t*    clist_T_back(clist_T* self);

void                clist_T_push_n(clist_T *self, const clist_T_input_t in[], size_t size);
void                clist_T_push_back(clist_T* self, Value value);
void                clist_T_emplace_back(clist_T* self, RawValue val);

void                clist_T_push_front(clist_T* self, Value value);
void                clist_T_emplace_front(clist_T* self, RawValue val);
void                clist_T_pop_front(clist_T* self);

clist_T_iter_t      clist_T_insert_after(clist_T* self, clist_T_iter_t pos, Value val);
clist_T_iter_t      clist_T_emplace_after(clist_T* self, clist_T_iter_t pos, RawValue val);

clist_T_iter_t      clist_T_erase_after(clist_T* self, clist_T_iter_t pos);
clist_T_iter_t      clist_T_erase_range_after(clist_T* self, clist_T_iter_t pos, clist_T_iter_t finish);

void                clist_T_splice_front(clist_T* self, clist_T* other);
void                clist_T_splice_back(clist_T* self, clist_T* other);
void                clist_T_splice_after(clist_T* self, clist_T_iter_t pos, clist_T* other);

clist_T_iter_t      clist_T_find(const clist_T* self, RawValue val);
clist_T_iter_t      clist_T_find_before(const clist_T* self,
                                        clist_T_iter_t first, clist_T_iter_t finish, RawValue val);

size_t              clist_T_remove(clist_T* self, RawValue val);

void                clist_T_sort(clist_T* self);

clist_T_iter_t      clist_T_before_begin(const clist_T* self);
clist_T_iter_t      clist_T_begin(const clist_T* self);
clist_T_iter_t      clist_T_last(const clist_T* self);
clist_T_iter_t      clist_T_end(const clist_T* self);
void                clist_T_next(clist_T_iter_t* it);
clist_T_value_t*    clist_T_itval(clist_T_iter_t it);
```
