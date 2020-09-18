/*
 * To be able to use cmap with a user-defined key-type, you need to define two things:
 *   1. A hash function; this must be a function that calculates the hash value given
 *      an object of the key-type.
 *   2. A comparison function for equality.

 * When your key type consists of several members, you will usually have the hash function
 * calculate hash values for the individual members, and then somehow combine them into one
 * hash value for the entire object.
 * In order to use Viking as a map key, it is smart to define a plain-old-data "view"
 * of the Viking struct, to simplfy insert and lookup in the map.
 */
#include <stdio.h>
#include <stc/cmap.h>
#include <stc/cstr.h>

// Viking data struct -----------------------

typedef struct Viking {
    cstr_t name;
    cstr_t country;
} Viking;


void viking_del(Viking* vk) {
    cstr_del(&vk->name);
    cstr_del(&vk->country);
}

// Viking view struct -----------------------

typedef struct VikingVw {
    const char* name;
    const char* country;
} VikingVw;

uint32_t vikingvw_hash(const VikingVw* vw, size_t ignore) {
    uint32_t hash = c_string_hash(vw->name) ^ c_string_hash(vw->country);
    return hash;
}
int vikingvw_equals(const VikingVw* x, const VikingVw* y) {
    if (strcmp(x->name, y->name) != 0) return false;
    return strcmp(x->country, y->country) == 0;
}

VikingVw viking_toVw(Viking* vk) {
    VikingVw vw = {vk->name.str, vk->country.str}; return vw;
}
Viking viking_fromVw(VikingVw vw) {
    Viking vk = {cstr(vw.name), cstr(vw.country)}; return vk;
}

// Using the full using_cmap() macro to define [Viking -> int] hash map type:
using_cmap(vk, Viking, int, c_default_del, vikingvw_equals, vikingvw_hash,
                 viking_del, VikingVw, viking_toVw, viking_fromVw);

// cmap_vk uses vikingvw_hash() for hash value calculations, and vikingvw_equals() for equality test.
// cmap_vk_del() will free all memory allocated for Viking keys and the hash table values.

// Main ----------------------------

int main()
{
    cmap_vk vikings = cmap_INIT;
    c_push_items(&vikings, cmap_vk, {
        {{"Einar", "Norway"}, 20},
        {{"Olaf", "Denmark"}, 24},
        {{"Harald", "Iceland"}, 12},
    });

    VikingVw einar = {"Einar", "Norway"};
    cmap_vk_entry_t *e = cmap_vk_find(&vikings, einar);
    e->second += 5; // update
    cmap_vk_emplace(&vikings, einar, 0).first->second += 5; // again

    c_foreach (k, cmap_vk, vikings) {
        printf("%s of %s has %d hp\n", k.get->first.name.str, k.get->first.country.str, k.get->second);
    }
    cmap_vk_del(&vikings);
}

