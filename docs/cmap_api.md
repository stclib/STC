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
#define i_key       // hash key: REQUIRED
#define i_val       // map value: REQUIRED
#define i_hash      // hash func i_keyraw*: REQUIRED IF i_keyraw is non-pod type
#define i_eq        // equality comparison two i_keyraw*: REQUIRED IF i_keyraw is a
                    // non-integral type. Three-way i_cmp may be specified alternatively.
#define i_keydrop   // destroy key func - defaults to empty destruct
#define i_keyraw    // convertion "raw" type - defaults to i_key
#define i_keyfrom   // convertion func i_keyraw => i_key - defaults to plain copy
#define i_keyto     // convertion func i_key* => i_keyraw - defaults to plain copy

#define i_valdrop   // destroy value func - defaults to empty destruct
#define i_valraw    // convertion "raw" type - defaults to i_val
#define i_valfrom   // convertion func i_valraw => i_val - defaults to plain copy
#define i_valto     // convertion func i_val* => i_valraw - defaults to plain copy

#define i_tag       // typename tag. defaults to i_key
#define i_type      // full typename of the container
#include <stc/cmap.h>
```
`X` should be replaced by the value of `i_tag` in all of the following documentation.

## Methods

```c
cmap_X                cmap_X_init(void);
cmap_X                cmap_X_with_capacity(size_t cap);
cmap_X                cmap_X_clone(cmap_x map);

void                  cmap_X_clear(cmap_X* self);
void                  cmap_X_copy(cmap_X* self, cmap_X other);
void                  cmap_X_max_load_factor(cmap_X* self, float max_load);                     // default: 0.85
bool                  cmap_X_reserve(cmap_X* self, size_t size);
void                  cmap_X_shrink_to_fit(cmap_X* self);
void                  cmap_X_swap(cmap_X* a, cmap_X* b);
void                  cmap_X_drop(cmap_X* self);                                                // destructor

size_t                cmap_X_size(cmap_X map);
size_t                cmap_X_capacity(cmap_X map);                                              // buckets * max_load_factor
bool                  cmap_X_empty(cmap_X map);
size_t                cmap_X_bucket_count(cmap_X map);                                          // num. of allocated buckets

const cmap_X_mapped*  cmap_X_at(const cmap_X* self, i_keyraw rkey);                             // rkey must be in map
cmap_X_mapped*        cmap_X_at_mut(cmap_X* self, i_keyraw rkey);                               // mutable at
const cmap_X_value*   cmap_X_get(const cmap_X* self, i_keyraw rkey);                            // const get
cmap_X_value*         cmap_X_get_mut(cmap_X* self, i_keyraw rkey);                              // mutable get
bool                  cmap_X_contains(const cmap_X* self, i_keyraw rkey);
cmap_X_iter           cmap_X_find(const cmap_X* self, i_keyraw rkey);                           // find element

cmap_X_result         cmap_X_insert(cmap_X* self, i_key key, i_val mapped);                     // no change if key in map
cmap_X_result         cmap_X_insert_or_assign(cmap_X* self, i_key key, i_val mapped);           // always update mapped
cmap_X_result         cmap_X_put(cmap_X* self, i_key key, i_val mapped);                        // alias for insert_or_assign

cmap_X_result         cmap_X_emplace(cmap_X* self, i_keyraw rkey, i_valraw rmapped);            // no change if rkey in map
cmap_X_result         cmap_X_emplace_or_assign(cmap_X* self, i_keyraw rkey, i_valraw rmapped);  // always update rmapped
cmap_X_result         cmap_X_put_raw(cmap_X* self, i_keyraw rkey, i_valraw rmapped);            // alias for emplace_or_assign

size_t                cmap_X_erase(cmap_X* self, i_keyraw rkey);                                // return 0 or 1
cmap_X_iter           cmap_X_erase_at(cmap_X* self, cmap_X_iter it);                            // return iter after it
void                  cmap_X_erase_entry(cmap_X* self, cmap_X_value* entry);

cmap_X_iter           cmap_X_begin(const cmap_X* self);
cmap_X_iter           cmap_X_end(const cmap_X* self);
void                  cmap_X_next(cmap_X_iter* it);

