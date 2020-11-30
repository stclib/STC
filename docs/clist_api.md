# Introduction

UNDER CONSTRUCTION!

This describes the API of circular singly linked list type **clist**.

## Declaration

```c
#define using_clist_str()

#define using_clist(z, Value, valueDestroy=c_default_del,
                              valueCompareRaw=c_default_compare,
                              RawValue=Value,
                              valueToRaw=c_default_to_raw,
                              valueFromRaw=c_default_from_raw)
```
The macro `using_clist()` can be instantiated with 2, 3, 4, or 7 arguments in the global scope.
Defaults are given above for args not specified. Note that `z` can be any name, it's a tag and
will affect the names of all clist types and methods, e.g. for `using_clist(my, int);` `_z` should
be replaced by `_my` in all of the following documentation. `using_clist_str()` is a predefined
macro for `using_clist(str, cstr_t, ...)`.

## Types

| Type name             | Type definition                        | Used to represent...                |
|:----------------------|:---------------------------------------|:------------------------------------|
| `clist_z`             | `struct { clist_z_node_t* last; }`     | The clist type                      |
| `clist_z_node_t`      | `struct {`                             | clist node                          |
|                       | `  struct clist_z_node* next;`         |                                     |
|                       | `  clist_z_value_t value;`             |                                     |
|                       | `}`                                    |                                     |
| `clist_z_value_t`     | `Value`                                | The clist element type              |
| `clist_z_input_t`     | `clist_z_value_t`                      | clist input type                    |
| `clist_z_rawvalue_t`  | `RawValue`                             | clist raw value type                |
| `clist_z_iter_t`      | `struct {`                             | clist iterator                      |
|                       | `  ...;`                               |                                     |
|                       | `  clist_z_value_t* val;`              |                                     |
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

The interfaces to create a clist_z object:
```c
clist_z             clist_z_init(void);

void                clist_z_clear(clist_z* self);
void                clist_z_del(clist_z* self);

bool                clist_z_empty(clist_z list);
size_t              clist_z_size(clist_z list);
Value               clist_z_value_from_raw(RawValue val);

clist_z_value_t*    clist_z_front(clist_z* self);
clist_z_value_t*    clist_z_back(clist_z* self);

void                clist_z_push_n(clist_z *self, const clist_z_input_t in[], size_t size);
void                clist_z_push_back(clist_z* self, Value value);
void                clist_z_emplace_back(clist_z* self, RawValue val);

void                clist_z_push_front(clist_z* self, Value value);
void                clist_z_emplace_front(clist_z* self, RawValue val);
void                clist_z_pop_front(clist_z* self);

clist_z_iter_t      clist_z_insert_after(clist_z* self, clist_z_iter_t pos, Value val);
clist_z_iter_t      clist_z_emplace_after(clist_z* self, clist_z_iter_t pos, RawValue val);

clist_z_iter_t      clist_z_erase_after(clist_z* self, clist_z_iter_t pos);
clist_z_iter_t      clist_z_erase_range_after(clist_z* self, clist_z_iter_t pos, clist_z_iter_t finish);

void                clist_z_splice_front(clist_z* self, clist_z* other);
void                clist_z_splice_back(clist_z* self, clist_z* other);
void                clist_z_splice_after(clist_z* self, clist_z_iter_t pos, clist_z* other);

clist_z_iter_t      clist_z_find(const clist_z* self, RawValue val);
clist_z_iter_t      clist_z_find_before(const clist_z* self,
                                        clist_z_iter_t first, clist_z_iter_t finish, RawValue val);

size_t              clist_z_remove(clist_z* self, RawValue val);

void                clist_z_sort(clist_z* self);

clist_z_iter_t      clist_z_before_begin(const clist_z* self);
clist_z_iter_t      clist_z_begin(const clist_z* self);
clist_z_iter_t      clist_z_last(const clist_z* self);
clist_z_iter_t      clist_z_end(const clist_z* self);
void                clist_z_next(clist_z_iter_t* it);
clist_z_value_t*    clist_z_itval(clist_z_iter_t it);
```
