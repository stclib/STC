# STC [hmap](../include/stc/hmap.h): HashMap (unordered)
![Map](pics/map.jpg)

A **hmap** is an associative container that contains key-value pairs with unique keys. Search, insertion, and removal of elements
have average constant-time complexity. Internally, the elements are not sorted in any particular order, but organized into
buckets. Which bucket an element is placed into depends entirely on the hash of its key. This allows fast access to individual
elements, since once the hash is computed, it refers to the exact bucket the element is placed into. It is implemented as closed
hashing (aka open addressing) with linear probing, and without leaving tombstones on erase.

***Iterator invalidation***: References and iterators are invalidated after erase. No iterators are invalidated after insert,
unless the hash-table need to be extended. The hash table size can be reserved prior to inserts if the total max size is known.
The order of elements may not be preserved after erase. It is still possible to erase elements when iterating through
the container by setting the iterator to the value returned from *erase_at()*, which references the next element. Note that a small number of elements may be visited twice when doing this, but all elements will be visited.

See the c++ class [std::unordered_map](https://en.cppreference.com/w/cpp/container/unordered_map) for a functional description.

## Header file and declaration

```c++
#define i_type <ct>,<kt>,<vt> // shorthand for defining i_type, i_key, i_val
#define i_type <t>            // container type name (default: hmap_{i_key})
// One of the following:
#define i_key <t>             // key type
#define i_keyclass <t>        // key type, and bind <t>_clone() and <t>_drop() function names
#define i_keypro <t>          // key "pro" type, use for cstr, arc, box types

// One of the following:
#define i_val <t>             // mapped value type
#define i_valclass <t>        // mapped type, and bind <t>_clone() and <t>_drop() function names
#define i_valpro <t>          // mapped "pro" type, use for cstr, arc, box types

#define i_hash <fn>           // hash func i_keyraw*: REQUIRED IF i_keyraw is non-pod type
#define i_eq <fn>             // equality comparison two i_keyraw*: REQUIRED IF i_keyraw is a
                              // non-integral type. Three-way i_cmp may be specified instead.
#define i_keydrop <fn>        // destroy key func - defaults to empty destruct
#define i_keyclone <fn>       // REQUIRED IF i_keydrop defined
#define i_keyraw <t>          // convertion "raw" type - defaults to i_key
#define i_rawclass <t>        // convertion "raw class". binds <t>_cmp(),  <t>_eq(),  <t>_hash()
#define i_keyfrom <fn>        // convertion func i_keyraw => i_key
#define i_keytoraw <fn>       // convertion func i_key* => i_keyraw

#define i_valdrop <fn>        // destroy value func - defaults to empty destruct
#define i_valclone <fn>       // REQUIRED IF i_valdrop defined
#define i_valraw <t>          // convertion "raw" type - defaults to i_val
#define i_valfrom <fn>        // convertion func i_valraw => i_val
#define i_valtoraw <fn>       // convertion func i_val* => i_valraw

#include "stc/hmap.h"
```
- In the following, `X` is the value of `i_key` unless `i_type` is defined.
- **emplace**-functions are only available when `i_keyraw`/`i_valraw` are implicitly or explicitly defined.
## Methods

```c++
hmap_X          hmap_X_init(void);
hmap_X          hmap_X_with_capacity(isize cap);

hmap_X          hmap_X_clone(hmap_x map);
void            hmap_X_copy(hmap_X* self, hmap_X other);
void            hmap_X_take(hmap_X* self, hmap_X unowned);                        // take ownership of unowned
hmap_X          hmap_X_move(hmap_X* self);                                        // move
void            hmap_X_drop(hmap_X* self);                                        // destructor

void            hmap_X_clear(hmap_X* self);
float           hmap_X_max_load_factor(const hmap_X* self);                       // default: 0.85f
bool            hmap_X_reserve(hmap_X* self, isize size);
void            hmap_X_shrink_to_fit(hmap_X* self);

bool            hmap_X_is_empty(const hmap_X* self );
isize           hmap_X_size(const hmap_X* self);
isize           hmap_X_capacity(const hmap_X* self);                              // buckets * max_load_factor
isize           hmap_X_bucket_count(const hmap_X* self);                          // num. of allocated buckets

const i_val*    hmap_X_at(const hmap_X* self, i_keyraw rkey);                     // rkey must be in map
i_val*          hmap_X_at_mut(hmap_X* self, i_keyraw rkey);                       // mutable at
const X_value*  hmap_X_get(const hmap_X* self, i_keyraw rkey);                    // const get
X_value*        hmap_X_get_mut(hmap_X* self, i_keyraw rkey);                      // mutable get
bool            hmap_X_contains(const hmap_X* self, i_keyraw rkey);
hmap_X_iter     hmap_X_find(const hmap_X* self, i_keyraw rkey);                   // find element

hmap_X_result   hmap_X_insert(hmap_X* self, i_key key, i_val mapped);             // no change if key in map
hmap_X_result   hmap_X_insert_or_assign(hmap_X* self, i_key key, i_val mapped);   // always update mapped
hmap_X_result   hmap_X_push(hmap_X* self, hmap_X_value entry);                    // similar to insert
hmap_X_result   hmap_X_put(hmap_X* self, i_keyraw rkey, i_valraw rmapped);        // like emplace_or_assign()

hmap_X_result   hmap_X_emplace(hmap_X* self, i_keyraw rkey, i_valraw rmapped);    // no change if rkey in map
hmap_X_result   hmap_X_emplace_or_assign(hmap_X* self, i_keyraw rkey, i_valraw rmapped); // always update mapped

int             hmap_X_erase(hmap_X* self, i_keyraw rkey);                        // return 0 or 1
hmap_X_iter     hmap_X_erase_at(hmap_X* self, hmap_X_iter it);                    // return iter after it
void            hmap_X_erase_entry(hmap_X* self, hmap_X_value* entry);

hmap_X_iter     hmap_X_begin(const hmap_X* self);
hmap_X_iter     hmap_X_end(const hmap_X* self);
void            hmap_X_next(hmap_X_iter* it);
hmap_X_iter     hmap_X_advance(hmap_X_iter it, hmap_X_ssize n);

hmap_X_value    hmap_X_value_clone(hmap_X_value val);
hmap_X_raw      hmap_X_value_toraw(hmap_X_value* pval);
```
Free helper functions:
```c++
size_t          c_hash_n(const void *data, isize n);                  // generic hash function of n bytes
size_t          c_hash_str(const char *str);                          // string hash function, uses strlen()
size_t          c_hash_mix(size_t h1, size_t h2, ...);                // mix/combine computed hashes
isize           c_next_pow2(isize k);                                 // get next power of 2 >= k

// hash/equal template default functions:
size_t          c_default_hash(const T *obj);                         // alias for c_hash_n(obj, sizeof *obj)
bool            c_default_eq(const i_keyraw* a, const i_keyraw* b);   // *a == *b
bool            c_memcmp_eq(const i_keyraw* a, const i_keyraw* b);    // !memcmp(a, b, sizeof *a)
```

## Types

| Type name          | Type definition                                 | Used to represent...          |
|:-------------------|:------------------------------------------------|:------------------------------|
| `hmap_X`           | `struct { ... }`                                | The hmap type                 |
| `hmap_X_key`       | `i_key`                                         | The key type                  |
| `hmap_X_mapped`    | `i_val`                                         | The mapped type               |
| `hmap_X_value`     | `struct { const i_key first; i_val second; }`   | The value: key is immutable   |
| `hmap_X_keyraw`    | `i_keyraw`                                      | The raw key type              |
| `hmap_X_rmapped`   | `i_valraw`                                      | The raw mapped type           |
| `hmap_X_raw`       | `struct { i_keyraw first; i_valraw second; }`   | i_keyraw + i_valraw type      |
| `hmap_X_result`    | `struct { hmap_X_value *ref; bool inserted; }`  | Result of insert/emplace      |
| `hmap_X_iter`      | `struct { hmap_X_value *ref; ... }`             | Iterator type                 |

## Examples

[ [Run this code](https://godbolt.org/z/ov349P5Y1) ]
```c++
#include "stc/cstr.h"

#define i_keypro cstr
#define i_valpro cstr
#include "stc/hmap.h"

int main(void)
{
    // Create an unordered_map of three strings (that map to strings)
    hmap_cstr umap = c_make(hmap_cstr, {
        {"RED", "#FF0000"},
        {"GREEN", "#00FF00"},
        {"BLUE", "#0000FF"}
    });

    // Iterate and print keys and values of unordered map
    for (c_each(n, hmap_cstr, umap)) {
        hmap_cstr_raw v = hmap_cstr_value_toraw(n.ref);
        printf("Key:[%s] Value:[%s]\n", v.first, v.second);
    }

    // Add two new entries to the unordered map
    hmap_cstr_emplace(&umap, "BLACK", "#000000");
    hmap_cstr_emplace(&umap, "WHITE", "#FFFFFF");

    // Output values by key
    printf("The HEX of color RED is:[%s]\n", cstr_str(hmap_cstr_at(&umap, "RED")));
    printf("The HEX of color BLACK is:[%s]\n", cstr_str(hmap_cstr_at(&umap, "BLACK")));

    hmap_cstr_drop(&umap);
}
```

### Example 2
Demonstrate hmap with mapped POD type Vec3i: hmap<int, Vec3i>:

[ [Run this code](https://godbolt.org/z/nhjMvvWjo) ]
```c++
#include <stdio.h>
typedef struct { int x, y, z; } Vec3i;

#define i_type hmap_iv, int, Vec3i
#include "stc/hmap.h"

int main(void)
{
    hmap_iv vecs = {0};

    hmap_iv_insert(&vecs, 1, (Vec3i){100,   0,   0});
    hmap_iv_insert(&vecs, 2, (Vec3i){  0, 100,   0});
    hmap_iv_insert(&vecs, 3, (Vec3i){  0,   0, 100});
    hmap_iv_insert(&vecs, 4, (Vec3i){100, 100, 100});

    for (c_each_kv(num, v3, hmap_iv, vecs))
        printf("%d: { %3d, %3d, %3d }\n", *num, v3->x, v3->y, v3->z);

    hmap_iv_drop(&vecs);
}
```

### Example 3
Inverse: Demonstrate hmap with plain-old-data key type Vec3i and int as mapped type: hmap<Vec3i, int>.

[ [Run this code](https://godbolt.org/z/9qznc7Gec) ]
```c++
#include <stdio.h>
typedef struct { int x, y, z; } Vec3i;

#define i_type hmap_vi, Vec3i, int
#define i_eq c_memcmp_eq // bitwise equal
#include "stc/hmap.h"

int main(void)
{
    // Define map with defered destruct
    hmap_vi vecs = {0};

    hmap_vi_insert(&vecs, (Vec3i){100,   0,   0}, 1);
    hmap_vi_insert(&vecs, (Vec3i){  0, 100,   0}, 2);
    hmap_vi_insert(&vecs, (Vec3i){  0,   0, 100}, 3);
    hmap_vi_insert(&vecs, (Vec3i){100, 100, 100}, 4);

    for (c_each_kv(v3, num, hmap_vi, vecs))
        printf("{ %3d, %3d, %3d }: %d\n", v3->x, v3->y, v3->z, *num);

    hmap_vi_drop(&vecs);
}
```

### Example 4: Advanced
Key type is struct. Based on https://doc.rust-lang.org/std/collections/struct.HashMap.html

[ [Run this code](https://godbolt.org/z/qoPaPozYK) ]
```c++
#include <stc/cstr.h>

typedef struct {
    cstr name;
    cstr country;
} Viking;

Viking Viking_make(cstr_raw name, cstr_raw country) {
    return (Viking){.name = cstr_from(name), .country = cstr_from(country)};
}

bool Viking_eq(const Viking* a, const Viking* b) {
    return cstr_eq(&a->name, &b->name) && cstr_eq(&a->country, &b->country);
}

size_t Viking_hash(const Viking* a) {
    return cstr_hash(&a->name) ^ cstr_hash(&a->country);
}

Viking Viking_clone(Viking v) {
    v.name = cstr_clone(v.name);
    v.country = cstr_clone(v.country);
    return v;
}

void Viking_drop(Viking* vp) {
    cstr_drop(&vp->name);
    cstr_drop(&vp->country);
}

#define i_type Vikings
#define i_keyclass Viking // binds the four Viking_xxxx() functions above
#define i_val int
#include <stc/hmap.h>

int main(void)
{
    // Use a HashMap to store the vikings' health points.
    Vikings vikings = c_make(Vikings, {
        {Viking_make("Einar", "Norway"), 25},
        {Viking_make("Olaf", "Denmark"), 24},
        {Viking_make("Harald", "Iceland"), 12},
    });

    Viking lookup = Viking_make("Olaf", "Denmark");
    printf("Lookup: Olaf of Denmark has %d hp\n\n", *Vikings_at(&vikings, lookup));
    Viking_drop(&lookup);

    // Print the status of the vikings.
    for (c_each_kv(viking, health, Vikings, vikings)) {
        printf("%s of %s has %d hp\n", cstr_str(&viking->name),
                                       cstr_str(&viking->country), *health);
    }
    Vikings_drop(&vikings);
}
```

### Example 5: More advanced
In example 4 we needed to construct a lookup key which may allocate strings, and then had to free it after.
In this example we use keyraw feature to make it simpler to use and avoids the creation of a Viking object
entirely when doing lookup.

[ [Run this code](https://godbolt.org/z/559noMbsx) ]
<!--{%raw%}-->
```c++
#include "stc/cstr.h"

typedef struct Viking {
    cstr name;
    cstr country;
} Viking;

Viking Viking_make(cstr_raw name, cstr_raw country) {
    return (Viking){.name = cstr_from(name), .country = cstr_from(country)};
}

void Viking_drop(Viking* vk) {
    cstr_drop(&vk->name);
    cstr_drop(&vk->country);
}

Viking Viking_clone(Viking v) {
    v.name = cstr_clone(v.name);
    v.country = cstr_clone(v.country);
    return v;
}

// Define Viking_raw, a Viking lookup struct with eq, hash and convertion functions between them:
typedef struct {
    const char* name;
    const char* country;
} Viking_raw;

bool Viking_raw_eq(const Viking_raw* rx, const Viking_raw* ry) {
    return strcmp(rx->name, ry->name)==0 && strcmp(rx->country, ry->country)==0;
}

size_t Viking_raw_hash(const Viking_raw* rv) {
    return c_hash_mix(c_hash_str(rv->name), c_hash_str(rv->country));
}

Viking Viking_from(Viking_raw raw) { // note: parameter is by value
    Viking v = {cstr_from(raw.name), cstr_from(raw.country)}; return v;
}

Viking_raw Viking_toraw(const Viking* vp) {
    Viking_raw rv = {cstr_str(&vp->name), cstr_str(&vp->country)}; return rv;
}

// With this in place, we define the Viking => int hash map type:
#define i_type   Vikings
#define i_keypro Viking  // key "class": binds Viking_drop, Viking_clone, Viking_from, Viking_toraw
#define i_val    int     // mapped health
#include "stc/hmap.h"

int main(void)
{
    Vikings vikings = c_make(Vikings, {
        {{"Einar", "Norway"}, 25},
        {{"Olaf", "Denmark"}, 24},
        {{"Harald", "Iceland"}, 12},
    });

    // Now lookup is using Viking_raw, not Viking:
    printf("Lookup: Olaf of Denmark has %d hp\n\n", *Vikings_at(&vikings, (Viking_raw){"Olaf", "Denmark"}));

    for (c_each(v, Vikings, vikings)) {
        Vikings_raw r = Vikings_value_toraw(v.ref);
        printf("%s of %s has %d hp\n", r.first.name, r.first.country, r.second);
    }
    Vikings_drop(&vikings);
}
```
<!--{%endraw%}-->
