# STC [cmap](../include/stc/cmap.h): Unordered Map
![Map](pics/map.jpg)

A **cmap** is an associative container that contains key-value pairs with unique keys. Search, insertion, and removal of elements
have average constant-time complexity. Internally, the elements are not sorted in any particular order, but organized into
buckets. Which bucket an element is placed into depends entirely on the hash of its key. This allows fast access to individual
elements, since once the hash is computed, it refers to the exact bucket the element is placed into. It is implemented as closed
hashing (aka open addressing) with linear probing, and without leaving tombstones on erase.

***Iterator invalidation***: References and iterators are invalidated after erase. No iterators are invalidated after insert,
unless the hash-table need to be extended. The hash table size can be reserved prior to inserts if the total max size is known.
The order of elements is preserved after erase and insert. This makes it possible to erase individual elements while iterating
through the container by using the returned iterator from *erase_at()*, which references the next element.

See the c++ class [std::unordered_map](https://en.cppreference.com/w/cpp/container/unordered_map) for a functional description.

## Header file and declaration

```c
#define i_type      // container type name (default: cmap_{i_key})
#define i_key       // hash key: REQUIRED
#define i_val       // map value: REQUIRED
#define i_hash      // hash func i_keyraw*: REQUIRED IF i_keyraw is non-pod type
#define i_eq        // equality comparison two i_keyraw*: REQUIRED IF i_keyraw is a
                    // non-integral type. Three-way i_cmp may alternatively be specified.
#define i_keydrop   // destroy key func - defaults to empty destruct
#define i_keyclone  // REQUIRED IF i_keydrop defined
#define i_keyraw    // convertion "raw" type - defaults to i_key
#define i_keyfrom   // convertion func i_keyraw => i_key
#define i_keyto     // convertion func i_key* => i_keyraw

#define i_valdrop   // destroy value func - defaults to empty destruct
#define i_valclone  // REQUIRED IF i_valdrop defined
#define i_valraw    // convertion "raw" type - defaults to i_val
#define i_valfrom   // convertion func i_valraw => i_val
#define i_valto     // convertion func i_val* => i_valraw

#define i_tag       // alternative typename: cmap_{i_tag}. i_tag defaults to i_val
#define i_ssize     // internal; default int32_t. If defined, table expand 2x (else 1.5x)
#include <stc/cmap.h>
```
`X` should be replaced by the value of `i_tag` in all of the following documentation.

## Methods

```c
cmap_X                cmap_X_init(void);
cmap_X                cmap_X_with_capacity(intptr_t cap);
cmap_X                cmap_X_clone(cmap_x map);

void                  cmap_X_clear(cmap_X* self);
void                  cmap_X_copy(cmap_X* self, const cmap_X* other);
float                 cmap_X_max_load_factor(const cmap_X* self);                       // default: 0.85f
bool                  cmap_X_reserve(cmap_X* self, intptr_t size);
void                  cmap_X_shrink_to_fit(cmap_X* self);
void                  cmap_X_drop(cmap_X* self);                                        // destructor

bool                  cmap_X_empty(const cmap_X* self );
intptr_t              cmap_X_size(const cmap_X* self);
intptr_t              cmap_X_capacity(const cmap_X* self);                              // buckets * max_load_factor
intptr_t              cmap_X_bucket_count(const cmap_X* self);                          // num. of allocated buckets

const cmap_X_mapped*  cmap_X_at(const cmap_X* self, i_keyraw rkey);                     // rkey must be in map
cmap_X_mapped*        cmap_X_at_mut(cmap_X* self, i_keyraw rkey);                       // mutable at
const cmap_X_value*   cmap_X_get(const cmap_X* self, i_keyraw rkey);                    // const get
cmap_X_value*         cmap_X_get_mut(cmap_X* self, i_keyraw rkey);                      // mutable get
bool                  cmap_X_contains(const cmap_X* self, i_keyraw rkey);
cmap_X_iter           cmap_X_find(const cmap_X* self, i_keyraw rkey);                   // find element

cmap_X_result         cmap_X_insert(cmap_X* self, i_key key, i_val mapped);             // no change if key in map
cmap_X_result         cmap_X_insert_or_assign(cmap_X* self, i_key key, i_val mapped);   // always update mapped
cmap_X_result         cmap_X_push(cmap_X* self, cmap_X_value entry);                    // similar to insert

cmap_X_result         cmap_X_emplace(cmap_X* self, i_keyraw rkey, i_valraw rmapped);    // no change if rkey in map
cmap_X_result         cmap_X_emplace_or_assign(cmap_X* self, i_keyraw rkey, i_valraw rmapped); // always update

int                   cmap_X_erase(cmap_X* self, i_keyraw rkey);                        // return 0 or 1
cmap_X_iter           cmap_X_erase_at(cmap_X* self, cmap_X_iter it);                    // return iter after it
void                  cmap_X_erase_entry(cmap_X* self, cmap_X_value* entry);

cmap_X_iter           cmap_X_begin(const cmap_X* self);
cmap_X_iter           cmap_X_end(const cmap_X* self);
void                  cmap_X_next(cmap_X_iter* it);
cmap_X_iter           cmap_X_advance(cmap_X_iter it, cmap_X_ssize n);

cmap_X_value          cmap_X_value_clone(cmap_X_value val);
cmap_X_raw            cmap_X_value_toraw(cmap_X_value* pval);
```
Helpers:
```c
uint64_t              c_default_hash(const X *obj);                         // macro, calls cfasthash(obj, sizeof *obj)
uint64_t              cstrhash(const char *str);                            // string hash funcion, uses strlen()
uint64_t              cfasthash(const void *data, intptr_t len);            // base hash function

// equalto template parameter functions:
bool                  c_default_eq(const i_keyraw* a, const i_keyraw* b);   // *a == *b
bool                  c_memcmp_eq(const i_keyraw* a, const i_keyraw* b);    // !memcmp(a, b, sizeof *a)
```

## Types

| Type name          | Type definition                                 | Used to represent...          |
|:-------------------|:------------------------------------------------|:------------------------------|
| `cmap_X`           | `struct { ... }`                                | The cmap type                 |
| `cmap_X_key`       | `i_key`                                         | The key type                  |
| `cmap_X_mapped`    | `i_val`                                         | The mapped type               |
| `cmap_X_value`     | `struct { const i_key first; i_val second; }`   | The value: key is immutable   |
| `cmap_X_keyraw`    | `i_keyraw`                                      | The raw key type              |
| `cmap_X_rmapped`   | `i_valraw`                                      | The raw mapped type           |
| `cmap_X_raw`       | `struct { i_keyraw first; i_valraw second; }`   | i_keyraw + i_valraw type      |
| `cmap_X_result`    | `struct { cmap_X_value *ref; bool inserted; }`  | Result of insert/emplace      |
| `cmap_X_iter`      | `struct { cmap_X_value *ref; ... }`             | Iterator type                 |

## Examples

```c
#include <stc/cstr.h>

#define i_key_str
#define i_val_str
#include <stc/cmap.h>

int main()
{
    // Create an unordered_map of three strings (that map to strings)
    cmap_str umap = c_make(cmap_str, {
        {"RED", "#FF0000"},
        {"GREEN", "#00FF00"},
        {"BLUE", "#0000FF"}
    });

    // Iterate and print keys and values of unordered map
    c_foreach (n, cmap_str, umap) {
        printf("Key:[%s] Value:[%s]\n", cstr_str(&n.ref->first), cstr_str(&n.ref->second));
    }

    // Add two new entries to the unordered map
    cmap_str_emplace(&umap, "BLACK", "#000000");
    cmap_str_emplace(&umap, "WHITE", "#FFFFFF");

    // Output values by key
    printf("The HEX of color RED is:[%s]\n", cstr_str(cmap_str_at(&umap, "RED")));
    printf("The HEX of color BLACK is:[%s]\n", cstr_str(cmap_str_at(&umap, "BLACK")));

    cmap_str_drop(&umap);
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
This example uses a cmap with cstr as mapped value.
```c
#include <stc/cstr.h>
#define i_type IDMap
#define i_key int
#define i_val_str
#include <stc/cmap.h>

int main()
{
    uint32_t col = 0xcc7744ff;

    IDMap idnames = {0};

    c_forlist (i, IDMap_raw, { {100, "Red"}, {110, "Blue"} })
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
Demonstrate cmap with plain-old-data key type Vec3i and int as mapped type: cmap<Vec3i, int>.
```c
#include <stdio.h>
typedef struct { int x, y, z; } Vec3i;

#define i_key Vec3i
#define i_val int
#define i_eq c_memcmp_eq // bitwise equal, and use c_default_hash
#define i_tag vi
#include <stc/cmap.h>

int main()
{
    // Define map with defered destruct
    cmap_vi vecs = {0};

    cmap_vi_insert(&vecs, (Vec3i){100,   0,   0}, 1);
    cmap_vi_insert(&vecs, (Vec3i){  0, 100,   0}, 2);
    cmap_vi_insert(&vecs, (Vec3i){  0,   0, 100}, 3);
    cmap_vi_insert(&vecs, (Vec3i){100, 100, 100}, 4);

    c_forpair (v3, num, cmap_vi, vecs)
        printf("{ %3d, %3d, %3d }: %d\n", _.v3->x, _.v3->y, _.v3->z, *_.num);

    cmap_vi_drop(&vecs);
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
#include <stdio.h>
typedef struct { int x, y, z; } Vec3i;

#define i_key int
#define i_val Vec3i
#define i_tag iv
#include <stc/cmap.h>

int main()
{
    cmap_iv vecs = {0}

    cmap_iv_insert(&vecs, 1, (Vec3i){100,   0,   0});
    cmap_iv_insert(&vecs, 2, (Vec3i){  0, 100,   0});
    cmap_iv_insert(&vecs, 3, (Vec3i){  0,   0, 100});
    cmap_iv_insert(&vecs, 4, (Vec3i){100, 100, 100});

    c_forpair (num, v3, cmap_iv, vecs)
        printf("%d: { %3d, %3d, %3d }\n", *_.num, _.v3->x, _.v3->y, _.v3->z);

    cmap_iv_drop(&vecs);
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
#include <stc/cstr.h>

typedef struct {
    cstr name;
    cstr country;
} Viking;

#define Viking_init() ((Viking){cstr_NULL, cstr_NULL})

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
#include <stc/cmap.h>

int main()
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
to add "raw" type entries (otherwise compile error):
```c
#include <stc/cstr.h>

typedef struct Viking {
    cstr name;
    cstr country;
} Viking;

static inline void Viking_drop(Viking* v) {
    c_drop(cstr, &v->name, &v->country);
}

// Define Viking raw struct with cmp, hash, and convertion functions between Viking and RViking structs:

typedef struct RViking {
    const char* name;
    const char* country;
} RViking;

static inline int RViking_cmp(const RViking* rx, const RViking* ry) {
    int c = strcmp(rx->name, ry->name);
    return c ? c : strcmp(rx->country, ry->country);
}

static inline Viking Viking_from(RViking raw) {
    return (Viking){cstr_from(raw.name), cstr_from(raw.country)};
}

static inline RViking Viking_toraw(const Viking* vp) {
    return (RViking){cstr_str(&vp->name), cstr_str(&vp->country)};
}

// With this in place, we define the Viking => int hash map type:
#define i_type      Vikings
#define i_keyclass  Viking
#define i_keyraw    RViking
#define i_keyfrom   Viking_from
#define i_opt       c_no_clone // disable map cloning
#define i_hash(rp)  (cstrhash(rp->name) ^ cstrhash(rp->country))
#define i_val       int
#include <stc/cmap.h>

int main()
{
    Vikings vikings = {0};

    Vikings_emplace(&vikings, (RViking){"Einar", "Norway"}, 20);
    Vikings_emplace(&vikings, (RViking){"Olaf", "Denmark"}, 24);
    Vikings_emplace(&vikings, (RViking){"Harald", "Iceland"}, 12);
    Vikings_emplace(&vikings, (RViking){"Björn", "Sweden"}, 10);

    Vikings_value *v = Vikings_get_mut(&vikings, (RViking){"Einar", "Norway"});
    if (v) v->second += 3; // add 3 hp points to Einar

    c_forpair (vk, hp, Vikings, vikings) {
        printf("%s of %s has %d hp\n", cstr_str(&_.vk->name), cstr_str(&_.vk->country), *_.hp);
    }
    Vikings_drop(&vikings);
}
```
