/*
 * To be able to use CHash with a user-defined key-type, you need to define two things:
 *   1. A hash function; this must be a function that calculates the hash value given
 *      an object of the key-type.
 *   2. A comparison function for equality.

 * When your key type consists of several members, you will usually have the hash function
 * calculate hash values for the individual members, and then somehow combine them into one 
 * hash value for the entire object.
 * In order to use Viking as a map key, it is smart to define a plain-old-data "view"
 * of the Viking struct first.
 */
#include <stdio.h>
#include <stc/chash.h>
#include <stc/cstring.h>

// Viking view struct -----------------------

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

// Viking data struct -----------------------

typedef struct Viking {
    CString name;
    CString country;
} Viking;


void viking_destroy(Viking* vk) {
    cstring_destroy(&vk->name);
    cstring_destroy(&vk->country);
}

VikingVw viking_getVw(Viking* vk) {
    VikingVw vw = {vk->name.str, vk->country.str}; return vw;
}
Viking viking_fromVw(VikingVw vw) {
    Viking vk = {cstring_make(vw.name), cstring_make(vw.country)}; return vk;
}


// Using the full declare_CHash() macro to define [Viking -> int] hash map type:
declare_CHash(vk, MAP, Viking, int, c_emptyDestroy, vikingvw_hash, vikingvw_equals, 
                  viking_destroy, VikingVw, viking_getVw, viking_fromVw);

// CHash_vk uses vikingvw_hash() for hash value calculations, and vikingvw_equals() for equality test.
// chash_vk_destroy() will free all memory allocated for Viking keys and the hash table values.

// Main ----------------------------

int main()
{
    CHash_vk vikings = chash_init;
     // emplace constructs the keys
    chash_vk_put(&vikings, (VikingVw) {"Einar", "Norway"}, 20);
    chash_vk_put(&vikings, (VikingVw) {"Olaf", "Denmark"}, 24);
    chash_vk_put(&vikings, (VikingVw) {"Harald", "Iceland"}, 12);

    CHashEntry_vk* e = chash_vk_get(&vikings, (VikingVw) {"Einar", "Norway"});
    e->value += 5; // update 

    c_foreach (k, chash_vk, vikings) {
        printf("%s of %s has %d hp\n", k.item->key.name.str, k.item->key.country.str, k.item->value);
    }
    chash_vk_destroy(&vikings);
}

