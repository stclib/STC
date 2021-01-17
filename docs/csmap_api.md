# STC Container [csmap](../stc/csmap.h): Sorted Map
![Map](pics/smap.jpg)

A **csmap** is a sorted associative container that contains key-value pairs with unique keys. Keys are sorted by using the comparison function *keyCompare*. Search, removal, and insertion operations have logarithmic complexity. **csmap** is implemented as an AA-tree.
See [std::map](https://en.cppreference.com/w/cpp/container/map) for a similar c++ class.

## Declaration

```c
#define using_csmap(X, Key, Mapped, mappedDestroy=c_default_del,
                                    mappedClone=c_default_clone,
                                    keyCompareRaw=c_default_compare,
                                    keyDestroy=c_default_del,
                                    keyFromRaw=c_default_clone,
                                    keyToRaw=c_default_to_raw,
                                    RawKey=Key)

#define using_csmap_strkey(X, Mapped, mappedDestroy=c_default_del,
                                      mappedClone=c_default_clone)

#define using_csmap_strval(X, Key, keyCompare=c_default_compare,
                                   keyDestroy=c_default_del,
                                   keyFromRaw=c_default_clone,
                                   keyToRaw=c_default_to_raw,
                                   RawKey=Key)
#define using_csmap_str()
```
The macro `using_csmap()` can be instantiated with 3, 5, 6, 8, or 10 arguments in the global scope.
Default values are given above for args not specified. `X` is a type tag name and
will affect the names of all csmap types and methods. E.g. declaring `using_csmap(my, int);`, `X` should
be replaced by `my` in all of the following documentation.

`using_csmap_strkey()` and `using_csmap_strval()` are special macros defined by
`using_csmap()`. The macro `using_csmap_str()` is a shorthand for
```c
using_csmap(str, cstr_t, cstr_t, cstr_compare_raw, cstr_del, cstr_from, ...)
```

## Types

| Type name             | Type definition                                  | Used to represent...         |
|:----------------------|:------------------------------------------------ |:-----------------------------|
| `csmap_X`             | `struct { ... }`                                 | The csmap type               |
| `csmap_X_rawkey_t`    | `RawKey`                                         | The raw key type             |
| `csmap_X_rawmapped_t` | `RawMapped`                                      | The raw mapped type          |
| `csmap_X_key_t`       | `Key`                                            | The key type                 |
| `csmap_X_mapped_t`    | `Mapped`                                         | The mapped type              |
| `csmap_X_value_t`     | `struct { Key first; Mapped second; }`           | The value type               |
| `csmap_X_rawvalue_t`  | `struct { RawKey first; RawMapped second; }`     | RawKey + RawVal type         |
| `csmap_X_result_t`    | `struct { csmap_X_value_t first; bool second; }` | Result of insert/put/emplace |
| `csmap_X_iter_t`      | `struct { csmap_X_value_t *ref; ... }`           | Iterator type                |

## Header file

All csmap definitions and prototypes may be included in your C source file by including a single header file.

```c
#include "stc/csmap.h"
```
## Methods

```c
csmap_X             csmap_X_init(void);
csmap_X             csmap_X_clone(csmap_x map);
void                csmap_X_clear(csmap_X* self);
void                csmap_X_swap(csmap_X* a, csmap_X* b);
void                csmap_X_del(csmap_X* self);

bool                csmap_X_empty(csmap_X m);
size_t              csmap_X_size(csmap_X m);

void                csmap_X_push_n(csmap_X* self, const csmap_X_rawvalue_t arr[], size_t size);

csmap_X_result_t    csmap_X_emplace(csmap_X* self, RawKey rkey, RawMapped rmapped);
csmap_X_result_t    csmap_X_insert(csmap_X* self, csmap_X_rawvalue_t rval);
csmap_X_result_t    csmap_X_insert_or_assign(csmap_X* self, RawKey rkey, RawMapped rmapped);
csmap_X_result_t    csmap_X_put(csmap_X* self, RawKey rkey, RawMapped rmapped);
csmap_X_result_t    csmap_X_put_mapped(csmap_X* self, RawKey rkey, Mapped mapped);
csmap_X_mapped_t*   csmap_X_at(const csmap_X* self, RawKey rkey);

size_t              csmap_X_erase(csmap_X* self, RawKey rkey);
csmap_X_iter_t      csmap_X_erase_at(csmap_X* self, csmap_X_iter_t pos);

csmap_X_value_t*    csmap_X_find(const csmap_X* self, RawKey rkey);
bool                csmap_X_contains(const csmap_X* self, RawKey rkey);

csmap_X_iter_t      csmap_X_begin(csmap_X* self);
csmap_X_iter_t      csmap_X_end(csmap_X* self);
void                csmap_X_next(csmap_X_iter_t* it);
csmap_X_mapped_t*   csmap_X_itval(csmap_X_iter_t it);

csmap_X_value_t     csmap_X_value_clone(csmap_X_value_t val);
void                csmap_X_value_del(csmap_X_value_t* val);
```

## Examples
```c
#include <stdio.h>
#include "stc/cstr.h"
#include "stc/csmap.h"

using_csmap_str();

int main()
{
    // Create an unordered_map of three strings (that map to strings)
    c_init (csmap_str, u, {
        {"RED", "#FF0000"},
        {"GREEN", "#00FF00"},
        {"BLUE", "#0000FF"}
    });

    // Iterate and print keys and values of unordered map
    c_foreach (n, csmap_str, u) {
        printf("Key:[%s] Value:[%s]\n", n.ref->first.str, n.ref->second.str);
    }

    // Add two new entries to the unordered map
    csmap_str_emplace(&u, "BLACK", "#000000");
    csmap_str_emplace(&u, "WHITE", "#FFFFFF");

    // Output values by key
    printf("The HEX of color RED is:[%s]\n", csmap_str_at(&u, "RED")->str);
    printf("The HEX of color BLACK is:[%s]\n", csmap_str_at(&u, "BLACK")->str);

    csmap_str_del(&u);
    return 0;
}
```
Output:
```
Key:[BLUE] Value:[#0000FF]
Key:[GREEN] Value:[#00FF00]
Key:[RED] Value:[#FF0000]
The HEX of color RED is:[#FF0000]
The HEX of color BLACK is:[#000000]
```

### Example 2
This example uses a csmap with cstr as mapped value, by the `using_csmap_strval(id, int)` macro.
```c
#include "stc/cstr.h"
#include "stc/csmap.h"

/* csmap<int, cstr>: */
using_csmap_strval(id, int);

int main()
{
    uint32_t col = 0xcc7744ff;
    c_init (csmap_id, idnames, {
        {100, "Red"},
        {110, "Blue"},
    });
    /* put replaces existing mapped value: */
    csmap_id_put(&idnames, 110, "White");
    /* put a constructed mapped value into map: */
    csmap_id_put_mapped(&idnames, 120, cstr_from_fmt("#%08x", col));
    /* emplace inserts only when key does not exist: */
    csmap_id_emplace(&idnames, 100, "Green");

    c_foreach (i, csmap_id, idnames)
        printf("%d: %s\n", i.ref->first, i.ref->second.str);

    csmap_id_del(&idnames);
}
```
Output:
```c
100: Red
110: White
120: #cc7744ff
```

### Example 3
Demonstrate csmap with plain-old-data key type Vec3i and int as mapped type: csmap<Vec3i, int>. 
```c
#include "stc/csmap.h"
#include <stdio.h>

typedef struct { int x, y, z; } Vec3i;

static int Vec3i_compare(const Vec3i* a, const Vec3i* b) {
    if (a->x != b->x) return 1 - ((a->x < b->x)<<1);
    if (a->y != b->y) return 1 - ((a->y < b->y)<<1);
    return c_default_compare(&a->z, &b->z);
}

using_csmap(v3, Vec3i, int, c_default_del,     // mapped: empty int destroy func
                            c_default_clone,   // mapped: clone int by "memcpy"
                            Vec3i_compare);    // key: compare Vec3i 

int main()
{
    csmap_v3 vecs = csmap_v3_init();

    csmap_v3_put(&vecs, (Vec3i){100,   0,   0}, 1);
    csmap_v3_put(&vecs, (Vec3i){  0, 100,   0}, 2);
    csmap_v3_put(&vecs, (Vec3i){  0,   0, 100}, 3);
    csmap_v3_put(&vecs, (Vec3i){100, 100, 100}, 4);

    c_foreach (i, csmap_v3, vecs)
        printf("{ %3d, %3d, %3d }: %d\n", i.ref->first.x,  i.ref->first.y,  i.ref->first.z,  i.ref->second);

    csmap_v3_del(&vecs);
}
```
Output:
```c
{   0,   0, 100 }: 3
{   0, 100,   0 }: 2
{ 100,   0,   0 }: 1
{ 100, 100, 100 }: 4
```

### Example 4
Inverse: demonstrate csmap with mapped POD type Vec3i: csmap<int, Vec3i>:
```c
#include "stc/csmap.h"
#include <stdio.h>

typedef struct { int x, y, z; } Vec3i;
using_csmap(iv, int, Vec3i);

int main()
{
    csmap_iv vecs = csmap_iv_init();
    csmap_iv_put(&vecs, 1, (Vec3i){100,   0,   0});
    csmap_iv_put(&vecs, 2, (Vec3i){  0, 100,   0});
    csmap_iv_put(&vecs, 3, (Vec3i){  0,   0, 100});
    csmap_iv_put(&vecs, 4, (Vec3i){100, 100, 100});

    c_foreach (i, csmap_iv, vecs)
        printf("%d: { %3d, %3d, %3d }\n", i.ref->first, i.ref->second.x,  i.ref->second.y,  i.ref->second.z);

    csmap_iv_del(&vecs);
}
```
Output:
```c
1: { 100,   0,   0 }
2: {   0, 100,   0 }
3: {   0,   0, 100 }
4: { 100, 100, 100 }
```