cmap_X_value          cmap_X_value_clone(cmap_X_value val);
cmap_X_raw            cmap_X_value_toraw(cmap_X_value* pval);
```
Helpers:
```c
uint64_t              c_strhash(const char *str);                              // utility function

// hash template parameter functions:
uint64_t              c_default_hash(const void *data, size_t len);            // key is any integral type
uint64_t              c_hash32(const void* data, size_t is4);                  // key is one 32-bit int
uint64_t              c_hash64(const void* data, size_t is8);                  // key is one 64-bit int

// equalto template parameter functions:
bool                  c_default_eq(const i_keyraw* a, const i_keyraw* b);      // *a == *b
bool                  c_memcmp_eq(const i_keyraw* a, const i_keyraw* b);       // !memcmp(a, b, sizeof *a)
```

## Types

| Type name          | Type definition                                 | Used to represent...          |
|:-------------------|:------------------------------------------------|:------------------------------|
| `cmap_X`           | `struct { ... }`                                | The cmap type                 |
| `cmap_X_rawkey`    | `i_keyraw`                                      | The raw key type              |
| `cmap_X_rawmapped` | `i_valraw`                                      | The raw mapped type           |
| `cmap_X_raw`       | `struct { i_keyraw first; i_valraw second; }`   | i_keyraw + i_valraw type      |
| `cmap_X_key`       | `i_key`                                         | The key type                  |
| `cmap_X_mapped`    | `i_val`                                         | The mapped type               |
| `cmap_X_value`     | `struct { const i_key first; i_val second; }`   | The value: key is immutable   |
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
    c_auto (cmap_str, u)
    {
        c_apply(v, cmap_str_emplace(&u, c_pair(v)), cmap_str_raw, {
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
    }
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

#define i_key int
#define i_val_str
#define i_tag id
#include <stc/cmap.h>

int main()
{
    uint32_t col = 0xcc7744ff;

    c_auto (cmap_id, idnames)
    {
        c_apply(v, cmap_id_emplace(&idnames, c_pair(v)), cmap_id_raw, {
            {100, "Red"}, {110, "Blue"}
        });
        // replace existing mapped value:
        cmap_id_emplace_or_assign(&idnames, 110, "White");
        
        // insert a new constructed mapped string into map:
        cmap_id_insert_or_assign(&idnames, 120, cstr_from_fmt("#%08x", col));
        
        // emplace/insert does nothing if key already exist:
        cmap_id_emplace(&idnames, 100, "Green");

        c_foreach (i, cmap_id, idnames)
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
    c_autovar (cmap_vi vecs = cmap_vi_init(), cmap_vi_drop(&vecs))
    {
        cmap_vi_insert(&vecs, (Vec3i){100,   0,   0}, 1);
        cmap_vi_insert(&vecs, (Vec3i){  0, 100,   0}, 2);
        cmap_vi_insert(&vecs, (Vec3i){  0,   0, 100}, 3);
        cmap_vi_insert(&vecs, (Vec3i){100, 100, 100}, 4);

        c_forpair (vec, num, cmap_vi, vecs)
            printf("{ %3d, %3d, %3d }: %d\n", _.vec.x,  _.vec.y,  _.vec.z,  _.num);
    }
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
    c_auto (cmap_iv, vecs) // shorthand for c_autovar with _init(), _drop().
    {
        cmap_iv_insert(&vecs, 1, (Vec3i){100,   0,   0});
        cmap_iv_insert(&vecs, 2, (Vec3i){  0, 100,   0});
        cmap_iv_insert(&vecs, 3, (Vec3i){  0,   0, 100});
        cmap_iv_insert(&vecs, 4, (Vec3i){100, 100, 100});

        c_forpair (num, vec, cmap_iv, vecs)
            printf("%d: { %3d, %3d, %3d }\n", _.num, _.vec.x,  _.vec.y,  _.vec.z);
    }
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
Advanced 1: Key type is struct.
```c
#include <stc/cstr.h>

typedef struct {
    cstr name;
    cstr country;
} Viking;

#define Viking_init() ((Viking){cstr_null, cstr_null})

static inline bool Viking_eq(const Viking* a, const Viking* b) {
    return cstr_equals_s(a->name, b->name) && cstr_equals_s(a->country, b->country);
}

