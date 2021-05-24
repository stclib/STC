# STC [csmap](../include/stc/csmap.h): Sorted Map
![Map](pics/smap.jpg)

A **csmap** is a sorted associative container that contains key-value pairs with unique keys. Keys are sorted by
using the comparison function *keyCompare*. Search, removal, and insertion operations have logarithmic complexity.
**csmap** is implemented as an AA-tree (Arne Andersson, 1993), which tends to create a flatter structure
(slightly more balanced) than red-black trees.

***Iterator invalidation***: Iterators are invalidated after insert and erase. References are only invalidated
after erase. It is possible to erase individual elements while iterating through the container by using the 
returned iterator from *erase_at()*, which references the next element. Alternatively *erase_range()* can be used.

See the c++ class [std::map](https://en.cppreference.com/w/cpp/container/map) for a functional description.

## Header file and declaration

```c
#include <stc/csmap.h>

using_csmap(X, Key, Mapped);
using_csmap(X, Key, Mapped, keyCompare);
using_csmap(X, Key, Mapped, keyCompare, mappedDel, mappedClone);
using_csmap(X, Key, Mapped, keyCompare, mappedDel, mappedFromRaw, mappedToRaw, RawMapped);
using_csmap(X, Key, Mapped, keyCompareRaw, mappedDel, mappedFromRaw, mappedToRaw, RawMapped,
                                           keyDel, keyFromRaw, keyToRaw, RawKey);
using_csmap_keydef(X, Key, Mapped, keyCompare, keyDel, keyClone);
using_csmap_keydef(X, Key, Mapped, keyCompareRaw, keyDel, keyFromRaw, keyToRaw, RawKey);

using_csmap_strkey(X, Mapped);                    // using_csmap(X, cstr, Mapped, ...)
using_csmap_strkey(X, Mapped, mappedDel, mappedClone);
using_csmap_strkey(X, Mapped, mappedDel, mappedFromRaw, mappedToRaw, RawMapped);

using_csmap_strval(X, Key);                       // using_csmap(X, Key, cstr, ...)
using_csmap_strval(X, Key, keyCompare);
using_csmap_strval(X, Key, keyCompare, keyDel, keyClone);
using_csmap_strval(X, Key, keyCompareRaw, keyDel, keyFromRaw, keyToRaw, RawKey);

using_csmap_str();                                // using_csmap(str, cstr, cstr, ...)
```
The `using_csmap()` macro family must be instantiated in the global scope. `X` is a type tag name and
will affect the names of all csmap types and methods. E.g. declaring `using_csmap(ii, int, int);`, `X` should
be replaced by `ii` in all of the following documentation.

## Methods

```c
csmap_X             csmap_X_init(void);
csmap_X             csmap_X_clone(csmap_x map);

void                csmap_X_clear(csmap_X* self);
void                csmap_X_swap(csmap_X* a, csmap_X* b);
void                csmap_X_del(csmap_X* self);                                                 // destructor

bool                csmap_X_empty(csmap_X map);
size_t              csmap_X_size(csmap_X map);

bool                csmap_X_contains(const csmap_X* self, RawKey rkey);
csmap_X_mapped_t*   csmap_X_at(const csmap_X* self, RawKey rkey);                               // rkey must be in map.
csmap_X_value_t*    csmap_X_get(const csmap_X* self, RawKey rkey);                              // return NULL if not found
csmap_X_iter_t      csmap_X_lower_bound(const csmap_X* self, RawKey rkey);                      // find closest entry >= rkey
csmap_X_iter_t      csmap_X_find(const csmap_X* self, RawKey rkey);
csmap_X_value_t*    csmap_X_find_it(const csmap_X* self, RawKey rkey, csmap_X_iter_t* out);     // return NULL if not found

csmap_X_result_t    csmap_X_insert(csmap_X* self, Key key, Mapped mapped);                      // no change if key in map
csmap_X_result_t    csmap_X_insert_or_assign(csmap_X* self, Key key, Mapped mapped);            // always update mapped
csmap_X_result_t    csmap_X_put(csmap_X* self, Key key, Mapped mapped);                         // same as insert_or_assign()

csmap_X_result_t    csmap_X_emplace(csmap_X* self, RawKey rkey, RawMapped rmapped);             // no change if rkey in map
csmap_X_result_t    csmap_X_emplace_or_assign(csmap_X* self, RawKey rkey, RawMapped rmapped);   // always update rmapped
void                csmap_X_emplace_items(csmap_X* self, const csmap_X_rawvalue_t arr[], size_t n);

size_t              csmap_X_erase(csmap_X* self, RawKey rkey);
csmap_X_iter_t      csmap_X_erase_at(csmap_X* self, csmap_X_iter_t it);                         // returns iter after it
csmap_X_iter_t      csmap_X_erase_range(csmap_X* self, csmap_X_iter_t it1, csmap_X_iter_t it2); // returns updated it2

csmap_X_iter_t      csmap_X_begin(const csmap_X* self);
csmap_X_iter_t      csmap_X_end(const csmap_X* self);
void                csmap_X_next(csmap_X_iter_t* iter);
csmap_X_iter_t      csmap_X_fwd(csmap_X_iter_t it, size_t n);

csmap_X_value_t     csmap_X_value_clone(csmap_X_value_t val);
csmap_X_rawvalue_t  csmap_X_value_toraw(csmap_X_value_t* pval);
```
## Types

| Type name             | Type definition                                   | Used to represent...         |
|:----------------------|:--------------------------------------------------|:-----------------------------|
| `csmap_X`             | `struct { ... }`                                  | The csmap type               |
| `csmap_X_rawkey_t`    | `RawKey`                                          | The raw key type             |
| `csmap_X_rawmapped_t` | `RawMapped`                                       | The raw mapped type          |
| `csmap_X_rawvalue_t`  | `struct { RawKey first; RawMapped second; }`      | RawKey+RawMapped type        |
| `csmap_X_key_t`       | `Key`                                             | The key type                 |
| `csmap_X_mapped_t`    | `Mapped`                                          | The mapped type              |
| `csmap_X_value_t`     | `struct { const Key first; Mapped second; }`      | The value: key is immutable  |
| `csmap_X_result_t`    | `struct { csmap_X_value_t *ref; bool inserted; }` | Result of insert/put/emplace |
| `csmap_X_iter_t`      | `struct { csmap_X_value_t *ref; ... }`            | Iterator type                |

## Examples
```c
#include <stc/csmap.h>
#include <stc/cstr.h>

using_csmap_str();

int main()
{
    // Create an unordered_map of three strings (maps to string)
    c_var (csmap_str, colors, {
        {"RED", "#FF0000"},
        {"GREEN", "#00FF00"},
        {"BLUE", "#0000FF"}
    });

    // Iterate and print keys and values of unordered map
    c_foreach (i, csmap_str, colors) {
        printf("Key:[%s] Value:[%s]\n", i.ref->first.str, i.ref->second.str);
    }

    // Add two new entries to the unordered map
    csmap_str_emplace(&colors, "BLACK", "#000000");
    csmap_str_emplace(&colors, "WHITE", "#FFFFFF");

    // Output values by key
    printf("The HEX of color RED is:[%s]\n", csmap_str_at(&colors, "RED")->str);
    printf("The HEX of color BLACK is:[%s]\n", csmap_str_at(&colors, "BLACK")->str);

    csmap_str_del(&colors);
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
#include <stc/csmap.h>
#include <stc/cstr.h>

/* csmap<int, cstr>: */
using_csmap_strval(id, int);

int main()
{
    uint32_t col = 0xcc7744ff;
    c_var (csmap_id, idnames, {
        {100, "Red"},
        {110, "Blue"},
    });
    c_fordefer (csmap_id_del(&idnames)) 
    {
        // put replaces existing mapped value:
        csmap_id_emplace_or_assign(&idnames, 110, "White");
        // put a constructed mapped value into map:
        csmap_id_insert_or_assign(&idnames, 120, cstr_from_fmt("#%08x", col));
        // emplace adds only when key does not exist:
        csmap_id_emplace(&idnames, 100, "Green");

        c_foreach (i, csmap_id, idnames)
            printf("%d: %s\n", i.ref->first, i.ref->second.str);
    }
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
#include <stc/csmap.h>
#include <stdio.h>

typedef struct { int x, y, z; } Vec3i;

static int Vec3i_compare(const Vec3i* a, const Vec3i* b) {
    // optimal way to return -1, 0, 1:
    if (a->x != b->x) return 1 - ((a->x < b->x)<<1);
    if (a->y != b->y) return 1 - ((a->y < b->y)<<1);
    return (a->z > b->z) - (a->z < b->z);
}

using_csmap(vi, Vec3i, int, Vec3i_compare);

int main()
{
    c_forvar (csmap_vi vecs = csmap_vi_init(), csmap_vi_del(&vecs))
    {
      csmap_vi_insert(&vecs, (Vec3i){100,   0,   0}, 1);
      csmap_vi_insert(&vecs, (Vec3i){  0, 100,   0}, 2);
      csmap_vi_insert(&vecs, (Vec3i){  0,   0, 100}, 3);
      csmap_vi_insert(&vecs, (Vec3i){100, 100, 100}, 4);

      c_foreach (i, csmap_vi, vecs)
          printf("{ %3d, %3d, %3d }: %d\n", i.ref->first.x, i.ref->first.y, i.ref->first.z, i.ref->second);
    }
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
#include <stc/csmap.h>
#include <stdio.h>

typedef struct { int x, y, z; } Vec3i;
using_csmap(iv, int, Vec3i);

int main()
{
    c_forvar (csmap_iv vecs = csmap_iv_init(), csmap_iv_del(&vecs))
    {
        csmap_iv_insert(&vecs, 1, (Vec3i){100,   0,   0});
        csmap_iv_insert(&vecs, 2, (Vec3i){  0, 100,   0});
        csmap_iv_insert(&vecs, 3, (Vec3i){  0,   0, 100});
        csmap_iv_insert(&vecs, 4, (Vec3i){100, 100, 100});

        c_foreach (i, csmap_iv, vecs)
            printf("%d: { %3d, %3d, %3d }\n", i.ref->first, i.ref->second.x, i.ref->second.y, i.ref->second.z);
    }
}
```
Output:
```c
1: { 100,   0,   0 }
2: {   0, 100,   0 }
3: {   0,   0, 100 }
4: { 100, 100, 100 }
```
