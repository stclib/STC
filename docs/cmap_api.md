# STC [cmap](../stc/cmap.h): Unordered Map
![Map](pics/map.jpg)

A **cmap** is an associative container that contains key-value pairs with unique keys. Search, insertion,
and removal of elements have average constant-time complexity. Internally, the elements are not sorted in any particular order, but organized into buckets. Which bucket an element is placed into depends entirely on the hash of its key. This allows fast access to individual elements, since once the hash is computed, it refers to the exact bucket the element is placed into. It is implemented as closed hashing (aka open addressing) with linear probing, and without leaving tombstones on erase.

See the c++ class [std::unordered_map](https://en.cppreference.com/w/cpp/container/unordered_map) for a functional description.

## Declaration

```c
using_cmap(X, Key, Mapped, keyEqualsRaw=c_default_equals,
                           keyHashRaw=c_default_hash,
                           mappedDestroy=c_default_del,
                           mappedClone=c_default_clone,
                           keyDestroy=c_default_del,
                           keyFromRaw=c_default_clone,
                           keyToRaw=c_default_to_raw,
                           RawKey=Key)

using_cmap_strkey(X, Mapped, mappedDestroy=c_default_del,
                             mappedClone=c_default_clone)

using_cmap_strval(X, Key, keyEquals=c_default_equals,
                          keyHash=c_default_hash,
                          keyDestroy=c_default_del,
                          keyFromRaw=c_default_clone,
                          keyToRaw=c_default_to_raw,
                          RawKey=Key)
using_cmap_str()
```
The macro `using_cmap()` can be instantiated with 3, 5, 7, 9, or 11 arguments in the global scope.
Default values are given above for args not specified. `X` is a type tag name and
will affect the names of all cmap types and methods. E.g. declaring `using_cmap(my, int);`, `X` should
be replaced by `my` in all of the following documentation.

`using_cmap_strkey()` and `using_cmap_strval()` are special macros defined by
`using_cmap()`. The macro `using_cmap_str()` is a shorthand for
```c
using_cmap(str, cstr_t, cstr_t, cstr_del, ...) // uses char* as "raw" types
```

## Header file

All cmap definitions and prototypes may be included in your C source file by including a single header file.

```c
#include "stc/cmap.h"
```
## Methods

```c
cmap_X              cmap_X_init(void);
cmap_X              cmap_X_with_capacity(size_t cap);
void                cmap_X_set_load_factors(cmap_X* self, float min_load, float max_load);

cmap_X              cmap_X_clone(cmap_x map);
void                cmap_X_clear(cmap_X* self);
void                cmap_X_reserve(cmap_X* self, size_t size);
void                cmap_X_swap(cmap_X* a, cmap_X* b);

void                cmap_X_del(cmap_X* self);

bool                cmap_X_empty(cmap_X map);
size_t              cmap_X_size(cmap_X map);
size_t              cmap_X_capacity(cmap_X map);                                             // buckets * max_load_factor
size_t              cmap_X_bucket_count(cmap_X map);                                         // num. of allocated buckets

cmap_X_iter_t       cmap_X_find(const cmap_X* self, RawKey rkey);
bool                cmap_X_contains(const cmap_X* self, RawKey rkey);

void                cmap_X_push_n(cmap_X* self, const cmap_X_rawvalue_t arr[], size_t size);
cmap_X_result_t     cmap_X_emplace(cmap_X* self, RawKey rkey, RawMapped rmapped);            // no change if rkey in map
cmap_X_result_t     cmap_X_put(cmap_X* self, RawKey rkey, RawMapped rmapped);                // std::map::operator[]
cmap_X_result_t     cmap_X_insert(cmap_X* self, Key key, Mapped mapped);                     // like emplace, other params
cmap_X_result_t     cmap_X_insert_or_assign(cmap_X* self, Key key, Mapped mapped);           // like put, other params
cmap_X_mapped_t*    cmap_X_at(const cmap_X* self, RawKey rkey);                              // rkey must be in map

size_t              cmap_X_erase(cmap_X* self, RawKey rkey);
void                cmap_X_erase_entry(cmap_X* self, cmap_X_value_t* entry);
cmap_X_iter_t       cmap_X_erase_at(cmap_X* self, cmap_X_iter_t pos);

cmap_X_iter_t       cmap_X_begin(cmap_X* self);
cmap_X_iter_t       cmap_X_end(cmap_X* self);
void                cmap_X_next(cmap_X_iter_t* it);
cmap_X_mapped_t*    cmap_X_itval(cmap_X_iter_t it);

cmap_X_value_t      cmap_X_value_clone(cmap_X_value_t val);
void                cmap_X_value_del(cmap_X_value_t* val);

uint64_t            c_default_hash(const void *data, size_t len);
uint64_t            c_default_hash32(const void* data, size_t ignored);
uint64_t            c_default_hash64(const void* data, size_t ignored);
```

## Types

| Type name            | Type definition                                 | Used to represent...          |
|:---------------------|:------------------------------------------------|:------------------------------|
| `cmap_X`             | `struct { ... }`                                | The cmap type                 |
| `cmap_X_rawkey_t`    | `RawKey`                                        | The raw key type              |
| `cmap_X_rawmapped_t` | `RawMapped`                                     | The raw mapped type           |
| `cmap_X_key_t`       | `Key`                                           | The key type                  |
| `cmap_X_mapped_t`    | `Mapped`                                        | The mapped type               |
| `cmap_X_value_t`     | `struct { Key first; Mapped second; }`          | The value type                |
| `cmap_X_rawvalue_t`  | `struct { RawKey first; RawMapped second; }`    | RawKey + RawMapped type       |
| `cmap_X_result_t`    | `struct { cmap_X_value_t *first; bool second; }`| Result of insert/put/emplace  |
| `cmap_X_iter_t`      | `struct { cmap_X_value_t *ref; ... }`           | Iterator type                 |

`c_default_hash` requires Key to be 16-bit aligned, and size to be a multiple of 16. There is also a `c_default_hash32` which is slightly faster.

## Constants and macros

| Name                                        | Purpose                |
|:--------------------------------------------|:-----------------------|
|  `c_try_emplace(self, ctype, rkey, mapped)` | Emplace if key exist   |

## Examples
```c
#include <stdio.h>
#include "stc/cstr.h"
#include "stc/cmap.h"

using_cmap_str();

int main()
{
    // Create an unordered_map of three strings (that map to strings)
    c_init (cmap_str, u, {
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
    c_init (cmap_id, idnames, {
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
Demonstrate cmap with plain-old-data key type Vec3i and int as mapped type: cmap<Vec3i, int>. 
```c
#include "stc/cmap.h"
#include <stdio.h>

typedef struct { int x, y, z; } Vec3i;

using_cmap(v3, Vec3i, int, c_mem_equals,    // compare Vec3i bitwise 
                           c_default_hash); // hash Vec3i bitwise.

int main()
{
    cmap_v3 vecs = cmap_v3_init();

    cmap_v3_put(&vecs, (Vec3i){100,   0,   0}, 1);
    cmap_v3_put(&vecs, (Vec3i){  0, 100,   0}, 2);
    cmap_v3_put(&vecs, (Vec3i){  0,   0, 100}, 3);
    cmap_v3_put(&vecs, (Vec3i){100, 100, 100}, 4);

    c_foreach (i, cmap_v3, vecs)
        printf("{ %3d, %3d, %3d }: %d\n", i.ref->first.x,  i.ref->first.y,  i.ref->first.z,  i.ref->second);

    cmap_v3_del(&vecs);
}
```
Output:
```c
{ 100,   0,   0 }: 1
{   0,   0, 100 }: 3
{ 100, 100, 100 }: 4
{   0, 100,   0 }: 2
```

### Example 4
Inverse: demonstrate cmap with mapped POD type Vec3i: cmap<int, Vec3i>:
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
        printf("%d: { %3d, %3d, %3d }\n", i.ref->first, i.ref->second.x,  i.ref->second.y,  i.ref->second.z);

    cmap_iv_del(&vecs);
}
```
Output:
```c
4: { 100, 100, 100 }
3: {   0,   0, 100 }
2: {   0, 100,   0 }
1: { 100,   0,   0 }
```

### Example 5
Advanced, rare usage: Complex key type.
```c
#include <stdio.h>
#include <stc/cmap.h>
#include <stc/cstr.h>

typedef struct Viking {
    cstr name;
    cstr country;
} Viking;

void viking_del(Viking* vk) {
    cstr_del(&vk->name);
    cstr_del(&vk->country);
}

// Define Viking raw struct with hash, equals, and convertion functions between Viking and VikingRaw structs:

typedef struct VikingRaw {
    const char* name;
    const char* country;
} VikingRaw;

uint32_t vikingraw_hash(const VikingRaw* raw, size_t ignore) {
    uint32_t hash = c_strhash(raw->name) ^ (c_strhash(raw->country) << 3);
    return hash;
}
static inline int vikingraw_equals(const VikingRaw* rx, const VikingRaw* ry) {
    return strcmp(rx->name, ry->name) == 0 && strcmp(rx->country, ry->country) == 0;
}

static inline Viking viking_fromRaw(VikingRaw raw) { // note: parameter is by value
    Viking vk = {cstr_from(raw.name), cstr_from(raw.country)}; return vk;
}

static inline VikingRaw viking_toRaw(Viking* vk) {
    VikingRaw raw = {vk->name.str, vk->country.str}; return raw;
}

// With this in place, we use the full using_cmap() macro to define {Viking -> int} hash map type:

using_cmap(vk, Viking, int, vikingraw_equals, vikingraw_hash,
                            c_default_del, c_default_clone, // mapped: int => use default.
                            viking_del, viking_fromRaw, viking_toRaw, VikingRaw);

int main()
{
    c_init (cmap_vk, vikings, {
        { {"Einar", "Norway"}, 20 },
        { {"Olaf", "Denmark"}, 24 },
        { {"Harald", "Iceland"}, 12 },
    });
    cmap_vk_put(&vikings, (VikingRaw){"Bjorn", "Sweden"}, 10);
    
    VikingRaw lookup = {"Einar", "Norway"};

    cmap_vk_value_t *e = cmap_vk_find(&vikings, lookup).ref;
    e->second += 3; // add 3 hp points to Einar
    cmap_vk_emplace(&vikings, lookup, 0).first->second += 5; // add 5 more to Einar

    c_foreach (k, cmap_vk, vikings) {
        printf("%s of %s has %d hp\n", k.ref->first.name.str, k.ref->first.country.str, k.ref->second);
    }
    cmap_vk_del(&vikings);
}
```
Output:
```
Olaf of Denmark has 24 hp
Bjorn of Sweden has 10 hp
Einar of Norway has 28 hp
Harald of Iceland has 12 hp
```
