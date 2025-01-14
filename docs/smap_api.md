# STC [smap](../include/stc/smap.h): Sorted Map
![Map](pics/smap.jpg)

A **smap** is a sorted associative container that contains key-value pairs with unique keys. Keys are sorted by
using the comparison function *keyCompare*. Search, removal, and insertion operations have logarithmic complexity.
**smap** is implemented as an AA-tree (Arne Andersson, 1993), which tends to create a flatter structure
(slightly more balanced) than red-black trees.

***Iterator invalidation***: Iterators are invalidated after insert and erase. References are only invalidated
after erase. It is possible to erase individual elements while iterating through the container by using the
returned iterator from *erase_at()*, which references the next element. Alternatively *erase_range()* can be used.

See the c++ class [std::map](https://en.cppreference.com/w/cpp/container/map) for a functional description.

## Header file and declaration

```c++
#define i_type <ct>,<kt>,<vt> // shorthand for defining i_type, i_key, i_val
#define i_type <t>            // container type name (default: smap_{i_key})
// One of the following:
#define i_key <t>             // key type
#define i_keyclass <t>        // key type, and bind <t>_clone() and <t>_drop() function names
#define i_keypro <t>          // key "pro" type, use for cstr, arc, box types

// One of the following:
#define i_val <t>             // mapped value type
#define i_valclass <t>        // mapped type, and bind <t>_clone() and <t>_drop() function names
#define i_valpro <t>          // mapped "pro" type, use for cstr, arc, box types

#define i_cmp <fn>            // three-way compare two i_keyraw* : REQUIRED IF i_keyraw is a non-integral type
#define i_less <fn>           // less comparison. Alternative to i_cmp
#define i_eq <fn>             // equality comparison. Implicitly defined with i_cmp, but not i_less.

#define i_keydrop <fn>        // destroy key func - defaults to empty destruct
#define i_keyclone <fn>       // REQUIRED IF i_valdrop defined
#define i_keyraw <t>          // convertion "raw" type - defaults to i_key
#define i_rawclass <t>        // convertion "raw class". binds <t>_cmp(),  <t>_eq(),  <t>_hash()
#define i_keyfrom <fn>        // convertion func i_keyraw => i_key
#define i_keytoraw <fn>       // convertion func i_key* => i_keyraw

#define i_valdrop <fn>        // destroy value func - defaults to empty destruct
#define i_valclone <fn>       // REQUIRED IF i_valdrop defined
#define i_valraw <t>          // convertion "raw" type - defaults to i_val
#define i_valfrom <fn>        // convertion func i_valraw => i_val
#define i_valtoraw <fn>       // convertion func i_val* => i_valraw

#include "stc/smap.h"
```
- In the following, `X` is the value of `i_key` unless `i_type` is defined.
- **emplace**-functions are only available when `i_keyraw`/`i_valraw` are implicitly or explicitly defined.

## Methods

```c++
smap_X          smap_X_init(void);
sset_X          smap_X_with_capacity(isize cap);

smap_X          smap_X_clone(smap_x map);
void            smap_X_copy(smap_X* self, smap_X other);
void            smap_X_take(smap_X* self, smap_X unowned);                               // take ownership of unowned
smap_X          smap_X_move(smap_X* self);                                               // move
void            smap_X_drop(smap_X* self);                                               // destructor

void            smap_X_clear(smap_X* self);
bool            smap_X_reserve(smap_X* self, isize cap);
void            smap_X_shrink_to_fit(smap_X* self);

bool            smap_X_is_empty(const smap_X* self);
isize           smap_X_size(const smap_X* self);
isize           smap_X_capacity(const smap_X* self);

const X_mapped* smap_X_at(const smap_X* self, i_keyraw rkey);                            // rkey must be in map
X_mapped*       smap_X_at_mut(smap_X* self, i_keyraw rkey);                              // mutable at
const i_key*    smap_X_get(const smap_X* self, i_keyraw rkey);                           // return NULL if not found
i_key*          smap_X_get_mut(smap_X* self, i_keyraw rkey);                             // mutable get
bool            smap_X_contains(const smap_X* self, i_keyraw rkey);
smap_X_iter     smap_X_find(const smap_X* self, i_keyraw rkey);
i_key*          smap_X_find_it(const smap_X* self, i_keyraw rkey, smap_X_iter* out);     // return NULL if not found
smap_X_iter     smap_X_lower_bound(const smap_X* self, i_keyraw rkey);                   // find closest entry >= rkey

i_key*          smap_X_front(const smap_X* self);
i_key*          smap_X_back(const smap_X* self);

smap_X_result   smap_X_insert(smap_X* self, i_key key, i_val mapped);                    // no change if key in map
smap_X_result   smap_X_insert_or_assign(smap_X* self, i_key key, i_val mapped);          // always update mapped
smap_X_result   smap_X_push(smap_X* self, i_key entry);                                  // similar to insert()
smap_X_result   smap_X_put(smap_X* self, i_keyraw rkey, i_valraw rmapped);               // like emplace_or_assign()

smap_X_result   smap_X_emplace(smap_X* self, i_keyraw rkey, i_valraw rmapped);           // no change if rkey in map
smap_X_result   smap_X_emplace_or_assign(smap_X* self, i_keyraw rkey, i_valraw rmapped); // always update rmapped

int             smap_X_erase(smap_X* self, i_keyraw rkey);
smap_X_iter     smap_X_erase_at(smap_X* self, smap_X_iter it);                           // returns iter after it
smap_X_iter     smap_X_erase_range(smap_X* self, smap_X_iter it1, smap_X_iter it2);      // returns updated it2

smap_X_iter     smap_X_begin(const smap_X* self);
smap_X_iter     smap_X_end(const smap_X* self);
void            smap_X_next(smap_X_iter* iter);
smap_X_iter     smap_X_advance(smap_X_iter it, isize n);

i_key           smap_X_value_clone(i_key val);
smap_X_raw      smap_X_value_toraw(const i_key* pval);
void            smap_X_value_drop(i_key* pval);
```
## Types

| Type name          | Type definition                                  | Used to represent...         |
|:-------------------|:-------------------------------------------------|:-----------------------------|
| `smap_X`           | `struct { ... }`                                 | The smap type                |
| `smap_X_key`       | `i_key`                                          | The key type                 |
| `smap_X_mapped`    | `i_val`                                          | The mapped type              |
| `smap_X_value`     | `struct { i_key first; i_val second; }`          | The value: key is immutable  |
| `smap_X_keyraw`    | `i_keyraw`                                       | The raw key type             |
| `smap_X_rmapped`   | `i_valraw`                                       | The raw mapped type          |
| `smap_X_raw`       | `struct { i_keyraw first; i_valraw second; }`    | i_keyraw+i_valraw type       |
| `smap_X_result`    | `struct { smap_X_value *ref; bool inserted; }`   | Result of insert/put/emplace |
| `smap_X_iter`      | `struct { smap_X_value *ref; ... }`              | Iterator type                |

## Examples

[ [Run this code](https://godbolt.org/z/Wc9ed493f) ]
```c++
#include "stc/cstr.h"

#define i_keypro cstr // special macro for i_key = cstr
#define i_valpro cstr // ditto
#include "stc/smap.h"

int main(void)
{
    // Create a sorted map of three strings (maps to string)
    smap_cstr colors = c_make(smap_cstr, {
        {"RED", "#FF0000"},
        {"GREEN", "#00FF00"},
        {"BLUE", "#0000FF"}
    });

    // Iterate and print keys and values of sorted map
    for (c_each(i, smap_cstr, colors)) {
        printf("Key:[%s] Value:[%s]\n", cstr_str(&i.ref->first), cstr_str(&i.ref->second));
    }

    // Add two new entries to the sorted map
    smap_cstr_emplace(&colors, "BLACK", "#000000");
    smap_cstr_emplace(&colors, "WHITE", "#FFFFFF");

    // Output values by key
    printf("The HEX of color RED is:[%s]\n", cstr_str(smap_cstr_at(&colors, "RED")));
    printf("The HEX of color BLACK is:[%s]\n", cstr_str(smap_cstr_at(&colors, "BLACK")));

    smap_cstr_drop(&colors);
}
```

### Example 2
Translate a
[C++ example using *insert* and *emplace*](https://en.cppreference.com/w/cpp/container/map/try_emplace)
 to STC:

[ [Run this code](https://godbolt.org/z/1jYhcjK44) ]
```c++
#include "stc/cstr.h"
#define i_type strmap
#define i_keypro cstr // key = cstr class
#define i_valpro cstr
#include "stc/smap.h"

static void print_node(const strmap_value* node) {
    printf("[%s] = %s\n", cstr_str(&node->first), cstr_str(&node->second));
}

static void print_result(strmap_result result) {
    printf("%s", result.inserted ? "inserted: " : "ignored:  ");
    print_node(result.ref);
}

int main(void)
{
    strmap m = {0};

    print_result( strmap_emplace(&m, "a", "a") );
    print_result( strmap_emplace(&m, "b", "abcd") );
    print_result( strmap_insert(&m, cstr_lit("c"), cstr_with_size(10, 'c') ) );
    print_result( strmap_emplace(&m, "c", "Won't be inserted") );

    for (c_each(p, strmap, m)) { print_node(p.ref); }
    strmap_drop(&m);
}
```

### Example 3
This example uses a smap with cstr as mapped value. Note the `i_valpro` usage.

[ [Run this code](https://godbolt.org/z/7h9q1Gr1x) ]
<!--{%raw%}-->
```c++
#include "stc/cstr.h"

#define i_type IdMap
#define i_key int
#define i_valpro cstr
#include "stc/smap.h"

int main(void)
{
    uint32_t col = 0xcc7744ff;
    IdMap idnames = c_make(IdMap, {{100, "Red"}, {110, "Blue"}});

    // Assign/overwrite an existing mapped value with a const char*
    IdMap_emplace_or_assign(&idnames, 110, "White");

    // Insert (or assign) a new cstr
    IdMap_insert_or_assign(&idnames, 120, cstr_from_fmt("#%08x", col));

    // emplace() adds only when key does not already exist:
    IdMap_emplace(&idnames, 100, "Green"); // ignored

    for (c_each(i, IdMap, idnames))
        printf("%d: %s\n", i.ref->first, cstr_str(&i.ref->second));

    IdMap_drop(&idnames);
}
```
<!--{%endraw%}-->

### Example 4
Demonstrate smap with plain-old-data key type Vec3i and int as mapped type: smap<Vec3i, int>.

[ [Run this code](https://godbolt.org/z/WT1z5eaKa) ]
```c++
#include <stdio.h>
typedef struct { int x, y, z; } Vec3i;

static int Vec3i_cmp(const Vec3i* a, const Vec3i* b) {
    int c;
    if ((c = a->x - b->x) != 0) return c;
    if ((c = a->y - b->y) != 0) return c;
    return a->z - b->z;
}

#define i_type smap_vi, Vec3i, int
#define i_cmp Vec3i_cmp
#include "stc/smap.h"

int main(void)
{
    smap_vi vmap = {0};

    smap_vi_insert(&vmap, (Vec3i){100, 0, 0}, 1);
    smap_vi_insert(&vmap, (Vec3i){0, 100, 0}, 2);
    smap_vi_insert(&vmap, (Vec3i){0, 0, 100}, 3);
    smap_vi_insert(&vmap, (Vec3i){100, 100, 100}, 4);

    for (c_each_kv(v, n, smap_vi, vmap))
        printf("{ %3d, %3d, %3d }: %d\n", v->x, v->y, v->z, *n);

    smap_vi_drop(&vmap);
}
```
