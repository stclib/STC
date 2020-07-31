Examples
========
This folder contains various examples and benchmarks.


Custom key example
------------------
This demonstrates how to customize **cmap** with a user-defined key-type. When your key type consists of several members, you will usually have the hash function calculate hash values for the individual members, and then somehow combine them into one hash value for the entire object. If your key-type stores dynamic memory (e.g. cstr_t, as we will use), it is recommended to define a "view/raw"-struct of the your data first. In addition, you must define two functions:

1. A hash function; calculates the hash value given an object of the key-type.

2. A comparison function for equality; 
```
#include <stdio.h>
#include <stc/cmap.h>
#include <stc/cstr.h>

// Viking view struct

typedef struct VikingVw {
    const char* name;
    const char* country;
} VikingVw;

uint32_t vikingvw_hash(const VikingVw* vw, size_t ignore) {
    uint32_t hash = c_defaultHash(vw->name, strlen(vw->name))
                  ^ c_defaultHash(vw->country, strlen(vw->country));
    return hash;
}
int vikingvw_equals(const VikingVw* x, const VikingVw* y) {
    if (strcmp(x->name, y->name) != 0) return false;
    return strcmp(x->country, y->country) == 0;
}
```
And the Viking data struct:
```
typedef struct Viking {
    cstr_t name;
    cstr_t country;
} Viking;


void viking_destroy(Viking* vk) {
    cstr_destroy(&vk->name);
    cstr_destroy(&vk->country);
}

VikingVw viking_getVw(Viking* vk) {
    VikingVw vw = {vk->name.str, vk->country.str}; return vw;
}
Viking viking_fromVw(VikingVw vw) {
    Viking vk = {cstr_make(vw.name), cstr_make(vw.country)}; return vk;
}
```
With this in place, we use the full declare_cmap() macro to define {Viking -> int} hash map type:
```
declare_cmap(vk, Viking, int, c_default_destroy, vikingvw_equals, vikingvw_hash, 
                 viking_destroy, VikingVw, viking_getVw, viking_fromVw);
```
cmap_vk uses vikingvw_hash() for hash value calculations, and vikingvw_equals() for equality test. cmap_vk_destroy() will free all memory allocated for Viking keys and the hash table values.
Finally, main which also demos the generic c_push() of multiple elements:
```
int main() {
    cmap_vk vikings = cmap_init;
    c_push(&vikings, cmap_vk, c_items(
        {{"Einar", "Norway"}, 20},
        {{"Olaf", "Denmark"}, 24},
        {{"Harald", "Iceland"}, 12},
    ));
    VikingVw look = {"Einar", "Norway"};
    cmap_vk_entry_t *e = cmap_vk_find(&vikings, look);
    e->value += 5; // update 
    cmap_vk_insert(&vikings, look, 0)->value += 5; // again

    c_foreach (k, cmap_vk, vikings) {
        printf("%s of %s has %d hp\n", k.item->key.name.str, k.item->key.country.str, k.item->value);
    }
    cmap_vk_destroy(&vikings);
}
```
