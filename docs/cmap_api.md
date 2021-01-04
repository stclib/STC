# STC Container [cmap](../stc/cmap.h): Unordered Map
![Map](pics/map.jpg)

A **cmap** is an associative container that contains key-value pairs with unique keys. Search, insertion, and removal of elements have average constant-time complexity.
Elements are pairs of keys and mapped values. Implemented as open hashing with linear probing and without storing tombstones. Very fast, see performance comparisons.
See [std::unordered_map](https://en.cppreference.com/w/cpp/container/unordered_map) for a similar c++ class.

## Declaration

```c
#define using_cmap(X, Key, Mapped, mappedDestroy=c_default_del,
                                   mappedClone=c_default_clone,
                                   keyEqualsRaw=c_default_equals,
                                   keyHashRaw=c_default_hash,
                                   keyDestroy=c_default_del,
                                   keyFromRaw=c_default_clone,
                                   keyToRaw=c_default_to_raw,
                                   RawKey=Key)
// or:
#define using_cmap(X, Key, Mapped, mappedDestroy,
                                   mappedFromRaw, mappedToRaw, RawMapped,
                                   keyEqualsRaw, keyHashRaw, keyDestroy,
                                   keyFromRaw, keyToRaw, RawKey)
                                   
#define using_cmap_strkey(X, Mapped, mappedDestroy=c_default_del,
                                     mappedClone=c_default_clone)

#define using_cmap_strval(X, Key, keyEquals=c_default_equals,
                                  keyHash=c_default_hash,
                                  keyDestroy=c_default_del,
                                  keyFromRaw=c_default_clone,
                                  keyToRaw=c_default_to_raw,
                                  RawKey=Key)
#define using_cmap_str()
```
The macro `using_cmap()` can be instantiated with 3, 5, 7, 9, 11, or 13 arguments in the global scope.
Default values are given above for args not specified. `X` is a type tag name and
will affect the names of all cmap types and methods. E.g. declaring `using_cmap(my, int);`, `X` should
be replaced by `my` in all of the following documentation.

`c_default_hash` requires Key to be 16-bit aligned, and size to be a multiple of 16. There is also a `c_default_hash32` which is slightly faster.

`using_cmap_strkey()` and `using_cmap_strval()` are special macros defined by
`using_cmap()`. The macro `using_cmap_str()` is a shorthand for
```c
using_cmap(str, cstr_t, cstr_t, cstr_del,
                cstr_from, cstr_to_raw, const char*,
                cstr_equals_raw, cstr_hash_raw, cstr_del,
                cstr_from, cstr_to_raw, const char*)
```

## Types

| Type name            | Type definition                                 | Used to represent...          |
|:---------------------|:------------------------------------------------|:------------------------------|
| `cmap_X`             | `struct { ... }`                                | The cmap type                 |
| `cmap_X_rawkey_t`    | `RawKey`                                        | The raw key type              |
| `cmap_X_rawval_t`    | `RawMapped`                                     | The raw mapped type           |
| `cmap_X_key_t`       | `Key`                                           | The key type                  |
| `cmap_X_mapped_t`    | `Mapped`                                        | The mapped type               |
| `cmap_X_value_t`     | `struct { Key first; Mapped second; }`          | The value type                |
| `cmap_X_input_t`     | `struct { RawKey first; RawMapped second; }`    | RawKey + RawVal type          |
| `cmap_X_result_t`    | `struct { cmap_X_value_t first; bool second; }` | Result of insert/put/emplace  |
| `cmap_X_iter_t`      | `struct { cmap_X_value_t *ref; ... }`           | Iterator type                 |

## Constants and macros

| Name                                     | Purpose                |
|:-----------------------------------------|:-----------------------|
|  `cmap_inits`                            | Initializer const      |
|  `c_try_emplace(self, ctype, key, ref)`  | Emplace if key exist   |

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

cmap_X              cmap_X_clone(cmap_x map);
void                cmap_X_clear(cmap_X* self);
void                cmap_X_reserve(cmap_X* self, size_t size);
void                cmap_X_swap(cmap_X* a, cmap_X* b);

void                cmap_X_del(cmap_X* self);

bool                cmap_X_empty(cmap_X m);
size_t              cmap_X_size(cmap_X m);
size_t              cmap_X_bucket_count(cmap_X m);
size_t              cmap_X_capacity(cmap_X m);

void                cmap_X_push_n(cmap_X* self, const cmap_X_input_t arr[], size_t size);

cmap_X_result_t     cmap_X_emplace(cmap_X* self, RawKey rkey, RawMapped rmapped);
cmap_X_result_t     cmap_X_insert(cmap_X* self, cmap_X_input_t rval);
cmap_X_result_t     cmap_X_insert_or_assign(cmap_X* self, RawKey rkey, RawMapped rmapped);
cmap_X_result_t     cmap_X_put(cmap_X* self, RawKey rkey, RawMapped rmapped);
cmap_X_result_t     cmap_X_put_mapped(cmap_X* self, RawKey rkey, Mapped mapped);
cmap_X_mapped_t*    cmap_X_at(const cmap_X* self, RawKey rkey);

size_t              cmap_X_erase(cmap_X* self, RawKey rkey);
void                cmap_X_erase_entry(cmap_X* self, cmap_X_value_t* ref);
cmap_X_iter_t       cmap_X_erase_at(cmap_X* self, cmap_X_iter_t pos);

cmap_X_value_t*     cmap_X_find(const cmap_X* self, RawKey rkey);
bool                cmap_X_contains(const cmap_X* self, RawKey rkey);

cmap_X_iter_t       cmap_X_begin(cmap_X* self);
cmap_X_iter_t       cmap_X_end(cmap_X* self);
void                cmap_X_next(cmap_X_iter_t* it);
cmap_X_mapped_t*    cmap_X_itval(cmap_X_iter_t it);

cmap_bucket_t       cmap_X_bucket(const cmap_X* self, const cmap_X_rawkey_t* rkeyPtr);
uint32_t            c_default_hash(const void *data, size_t len);
uint32_t            c_default_hash32(const void* data, size_t len);
```

## Examples
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
        {"RED", "#FF0000"},
        {"GREEN", "#00FF00"},
        {"BLUE", "#0000FF"}
    });

    // Iterate and print keys and values of unordered map
    c_foreach (n, cmap_str, u) {
        printf("Key:[%s] Value:[%s]\n", n.ref->first.str, n.ref->second.str);
    }

    // Add two new entries to the unordered map
    cmap_str_emplace(&u, "BLACK", "#000000");
    cmap_str_emplace(&u, "WHITE", "#FFFFFF");

    // Output values by key
    printf("The HEX of color RED is:[%s]\n", cmap_str_at(&u, "RED")->str);
    printf("The HEX of color BLACK is:[%s]\n", cmap_str_at(&u, "BLACK")->str);

    cmap_str_del(&u);
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

### Example 2
This example uses a cmap with cstr as mapped value, by the `using_cmap_strval(id, int)` macro.
```c
#include "stc/cstr.h"
#include "stc/cmap.h"

/* cmap<int, cstr>: */
using_cmap_strval(id, int);

int main()
{
    uint32_t col = 0xcc7744ff;
    cmap_id idnames = cmap_inits;
    c_push_items(&idnames, cmap_id, {
        {100, "Red"},
        {110, "Blue"},
    });
    /* put replaces existing mapped value: */
    cmap_id_put(&idnames, 110, "White");
    /* put a constructed mapped value into map: */
    cmap_id_put_mapped(&idnames, 120, cstr_from_fmt("#%08x", col));
    /* emplace inserts only when key does not exist: */
    cmap_id_emplace(&idnames, 100, "Green");

    c_foreach (i, cmap_id, idnames)
        printf("%d: %s\n", i.ref->first, i.ref->second.str);
    puts("");

    cmap_id_del(&idnames);
}
```
Output:
```c
100: Red
110: White
120: #cc7744ff
```

### Example 3
Demonstrate cmap with plain-old-data struct key type Vec3i: cmap<Vec3i, int>:
```c
#include "stc/cmap.h"
#include <stdio.h>

typedef struct { int x, y, z; } Vec3i;

using_cmap(v3, Vec3i, int, c_default_del,
                           c_default_clone,
                           c_mem_equals,
                           c_default_hash32);

int main()
{
    cmap_v3 vecs = cmap_v3_init();

    cmap_v3_put(&vecs, (Vec3i){100,   0,   0}, 1);
    cmap_v3_put(&vecs, (Vec3i){  0, 100,   0}, 2);
    cmap_v3_put(&vecs, (Vec3i){  0,   0, 100}, 3);
    cmap_v3_put(&vecs, (Vec3i){100, 100, 100}, 4);

    c_foreach (i, cmap_v3, vecs)
        printf("{%4d, %4d, %4d}: %d\n", i.ref->first.x,  i.ref->first.y,  i.ref->first.z,  i.ref->second);
    puts("");

    cmap_v3_del(&vecs);
}
```
Output:
```c
{ 100,    0,    0}: 1
{   0,    0,  100}: 3
{ 100,  100,  100}: 4
{   0,  100,    0}: 2
```

### Example 4
Inverse: demonstrate cmap with mapped POD type Vec3f: cmap<int, Vec3f>:
```c
#include "stc/cmap.h"
#include <stdio.h>

typedef struct { int x, y, z; } Vec3i;

using_cmap(iv, int, Vec3i);

int main()
{
    cmap_iv vecs = cmap_iv_init();

    cmap_iv_put(&vecs, 1, (Vec3i){100,   0,   0});
    cmap_iv_put(&vecs, 2, (Vec3i){  0, 100,   0});
    cmap_iv_put(&vecs, 3, (Vec3i){  0,   0, 100});
    cmap_iv_put(&vecs, 4, (Vec3i){100, 100, 100});

    c_foreach (i, cmap_iv, vecs)
        printf("%d: {%4d, %4d, %4d}\n", i.ref->first, i.ref->second.x,  i.ref->second.y,  i.ref->second.z);
    puts("");

    cmap_iv_del(&vecs);
}
```
Output:
```c
4: { 100,  100,  100}
3: {   0,    0,  100}
2: {   0,  100,    0}
1: { 100,    0,    0}
```
