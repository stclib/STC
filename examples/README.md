Examples
========
This folder contains various examples and benchmarks.


Custom key example
------------------
This demonstrates how to customize **cmap** with a user-defined key-type. When your key type consists of several members, you will usually have the hash function calculate hash values for the individual members, and then somehow combine them into one hash value for the entire object. If your key-type stores dynamic memory (e.g. cstr_t, as we will use), it is highly recommended to define a "raw"-struct representaion for your dynamic data struct. In addition, you must define two functions:

1. A hash function; calculates the hash value given an object of the key-type.
2. A comparison function for equality;

First, the Viking struct with destructor function:
```C
#include <stdio.h>
#include <stc/cmap.h>
#include <stc/cstr.h>

typedef struct Viking {
    cstr_t name;
    cstr_t country;
} Viking;

void viking_del(Viking* vk) {
    cstr_del(&vk->name);
    cstr_del(&vk->country);
}
```
And the Viking raw struct with hash, equals, and convertion functions between Viking and VikingRaw structs:
```C
// Viking raw struct

typedef struct VikingRaw {
    const char* name;
    const char* country;
} VikingRaw;

uint32_t vikingraw_hash(const VikingRaw* vw, size_t ignore) {
    uint32_t hash = c_string_hash(vw->name) ^ c_string_hash(w->country);
    return hash;
}
static inline int vikingraw_equals(const VikingRaw* x, const VikingRaw* y) {
    return strcmp(x->name, y->name) == 0 && strcmp(x->country, y->country) == 0;
}

static inline VikingRaw viking_toRaw(Viking* vk) {
    VikingRaw vw = {vk->name.str, vk->country.str}; return vw;
}
static inline Viking viking_fromRaw(VikingRaw vw) { // note: parameter is by value
    Viking vk = {cstr_from(vw.name), cstr_from(vw.country)}; return vk;
}
```
With this in place, we use the full using_cmap() macro to define {Viking -> int} hash map type:
```C
using_cmap(vk, Viking, int, c_default_del, vikingraw_equals, vikingraw_hash,
               viking_del, VikingRaw, viking_toRaw, viking_fromRaw);
```
cmap_vk uses vikingraw_hash() for hash value calculations, and vikingraw_equals() for equality test. cmap_vk_del() will free all memory allocated for Viking keys and the hash table values. Finally, main which also demos the generic c_push_items() of multiple elements:
```C
int main() {
    cmap_vk vikings = cmap_INIT;
    c_push_items(&vikings, cmap_vk, {
        { {"Einar", "Norway"}, 20},
        { {"Olaf", "Denmark"}, 24},
        { {"Harald", "Iceland"}, 12},
    });

    VikingRaw look = {"Einar", "Norway"};

    cmap_vk_entry_t *e = cmap_vk_find(&vikings, look);
    e->second += 5; // add
    cmap_vk_emplace(&vikings, look, 0)->second += 5; // update again

    c_foreach (k, cmap_vk, vikings) {
        printf("%s of %s has %d hp\n", k.val->first.name.str, k.val->first.country.str, k.val->second);
    }
    cmap_vk_del(&vikings);
}
```
