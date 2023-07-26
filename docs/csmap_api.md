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
#define i_key <t>      // key type: REQUIRED.
#define i_val <t>      // mapped value type: REQUIRED.
#define i_type <t>     // container type name (default: cmap_{i_key})
#define i_cmp <f>      // three-way compare two i_keyraw* : REQUIRED IF i_keyraw is a non-integral type

#define i_keydrop <f>  // destroy key func - defaults to empty destruct
#define i_keyclone <f> // REQUIRED IF i_valdrop defined
#define i_keyraw <t>   // convertion "raw" type - defaults to i_key
#define i_keyfrom <f>  // convertion func i_keyraw => i_key
#define i_keyto <f>    // convertion func i_key* => i_keyraw

#define i_valdrop <f>  // destroy value func - defaults to empty destruct
#define i_valclone <f> // REQUIRED IF i_valdrop defined
#define i_valraw <t>   // convertion "raw" type - defaults to i_val
#define i_valfrom <f>  // convertion func i_valraw => i_val
#define i_valto <f>    // convertion func i_val* => i_valraw

#define i_tag <s>      // alternative typename: csmap_{i_tag}. i_tag defaults to i_val
#include <stc/csmap.h>
```
`X` should be replaced by the value of `i_tag` in all of the following documentation.

## Methods

```c
csmap_X               csmap_X_init(void);
csset_X               csmap_X_with_capacity(intptr_t cap);
bool                  csmap_X_reserve(csmap_X* self, intptr_t cap);
void                  csmap_X_shrink_to_fit(csmap_X* self);
csmap_X               csmap_X_clone(csmap_x map);

void                  csmap_X_clear(csmap_X* self);
void                  csmap_X_copy(csmap_X* self, const csmap_X* other);
void                  csmap_X_drop(csmap_X* self);                                               // destructor

bool                  csmap_X_empty(const csmap_X* self);
intptr_t              csmap_X_size(const csmap_X* self);
intptr_t              csmap_X_capacity(const csmap_X* self);

const csmap_X_mapped* csmap_X_at(const csmap_X* self, i_keyraw rkey);                            // rkey must be in map
csmap_X_mapped*       csmap_X_at_mut(csmap_X* self, i_keyraw rkey);                              // mutable at
const csmap_X_value*  csmap_X_get(const csmap_X* self, i_keyraw rkey);                           // return NULL if not found
csmap_X_value*        csmap_X_get_mut(csmap_X* self, i_keyraw rkey);                             // mutable get
bool                  csmap_X_contains(const csmap_X* self, i_keyraw rkey);
csmap_X_iter          csmap_X_find(const csmap_X* self, i_keyraw rkey);
csmap_X_value*        csmap_X_find_it(const csmap_X* self, i_keyraw rkey, csmap_X_iter* out);    // return NULL if not found
csmap_X_iter          csmap_X_lower_bound(const csmap_X* self, i_keyraw rkey);                   // find closest entry >= rkey

csmap_X_value*        csmap_X_front(const csmap_X* self);
csmap_X_value*        csmap_X_back(const csmap_X* self);

csmap_X_result        csmap_X_insert(csmap_X* self, i_key key, i_val mapped);                    // no change if key in map
csmap_X_result        csmap_X_insert_or_assign(csmap_X* self, i_key key, i_val mapped);          // always update mapped
csmap_X_result        csmap_X_push(csmap_X* self, csmap_X_value entry);                          // similar to insert()

csmap_X_result        csmap_X_emplace(csmap_X* self, i_keyraw rkey, i_valraw rmapped);           // no change if rkey in map
csmap_X_result        csmap_X_emplace_or_assign(csmap_X* self, i_keyraw rkey, i_valraw rmapped); // always update rmapped
csmap_X_result        csmap_X_emplace_key(csmap_X* self, i_keyraw rkey);    // if key not in map, mapped is left unassigned

int                   csmap_X_erase(csmap_X* self, i_keyraw rkey);
csmap_X_iter          csmap_X_erase_at(csmap_X* self, csmap_X_iter it);                          // returns iter after it
csmap_X_iter          csmap_X_erase_range(csmap_X* self, csmap_X_iter it1, csmap_X_iter it2);    // returns updated it2

csmap_X_iter          csmap_X_begin(const csmap_X* self);
csmap_X_iter          csmap_X_end(const csmap_X* self);
void                  csmap_X_next(csmap_X_iter* iter);
csmap_X_iter          csmap_X_advance(csmap_X_iter it, intptr_t n);

