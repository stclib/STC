# Container type cmap

This describes the API of the unordered map type **cmap**.

## Declaration

```c
#define using_cmap_str()

#define using_cmap_strkey(X, Mapped, mappedDestroy=c_default_del)

#define using_cmap_strval(X, Key, keyEquals=c_default_equals,
                                  keyHash=c_default_hash16,
                                  keyDestroy=c_default_del,
                                  RawKey=Key,
                                  keyToRaw=c_default_to_raw,
                                  keyFromRaw=c_default_from_raw)

#define using_cmap(X, Key, Mapped, mappedDestroy=c_default_del,
                                   keyEqualsRaw=c_default_equals,
                                   keyHashRaw=c_default_hash16,
                                   keyDestroy=c_default_del,
                                   RawKey=Key,
                                   keyToRaw=c_default_to_raw,
                                   keyFromRaw=c_default_from_raw,
                                   RawMapped=Mapped,
                                   mappedFromRaw=c_default_from_raw)
```
The macro `using_cmap()` can be instantiated with 3, 4, 6, 10, or 12 arguments in the global scope.
Default values are given above for args not specified. `X` is a type tag name and
will affect the names of all cmap types and methods. E.g. declaring `using_cmap(my, int);`, `X` should
be replaced by `my` in all of the following documentation.

`using_cmap_str()` is a specific definition of `using_cmap(str, cstr_t, ...)`. `using_cmap_strkey(X, ...)` and `using_cmap_strval(X, ...)`
are special macros for `using_cmap()` with `cstr_t` as key and mapped value correspondingly.

## Types

| Type name            | Type definition                       | Used to represent...               |
|:---------------------|:--------------------------------------|:-----------------------------------|
| `cmap_X`             | `struct {`                            | The cmap type                      |
|                      | `  cmap_X_value_t* table;`            |                                    |
|                      | `  uint8_t* _hashx;`                  |                                    |
|                      | `  ...;`                              |                                    |
|                      | `}`                                   |                                    |
| `cmap_X_rawkey_t`    | `RawKey`                              | The raw key type                   |
| `cmap_X_rawval_t`    | `RawMapped`                           | The raw mapped type                |
| `cmap_X_key_t`       | `Key`                                 | The key type                       |
| `cmap_X_mapped_t`    | `Mapped`                              | The mapped type                    |
| `cmap_X_value_t`     | `struct {`                            | The value type                     |
|                      | `  cmap_X_key_t first;`               |                                    |
|                      | `  cmap_X_mapped_t second;`           |                                    |
|                      | `}`                                   |                                    |
| `cmap_X_input_t`     | `struct { ... }`                      | RawKey + RawVal type               |
| `cmap_X_result_t`    | `struct {`                            | Result of insert/put/emplace       |
|                      | `  cmap_X_value_t* first;`            |                                    |
|                      | `  bool second; /* inserted */`       |                                    |
|                      | `}`                                   |                                    |
| `cmap_X_iter_t`      | `struct {`                            | Iterator type                      |
|                      | `  cmap_X_value_t* val;`              |                                    |
|                      | `  ...;`                              |                                    |
|                      | `}`                                   |                                    |

## Constants and macros

| Name                                            | Purpose                |
|:------------------------------------------------|:-----------------------|
|  `cmap_inits`                                   | Initializer const      |
|  `cmap_empty(map)`                              | Test for empty map     |
|  `cmap_size(map)`                               | Get map size           |
|  `cmap_capacity(map)`                           | Get map capacity       |
|  `c_try_emplace(self, ctype, key, val)`         | Emplace if key exist   |

## Header file

All cmap definitions and prototypes may be included in your C source file by including a single header file.

```c
#include "stc/cmap.h"
```
## Methods

```c
cmap_X              cmap_X_init(void);
cmap_X              cmap_X_with_capacity(size_t cap);
void                cmap_X_set_load_factors(cmap_X* self, float max, float shrink);

void                cmap_X_clear(cmap_X* self);
void                cmap_X_reserve(cmap_X* self, size_t size);
void                cmap_X_swap(cmap_X* a, cmap_X* b);

void                cmap_X_del(cmap_X* self);

bool                cmap_X_empty(cmap_X m);
size_t              cmap_X_size(cmap_X m);
size_t              cmap_X_bucket_count(cmap_X m);
size_t              cmap_X_capacity(cmap_X m);

void                cmap_X_push_n(cmap_X* self, const cmap_X_input_t in[], size_t size);

cmap_X_result_t     cmap_X_emplace(cmap_X* self, RawKey rkey, RawMapped rmapped);
cmap_X_result_t     cmap_X_insert(cmap_X* self, cmap_X_input_t in);
cmap_X_result_t     cmap_X_insert_or_assign(cmap_X* self, RawKey rkey, RawMapped rmapped);
cmap_X_result_t     cmap_X_put(cmap_X* self, RawKey rkey, RawMapped rmapped);
cmap_X_result_t     cmap_X_putv(cmap_X* self, RawKey rkey, Mapped mapped);
cmap_X_mapped_t*    cmap_X_at(const cmap_X* self, RawKey rkey);

size_t              cmap_X_erase(cmap_X* self, RawKey rkey);
void                cmap_X_erase_entry(cmap_X* self, cmap_X_value_t* val);
cmap_X_iter_t       cmap_X_erase_at(cmap_X* self, cmap_X_iter_t pos);

cmap_X_value_t*     cmap_X_find(const cmap_X* self, RawKey rkey);
bool                cmap_X_contains(const cmap_X* self, RawKey rkey);

cmap_X_iter_t       cmap_X_begin(cmap_X* self);
cmap_X_iter_t       cmap_X_end(cmap_X* self);
void                cmap_X_next(cmap_X_iter_t* it);
cmap_X_mapped_t*    cmap_X_itval(cmap_X_iter_t it);

cmap_bucket_t       cmap_X_bucket(const cmap_X* self, const cmap_X_rawkey_t* rawKeyPtr);

uint32_t            c_default_hash16(const void *data, size_t len);
uint32_t            c_default_hash32(const void* data, size_t len);
```

## Example
```c
#include <stdio.h>
#include "stc/cstr.h"
#include "stc/cmap.h"

using_cmap_str();

int main()
{
    // Create an unordered_map of three strings (that map to strings)
    cmap_str u = cmap_inits;
    c_push_items(&u, cmap_str, {
        {"RED","#FF0000"},
        {"GREEN","#00FF00"},
        {"BLUE","#0000FF"}
    });

    // Iterate and print keys and values of unordered map
    c_foreach (n, cmap_str, u) {
        printf("Key:[%s] Value:[%s]\n", n.val->first.str, n.val->second.str);
    }

    // Add two new entries to the unordered map
    cmap_str_put(&u, "BLACK", "#000000");
    cmap_str_put(&u, "WHITE", "#FFFFFF");

    // Output values by key
    printf("The HEX of color RED is:[%s]\n", cmap_str_at(&u, "RED")->str);
    printf("The HEX of color BLACK is:[%s]\n", cmap_str_at(&u, "BLACK")->str);

    return 0;
}
```
Output:
```
Key:[RED] Value:[#FF0000]
Key:[GREEN] Value:[#00FF00]
Key:[BLUE] Value:[#0000FF]
The HEX of color RED is:[#FF0000]
The HEX of color BLACK is:[#000000]
```
