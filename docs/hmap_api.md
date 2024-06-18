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

```c
#define i_TYPE <ct>,<kt>,<vt> // shorthand to define i_type,i_key,i_val
#define i_type <t>            // container type name (default: hmap_{i_key})
#define i_key <t>             // key type: REQUIRED.
#define i_val <t>             // mapped value type: REQUIRED.
#define i_hash <f>            // hash func i_keyraw*: REQUIRED IF i_keyraw is non-pod type
#define i_eq <f>              // equality comparison two i_keyraw*: REQUIRED IF i_keyraw is a
                              // non-integral type. Three-way i_cmp may be specified instead.
#define i_keydrop <f>         // destroy key func - defaults to empty destruct
#define i_keyclone <f>        // REQUIRED IF i_keydrop defined
#define i_keyraw <t>          // convertion "raw" type - defaults to i_key
#define i_keyfrom <f>         // convertion func i_keyraw => i_key
#define i_keyto <f>           // convertion func i_key* => i_keyraw

#define i_valdrop <f>         // destroy value func - defaults to empty destruct
#define i_valclone <f>        // REQUIRED IF i_valdrop defined
#define i_valraw <t>          // convertion "raw" type - defaults to i_val
#define i_valfrom <f>         // convertion func i_valraw => i_val
#define i_valto <f>           // convertion func i_val* => i_valraw

#define i_tag <s>             // alternative typename: hmap_{i_tag}. i_tag defaults to i_key
#include "stc/hmap.h"
```
`X` should be replaced by the value of `i_tag` in all of the following documentation.

## Methods

```c
hmap_X                hmap_X_init(void);
hmap_X                hmap_X_with_capacity(intptr_t cap);
hmap_X                hmap_X_clone(hmap_x map);

void                  hmap_X_clear(hmap_X* self);
void                  hmap_X_copy(hmap_X* self, const hmap_X* other);
float                 hmap_X_max_load_factor(const hmap_X* self);                       // default: 0.85f
bool                  hmap_X_reserve(hmap_X* self, intptr_t size);
void                  hmap_X_shrink_to_fit(hmap_X* self);
void                  hmap_X_drop(hmap_X* self);                                        // destructor

bool                  hmap_X_is_empty(const hmap_X* self );
intptr_t              hmap_X_size(const hmap_X* self);
intptr_t              hmap_X_capacity(const hmap_X* self);                              // buckets * max_load_factor
intptr_t              hmap_X_bucket_count(const hmap_X* self);                          // num. of allocated buckets

const hmap_X_mapped*  hmap_X_at(const hmap_X* self, i_keyraw rkey);                     // rkey must be in map
hmap_X_mapped*        hmap_X_at_mut(hmap_X* self, i_keyraw rkey);                       // mutable at
const hmap_X_value*   hmap_X_get(const hmap_X* self, i_keyraw rkey);                    // const get
hmap_X_value*         hmap_X_get_mut(hmap_X* self, i_keyraw rkey);                      // mutable get
bool                  hmap_X_contains(const hmap_X* self, i_keyraw rkey);
hmap_X_iter           hmap_X_find(const hmap_X* self, i_keyraw rkey);                   // find element

hmap_X_result         hmap_X_insert(hmap_X* self, i_key key, i_val mapped);             // no change if key in map
hmap_X_result         hmap_X_insert_or_assign(hmap_X* self, i_key key, i_val mapped);   // always update mapped
hmap_X_result         hmap_X_push(hmap_X* self, hmap_X_value entry);                    // similar to insert

hmap_X_result         hmap_X_emplace(hmap_X* self, i_keyraw rkey, i_valraw rmapped);    // no change if rkey in map
hmap_X_result         hmap_X_emplace_or_assign(hmap_X* self, i_keyraw rkey, i_valraw rmapped); // always update mapped
hmap_X_result         hmap_X_emplace_key(hmap_X* self, i_keyraw rkey);                  // see example 1.

int                   hmap_X_erase(hmap_X* self, i_keyraw rkey);                        // return 0 or 1
hmap_X_iter           hmap_X_erase_at(hmap_X* self, hmap_X_iter it);                    // return iter after it
void                  hmap_X_erase_entry(hmap_X* self, hmap_X_value* entry);

hmap_X_iter           hmap_X_begin(const hmap_X* self);
hmap_X_iter           hmap_X_end(const hmap_X* self);
void                  hmap_X_next(hmap_X_iter* it);
hmap_X_iter           hmap_X_advance(hmap_X_iter it, hmap_X_ssize n);

hmap_X_value          hmap_X_value_clone(hmap_X_value val);
hmap_X_raw            hmap_X_value_toraw(hmap_X_value* pval);
```
Free helper functions:
```c
uint64_t              c_hash_n(const void *data, intptr_t n);               // generic hash function of n bytes
uint64_t              c_hash_str(const char *str);                          // string hash function, uses strlen()
uint64_t              c_hash_mix(uint64_t h1, uint64_t h2, ...);            // mix/combine computed hashes
uint64_t              c_next_pow2(intptr_t k);                              // get next power of 2 >= k

// hash/equal template default functions:
uint64_t              c_default_hash(const T *obj);                         // alias for c_hash_n(obj, sizeof *obj)
bool                  c_default_eq(const i_keyraw* a, const i_keyraw* b);   // *a == *b
bool                  c_memcmp_eq(const i_keyraw* a, const i_keyraw* b);    // !memcmp(a, b, sizeof *a)
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

```c
#define i_implement
#include "stc/cstr.h"

