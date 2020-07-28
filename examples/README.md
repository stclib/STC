Examples
========
Contains various examples and benchmarks.


advanced.c Example
------------------

This demonstrates how to customize hash **cmap** with a user-defined key-type. You need to define two things:

1. A hash function; calculates the hash value given an object of the key-type.

2. A comparison function for equality; 

When your key type consists of several members, you will usually have the hash function calculate hash values for the individual members, and then somehow combine them into one hash value for the entire object.
If your key-type stores dynamic memory (e.g. CStr as we will use), it is smart to define a plain-old-data "view" of the your key struct first:
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
    cstr name;
    cstr country;
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
CMap_vk uses vikingvw_hash() for hash value calculations, and vikingvw_equals() for equality test. cmap_vk_destroy() will free all memory allocated for Viking keys and the hash table values.
Finally, the demo:
```
int main()
{
    cmap_vk vikings = cmap_init;
    // emplace constructs the keys
    cmap_vk_put(&vikings, (VikingVw) {"Einar", "Norway"}, 20);
    cmap_vk_put(&vikings, (VikingVw) {"Olaf", "Denmark"}, 24);
    cmap_vk_put(&vikings, (VikingVw) {"Harald", "Iceland"}, 12);

    cmapentry_vk* e = cmap_vk_get(&vikings, (VikingVw) {"Einar", "Norway"});
    e->value += 5; // update 

    c_foreach (k, cmap_vk, vikings) {
        printf("%s of %s has %d hp\n", k.item->key.name.str, k.item->key.country.str, k.item->value);
    }
    cmap_vk_destroy(&vikings);
}
```
