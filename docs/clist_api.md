# Introduction

UNDER CONSTRUCTION!

This describes the API of string type **clist_xx**.

## Instantiation

```c
#define using_clist(xx, Value, ...)

#define using_clist_str()

#define using_clist(xx, Value, valueDestroy=c_default_del,
                               valueCompareRaw=c_default_compare,
                               RawValue=Value,
                               valueToRaw=c_default_to_raw,
                               valueFromRaw=c_default_from_raw)
```
The macro `using_clist()` can be instantiated with 2, 3, 4, or 7 arguments. Defaults are given above for args not specified.
Note that xx can be any name, it's a tag and will affect the names of all clist types and methods,
e.g. for `using_clist(my, int);` 'xx' should be replaced by 'my' in all the following.
`using_clist_str()` is a predefined definition of clist_using(str, cstr_t, ...).

## Types

| cstr                  | Type definition                        | Used to represent...                |
|:----------------------|:---------------------------------------|:------------------------------------|
| `clist_xx`            | `struct { clist_xx_node_t* last; }`    | The clist type                      |
| `clist_xx_node_t`     | `struct {`                             |                                     |
|                       | `  struct clist_xx_node* next;`        |                                     |
|                       | `  clist_xx_value_t value;`            |                                     |
|                       | `}`                                    | clist node                          |
| `clist_xx_value_t`    | `Value`                                | The clist element type              |
| `clist_xx_iter_t`     | `struct {`                             |                                     |
|                       | `  clist_xx_node_t* const* _last;`     |                                     |
|                       | `  clist_xx_value_t* val;`             |                                     |
|                       | `  int _state;`                        |                                     |
|                       | `}`                                    | clist iterator                      |

## Constants

| cstr constant name         | Numerical values |
|:---------------------------|:-----------------|
|  `clist_inits`             | `{NULL}`         |

## Header file

All clist definitions and prototypes may be included in your C source file by including a single header file.

```c
#include "stc/clist.h"
```
## Methods

### Construction

The interfaces to create a clist_xx object:
```c
clist_xx            clist_xx_init(void);
bool                clist_xx_empty(clist_xx ls);
size_t              clist_xx_size(clist_xx ls);

void                clist_xx_clear(clist_xx* self);
void                clist_xx_del(clist_xx* self);

Value               clist_xx_value_from_raw(RawValue rawValue);

void                clist_xx_push_n(clist_xx *self, const clist_xx_input_t in[], size_t size);
void                clist_xx_push_back(clist_xx* self, Value value);
void                clist_xx_emplace_back(clist_xx* self, RawValue rawValue);

void                clist_xx_push_front(clist_xx* self, Value value);
void                clist_xx_emplace_front(clist_xx* self, RawValue rawValue);

void                clist_xx_pop_front(clist_xx* self);

clist_xx_iter_t     clist_xx_insert_after(clist_xx* self, clist_xx_iter_t pos, Value value);
clist_xx_iter_t     clist_xx_emplace_after(clist_xx* self, clist_xx_iter_t pos, RawValue rawValue);

clist_xx_iter_t     clist_xx_erase_after(clist_xx* self, clist_xx_iter_t pos);
clist_xx_iter_t     clist_xx_erase_range_after(clist_xx* self, clist_xx_iter_t pos, clist_xx_iter_t finish);

void                clist_xx_splice_front(clist_xx* self, clist_xx* other);
void                clist_xx_splice_back(clist_xx* self, clist_xx* other);
void                clist_xx_splice_after(clist_xx* self, clist_xx_iter_t pos, clist_xx* other);

clist_xx_iter_t     clist_xx_find(const clist_xx* self, RawValue val);
clist_xx_iter_t     clist_xx_find_before(const clist_xx* self, clist_xx_iter_t first, clist_xx_iter_t finish, RawValue val);

size_t              clist_xx_remove(clist_xx* self, RawValue val);

void                clist_xx_sort(clist_xx* self);

Value*              clist_xx_front(clist_xx* self);
Value*              clist_xx_back(clist_xx* self);

clist_xx_iter_t     clist_xx_before_begin(const clist_xx* self);
clist_xx_iter_t     clist_xx_begin(const clist_xx* self);
clist_xx_iter_t     clist_xx_last(const clist_xx* self);
clist_xx_iter_t     clist_xx_end(const clist_xx* self);
void                clist_xx_next(clist_xx_iter_t* it);
clist_xx_value_t*   clist_xx_itval(clist_xx_iter_t it);
```