csmap_X_value         csmap_X_value_clone(csmap_X_value val);
csmap_X_raw           csmap_X_value_toraw(const csmap_X_value* pval);
void                  csmap_X_value_drop(csmap_X_value* pval);
```
## Types

| Type name           | Type definition                                   | Used to represent...         |
|:--------------------|:--------------------------------------------------|:-----------------------------|
| `csmap_X`           | `struct { ... }`                                  | The csmap type               |
| `csmap_X_key`       | `i_key`                                           | The key type                 |
| `csmap_X_mapped`    | `i_val`                                           | The mapped type              |
| `csmap_X_value`     | `struct { i_key first; i_val second; }`           | The value: key is immutable  |
| `csmap_X_keyraw`    | `i_keyraw`                                        | The raw key type             |
| `csmap_X_rmapped`   | `i_valraw`                                        | The raw mapped type          |
| `csmap_X_raw`       | `struct { i_keyraw first; i_valraw second; }`     | i_keyraw+i_valraw type       |
| `csmap_X_result`    | `struct { csmap_X_value *ref; bool inserted; }`   | Result of insert/put/emplace |
| `csmap_X_iter`      | `struct { csmap_X_value *ref; ... }`              | Iterator type                |

## Examples
```c
#define i_implement
#include <stc/cstr.h>
#define i_key_str // special macro for i_key = cstr, i_tag = str
#define i_val_str // ditto
#include <stc/csmap.h>

int main(void)
{
    // Create a sorted map of three strings (maps to string)
    csmap_str colors = c_init(csmap_str, {
        {"RED", "#FF0000"},
        {"GREEN", "#00FF00"},
        {"BLUE", "#0000FF"}
    });

    // Iterate and print keys and values of sorted map
    c_foreach (i, csmap_str, colors) {
        printf("Key:[%s] Value:[%s]\n", cstr_str(&i.ref->first), cstr_str(&i.ref->second));
    }

    // Add two new entries to the sorted map
    csmap_str_emplace(&colors, "BLACK", "#000000");
    csmap_str_emplace(&colors, "WHITE", "#FFFFFF");

    // Output values by key
    printf("The HEX of color RED is:[%s]\n", cstr_str(csmap_str_at(&colors, "RED")));
    printf("The HEX of color BLACK is:[%s]\n", cstr_str(csmap_str_at(&colors, "BLACK")));

    csmap_str_drop(&colors);
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
Translate a 
[C++ example using *insert* and *emplace*](https://en.cppreference.com/w/cpp/container/map/try_emplace)
 to STC:

[ [Run this code](https://godbolt.org/z/b46W5Ezrb) ]
```c
#define i_implement
#include <stc/cstr.h>
#define i_type strmap
#define i_key_str
#define i_val_str
#include <stc/csmap.h>
 
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
    print_result( strmap_insert(&m, cstr_from("c"), cstr_with_size(10, 'c') ) );
    print_result( strmap_emplace(&m, "c", "Won't be inserted") );
 
    c_foreach (p, strmap, m) { print_node(p.ref); }
    strmap_drop(&m);
}
```

### Example 3
This example uses a csmap with cstr as mapped value.
```c
#define i_implement
#include <stc/cstr.h>

#define i_type IDSMap
#define i_key int
#define i_val_str
#include <stc/csmap.h>

int main(void)
{
    uint32_t col = 0xcc7744ff;
    IDSMap idnames = c_init(IDSMap, { {100, "Red"}, {110, "Blue"} });

    // Assign/overwrite an existing mapped value with a const char*
    IDSMap_emplace_or_assign(&idnames, 110, "White");

    // Insert (or assign) a new cstr
    IDSMap_insert_or_assign(&idnames, 120, cstr_from_fmt("#%08x", col));

    // emplace() adds only when key does not already exist:
    IDSMap_emplace(&idnames, 100, "Green"); // ignored

    c_foreach (i, IDSMap, idnames)
        printf("%d: %s\n", i.ref->first, cstr_str(&i.ref->second));

    IDSMap_drop(&idnames);
}
```
Output:
```c
100: Red
110: White
120: #cc7744ff
```

### Example 4
Demonstrate csmap with plain-old-data key type Vec3i and int as mapped type: csmap<Vec3i, int>.
```c
typedef struct { int x, y, z; } Vec3i;

static int Vec3i_cmp(const Vec3i* a, const Vec3i* b) {
    int c;
    if ((c = a->x - b->x) != 0) return c;
    if ((c = a->y - b->y) != 0) return c;
    return a->z - b->z;
}

#define i_key Vec3i
#define i_val int
#define i_cmp Vec3i_cmp
#define i_tag vi
#include <stc/csmap.h>
#include <stdio.h>

int main(void)
{
    csmap_vi vmap = {0};

    csmap_vi_insert(&vmap, (Vec3i){100, 0, 0}, 1);
    csmap_vi_insert(&vmap, (Vec3i){0, 100, 0}, 2);
    csmap_vi_insert(&vmap, (Vec3i){0, 0, 100}, 3);
    csmap_vi_insert(&vmap, (Vec3i){100, 100, 100}, 4);

    c_forpair (v, n, csmap_vi, vmap)
        printf("{ %3d, %3d, %3d }: %d\n", _.v->x, _.v->y, _.v->z, *_.n);

    csmap_vi_drop(&vmap)
}
```
Output:
```c
{   0,   0, 100 }: 3
{   0, 100,   0 }: 2
{ 100,   0,   0 }: 1
{ 100, 100, 100 }: 4
```