static inline uint32_t Viking_hash(const Viking* a, int ignored) {
    return c_strhash(a->name.str) ^ (c_strhash(a->country.str) >> 15);
}

static inline Viking Viking_clone(Viking v) {
    v.name = cstr_clone(v.name); 
    v.country = cstr_clone(v.country);
}

static inline void Viking_drop(Viking* vk) {
    cstr_drop(&vk->name);
    cstr_drop(&vk->country);
}

#define i_type Vikings
#define i_key_bind Viking
#define i_val int
// i_key_bind auto-binds:
//  #define i_eq Viking_eq
//  #define i_hash Viking_hash
//  #define i_keyfrom Viking_clone
//  #define i_keydrop Viking_drop
#include <stc/cmap.h>

int main()
{
    // Use a HashMap to store the vikings' health points.
    c_auto (Vikings, vikings) // uses Vikings_init(), Vikings_drop()
    {
        Vikings_insert(&vikings, (Viking){cstr_new("Einar"), cstr_new("Norway")}, 25);
        Vikings_insert(&vikings, (Viking){cstr_new("Olaf"), cstr_new("Denmark")}, 24);
        Vikings_insert(&vikings, (Viking){cstr_new("Harald"), cstr_new("Iceland")}, 12);
        Vikings_insert(&vikings, (Viking){cstr_new("Einar"), cstr_new("Denmark")}, 21);
        
        c_auto (Viking, lookup) {
            lookup = (Viking){cstr_new("Einar"), cstr_new("Norway")};
            printf("Lookup: Einar of Norway has %d hp\n\n", *Vikings_at(&vikings, lookup));
        }

        // Print the status of the vikings.
        c_forpair (viking, hp, Vikings, vikings) {
            printf("%s of %s has %d hp\n", _.viking.name.str, _.viking.country.str, _.hp);
        }
    }
}
```
Output:
```
Olaf of Denmark has 24 hp
Einar of Denmark has 21 hp
Einar of Norway has 25 hp
Harald of Iceland has 12 hp
```

### Example 6
Advanced 2: In example 5 we needed to construct a lookup key which allocated strings, and then had to free it after.
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

// Define Viking raw struct with hash, equalto, and convertion functions between Viking and RViking structs:

typedef struct RViking {
    const char* name;
    const char* country;
} RViking;

static inline uint64_t RViking_hash(const RViking* raw, size_t ignore) {
    uint64_t hash = c_strhash(raw->name) ^ (c_strhash(raw->country) >> 15);
    return hash;
}
static inline bool RViking_eq(const RViking* rx, const RViking* ry) {
    return strcmp(rx->name, ry->name) == 0 && strcmp(rx->country, ry->country) == 0;
}

static inline Viking Viking_from(RViking raw) {
    return (Viking){cstr_from(raw.name), cstr_from(raw.country)};
}
static inline RViking Viking_toraw(const Viking* vk) {
    return (RViking){vk->name.str, vk->country.str};
}

// With this in place, we define the Viking => int hash map type:
#define i_type      Vikings
#define i_key_bind  Viking
#define i_val       int
#define i_keyraw    RViking
// i_key_bind macro will make these functions auto-bind:
//  #define i_hash     RViking_hash
//  #define i_eq       RViking_eq
//  #define i_keyfrom  Viking_from // uses _from because i_keyraw is defined
//  #define i_keyto    Viking_toraw
//  #define i_keydrop  Viking_drop
#include <stc/cmap.h>

int main()
{
    c_auto (Vikings, vikings) {
        c_apply(v, Vikings_emplace(&vikings, c_pair(v)), Vikings_raw, {
            { {"Einar", "Norway"}, 20 },
            { {"Olaf", "Denmark"}, 24 },
            { {"Harald", "Iceland"}, 12 },
        });
        Vikings_emplace_or_assign(&vikings, (RViking){"Bjorn", "Sweden"}, 10);

        Vikings_value *v = Vikings_get_mut(&vikings, (RViking){"Einar", "Norway"});
        if (v) v->second += 3; // add 3 hp points to Einar

        c_forpair (viking, health, Vikings, vikings) {
            printf("%s of %s has %d hp\n", _.viking.name.str, _.viking.country.str, _.health);
        }
    }
}
```