#define i_key_cstr
#define i_val_cstr
#include "stc/hmap.h"

int main(void)
{
    // Create an unordered_map of three strings (that map to strings)
    hmap_cstr umap = c_init(hmap_cstr, {
        {"RED", "#FF0000"},
        {"GREEN", "#00FF00"},
        {"BLUE", "#0000FF"}
    });

    // Iterate and print keys and values of unordered map
    c_foreach (n, hmap_cstr, umap) {
        printf("Key:[%s] Value:[%s]\n", cstr_str(&n.ref->first), cstr_str(&n.ref->second));
    }

    // Add two new entries to the unordered map
    hmap_cstr_emplace(&umap, "BLACK", "#000000");
    hmap_cstr_emplace(&umap, "WHITE", "#FFFFFF");

    // Insert only if "CYAN" is not in the map: create mapped value when needed only.
    hmap_cstr_result res = hmap_cstr_emplace_key(&umap, "CYAN");
    if (res.inserted)
        res.ref->second = cstr_lit("#00FFFF"); // must assign second if key was inserted.

    // Output values by key
    printf("The HEX of color RED is:[%s]\n", cstr_str(hmap_cstr_at(&umap, "RED")));
    printf("The HEX of color BLACK is:[%s]\n", cstr_str(hmap_cstr_at(&umap, "BLACK")));

    hmap_cstr_drop(&umap);
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
This example uses a hmap with cstr as mapped value.
```c
#define i_implement
#include "stc/cstr.h"
#define i_type IDMap
#define i_key int
#define i_val_cstr
#include "stc/hmap.h"

int main(void)
{
    uint32_t col = 0xcc7744ff;

    IDMap idnames = {0};

    c_foritems (i, IDMap_raw, { {100, "Red"}, {110, "Blue"} })
        IDMap_emplace(&idnames, i.ref->first, i.ref->second);

    // replace existing mapped value:
    IDMap_emplace_or_assign(&idnames, 110, "White");

    // insert a new constructed mapped string into map:
    IDMap_insert_or_assign(&idnames, 120, cstr_from_fmt("#%08x", col));

    // emplace/insert does nothing if key already exist:
    IDMap_emplace(&idnames, 100, "Green");

    c_foreach (i, IDMap, idnames)
        printf("%d: %s\n", i.ref->first, cstr_str(&i.ref->second));

    IDMap_drop(&idnames);
}
```
Output:
```c
100: Red
110: White
120: #cc7744ff
```

### Example 3
Demonstrate hmap with plain-old-data key type Vec3i and int as mapped type: hmap<Vec3i, int>.
```c
#include <stdio.h>
typedef struct { int x, y, z; } Vec3i;

#define i_TYPE hmap_vi, Vec3i, int
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

    c_forpair (v3, num, hmap_vi, vecs)
        printf("{ %3d, %3d, %3d }: %d\n", _.v3->x, _.v3->y, _.v3->z, *_.num);

    hmap_vi_drop(&vecs);
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
Inverse: demonstrate hmap with mapped POD type Vec3i: hmap<int, Vec3i>:
```c
#include <stdio.h>
typedef struct { int x, y, z; } Vec3i;

#define i_TYPE hmap_iv, int, Vec3i
#include "stc/hmap.h"

int main(void)
{
    hmap_iv vecs = {0}

    hmap_iv_insert(&vecs, 1, (Vec3i){100,   0,   0});
    hmap_iv_insert(&vecs, 2, (Vec3i){  0, 100,   0});
    hmap_iv_insert(&vecs, 3, (Vec3i){  0,   0, 100});
    hmap_iv_insert(&vecs, 4, (Vec3i){100, 100, 100});

    c_forpair (num, v3, hmap_iv, vecs)
        printf("%d: { %3d, %3d, %3d }\n", *_.num, _.v3->x, _.v3->y, _.v3->z);

    hmap_iv_drop(&vecs);
}
```
Output:
```c
4: { 100, 100, 100 }
3: {   0,   0, 100 }
2: {   0, 100,   0 }
1: { 100,   0,   0 }
```

### Example 5: Advanced
Key type is struct.
```c
#define i_implement
#include "stc/cstr.h"

typedef struct {
    cstr name;
    cstr country;
} Viking;

#define Viking_init() ((Viking){.name={0}, .country={0}})

static inline int Viking_cmp(const Viking* a, const Viking* b) {
    int c = cstr_cmp(&a->name, &b->name);
    return c ? c : cstr_cmp(&a->country, &b->country);
}

static inline uint32_t Viking_hash(const Viking* a) {
    return cstr_hash(&a->name) ^ cstr_hash(&a->country);
}

static inline Viking Viking_clone(Viking v) {
    v.name = cstr_clone(v.name);
    v.country = cstr_clone(v.country);
    return v;
}

static inline void Viking_drop(Viking* vk) {
    cstr_drop(&vk->name);
    cstr_drop(&vk->country);
}

#define i_type Vikings
#define i_keyclass Viking
#define i_val int
#include "stc/hmap.h"

int main(void)
{
    // Use a HashMap to store the vikings' health points.
    Vikings vikings = {0};

    Vikings_insert(&vikings, (Viking){cstr_lit("Einar"), cstr_lit("Norway")}, 25);
    Vikings_insert(&vikings, (Viking){cstr_lit("Olaf"), cstr_lit("Denmark")}, 24);
    Vikings_insert(&vikings, (Viking){cstr_lit("Harald"), cstr_lit("Iceland")}, 12);
    Vikings_insert(&vikings, (Viking){cstr_lit("Einar"), cstr_lit("Denmark")}, 21);

    Viking lookup = (Viking){cstr_lit("Einar"), cstr_lit("Norway")};
    printf("Lookup: Einar of Norway has %d hp\n\n", *Vikings_at(&vikings, lookup));
    Viking_drop(&lookup);

    // Print the status of the vikings.
    c_forpair (vik, hp, Vikings, vikings) {
        printf("%s of %s has %d hp\n", cstr_str(&_.vik->name), cstr_str(&_.vik->country), *_.hp);
    }
    Vikings_drop(&vikings);
}
```
Output:
```
Olaf of Denmark has 24 hp
Einar of Denmark has 21 hp
Einar of Norway has 25 hp
Harald of Iceland has 12 hp
```

### Example 6: More advanced
In example 5 we needed to construct a lookup key which allocated strings, and then had to free it after.
In this example we use rawtype feature to make it even simpler to use. Note that we must use the emplace() methods
to add "raw" type entries (otherwise compile error).
```c
#define i_implement
#include "stc/cstr.h"

typedef struct Viking {
    cstr name;
    cstr country;
} Viking;

void Viking_drop(Viking* vk) {
    cstr_drop(&vk->name);
    cstr_drop(&vk->country);
}

Viking Viking_clone(Viking vk) {
    vk.name = cstr_clone(vk.name);
    vk.country = cstr_clone(vk.country);
    return vk;
}

// Define Viking lookup struct with hash, cmp, and convertion functions between Viking and RViking structs:

typedef struct RViking {
    const char* name;
    const char* country;
} RViking;

static inline int RViking_cmp(const RViking* rx, const RViking* ry) {
    int c = strcmp(rx->name, ry->name);
    return c ? c : strcmp(rx->country, ry->country);
}

static inline Viking Viking_from(RViking raw) { // note: parameter is by value
    Viking v = {cstr_from(raw.name), cstr_from(raw.country)}; return v;
}

static inline RViking Viking_toraw(const Viking* vp) {
    RViking rv = {cstr_str(&vp->name), cstr_str(&vp->country)}; return rv;
}

// With this in place, we define the Viking => int hash map type:
#define i_type      Vikings
#define i_rawclass RViking     // lookup type ; binds _cmp, _hash (unless overridden)
#define i_keyclass Viking      // key type    ; binds _drop, _clone, _from, _toraw
#define i_val       int         // mapped type
#define i_hash(rp)  c_hash_mix(c_hash_str(rp->name), c_hash_str(rp->country))
#include "stc/hmap.h"

int main(void)
{
    Vikings vikings = {0};
    Vikings_emplace(&vikings, (RViking){"Einar", "Norway"}, 20);
    Vikings_emplace(&vikings, (RViking){"Olaf", "Denmark"}, 24);
    Vikings_emplace(&vikings, (RViking){"Harald", "Iceland"}, 12);
    Vikings_emplace(&vikings, (RViking){"Björn", "Sweden"}, 10);

    Vikings_value* v = Vikings_get_mut(&vikings, (RViking){"Einar", "Norway"});
    v->second += 3; // add 3 hp points to Einar

    c_forpair (vk, hp, Vikings, vikings) {
        printf("%s of %s has %d hp\n", cstr_str(&_.vk->name), cstr_str(&_.vk->country), *_.hp);
    }
    Vikings_drop(&vikings);
}
```
