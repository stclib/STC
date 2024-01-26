// Highly advanced example: Hashmap with struct as key with rawtype,
// and (optionally) wrapped in a smart pointer (box) which allocates Viking on the heap.

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

// Define RViking lookup struct with hash, cmp, and convertion
// functions between Viking and RViking structs:

typedef struct RViking {
    const char* name;
    const char* country;
} RViking;

static inline bool RViking_eq(const RViking* rx, const RViking* ry) {
    return strcmp(rx->name, ry->name) == 0 &&
           strcmp(rx->country, ry->country) == 0;
}

static inline uint64_t RViking_hash(const RViking* rp) {
    return c_hash_mix(c_hash_str(rp->name), c_hash_str(rp->country));
}

static inline Viking Viking_from(RViking raw) { // note: parameter is by value
    Viking v = {cstr_from(raw.name), cstr_from(raw.country)}; return v;
}

static inline RViking Viking_toraw(const Viking* vp) {
    RViking rv = {cstr_str(&vp->name), cstr_str(&vp->country)}; return rv;
}

#ifdef USE_BOX
// Wrap Viking into a smart pointer (box), which itself uses RViking as raw-type:
#define i_type          VikingBox
#define i_key_class     Viking
#define i_raw_class     RViking  // Lookup type ; binds _keyraw, _from and _toraw
#define i_use_eq                 // For box/arc, if neither of i_use_cmp, i_cmp, i_use_eq, i_eq, or i_hash
                                 // is defined, it does comparison/hashing of the object's address.
#include "stc/box.h"

// With this in place, we define the VikingBox => int hash map type:
#define i_type          Vikings
#define i_key_arcbox    VikingBox
#define i_val           int
#include "stc/hmap.h"

#else
// With this in place, we define the Viking => int hash map type:
#define i_type      Vikings
#define i_key_class Viking      // key type    ; binds _drop, _clone, _from, _toraw
#define i_raw_class RViking     // lookup type ; binds _cmp, _hash (unless overridden)
#define i_val       int         // mapped type
#include "stc/hmap.h"

#endif

int main(void)
{
    Vikings vikings = {0};
    Vikings_emplace(&vikings, c_LITERAL(RViking){"Einar", "Norway"}, 20);
    Vikings_emplace(&vikings, c_LITERAL(RViking){"Olaf", "Denmark"}, 24);
    Vikings_emplace(&vikings, c_LITERAL(RViking){"Harald", "Iceland"}, 12);
    Vikings_emplace(&vikings, c_LITERAL(RViking){"Björn", "Sweden"}, 10);

    Vikings_value* vv = Vikings_get_mut(&vikings, c_LITERAL(RViking){"Einar", "Norway"});
    if (vv) vv->second += 3; // add 3 hp points to Einar

#ifdef USE_BOX
    c_foreach (i, Vikings, vikings) {
        RViking rv = VikingBox_toraw(&i.ref->first); // two-level unwrapping
        printf("(boxed) %s of %s has %d hp\n", rv.name, rv.country, i.ref->second);
    }
#else
    c_foreach (i, Vikings, vikings) {
        RViking rv = Viking_toraw(&i.ref->first);
        printf("%s of %s has %d hp\n", rv.name, rv.country, i.ref->second);
    }
#endif

    Vikings_drop(&vikings);
}
