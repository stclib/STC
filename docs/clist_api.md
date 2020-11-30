# Introduction

UNDER CONSTRUCTION!

This describes the API of circular singly linked list type **clist**.

## Declaration

```c
#define using_clist_str()

#define using_clist($, Value, valueDestroy=c_default_del,
                              valueCompareRaw=c_default_compare,
                              RawValue=Value,
                              valueToRaw=c_default_to_raw,
                              valueFromRaw=c_default_from_raw)
```
The macro `using_clist()` can be instantiated with 2, 3, 4, or 7 arguments in the global scope.
Default values are given above for args not specified. `$` is a type tag name and
will affect the names of all clist types and methods. E.g. declaring `using_clist(my, int);`, `$` should
be replaced by `my` in all of the following documentation.

`using_clist_str()` is a predefined macro for `using_clist(str, cstr_t, ...)`.

## Types

| Type name             | Type definition                        | Used to represent...                |
|:----------------------|:---------------------------------------|:------------------------------------|
| `clist_$`             | `struct { clist_$_node_t* last; }`     | The clist type                      |
| `clist_$_node_t`      | `struct {`                             | clist node                          |
|                       | `  struct clist_$_node* next;`         |                                     |
|                       | `  clist_$_value_t value;`             |                                     |
|                       | `}`                                    |                                     |
| `clist_$_value_t`     | `Value`                                | The clist element type              |
| `clist_$_input_t`     | `clist_$_value_t`                      | clist input type                    |
| `clist_$_rawvalue_t`  | `RawValue`                             | clist raw value type                |
| `clist_$_iter_t`      | `struct {`                             | clist iterator                      |
|                       | `  ...;`                               |                                     |
|                       | `  clist_$_value_t* val;`              |                                     |
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

The interfaces to create a clist_$ object:
```c
clist_$             clist_$_init(void);

void                clist_$_clear(clist_$* self);
void                clist_$_del(clist_$* self);

bool                clist_$_empty(clist_$ list);
size_t              clist_$_size(clist_$ list);
Value               clist_$_value_from_raw(RawValue val);

clist_$_value_t*    clist_$_front(clist_$* self);
clist_$_value_t*    clist_$_back(clist_$* self);

void                clist_$_push_n(clist_$ *self, const clist_$_input_t in[], size_t size);
void                clist_$_push_back(clist_$* self, Value value);
void                clist_$_emplace_back(clist_$* self, RawValue val);

void                clist_$_push_front(clist_$* self, Value value);
void                clist_$_emplace_front(clist_$* self, RawValue val);
void                clist_$_pop_front(clist_$* self);

clist_$_iter_t      clist_$_insert_after(clist_$* self, clist_$_iter_t pos, Value val);
clist_$_iter_t      clist_$_emplace_after(clist_$* self, clist_$_iter_t pos, RawValue val);

clist_$_iter_t      clist_$_erase_after(clist_$* self, clist_$_iter_t pos);
clist_$_iter_t      clist_$_erase_range_after(clist_$* self, clist_$_iter_t pos, clist_$_iter_t finish);

void                clist_$_splice_front(clist_$* self, clist_$* other);
void                clist_$_splice_back(clist_$* self, clist_$* other);
void                clist_$_splice_after(clist_$* self, clist_$_iter_t pos, clist_$* other);

clist_$_iter_t      clist_$_find(const clist_$* self, RawValue val);
clist_$_iter_t      clist_$_find_before(const clist_$* self,
                                        clist_$_iter_t first, clist_$_iter_t finish, RawValue val);

size_t              clist_$_remove(clist_$* self, RawValue val);

void                clist_$_sort(clist_$* self);

clist_$_iter_t      clist_$_before_begin(const clist_$* self);
clist_$_iter_t      clist_$_begin(const clist_$* self);
clist_$_iter_t      clist_$_last(const clist_$* self);
clist_$_iter_t      clist_$_end(const clist_$* self);
void                clist_$_next(clist_$_iter_t* it);
clist_$_value_t*    clist_$_itval(clist_$_iter_t it);
```
