// Advanced example: Hashmap with struct as key with rawtype,

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

// Define RawVik lookup struct with hash, cmp, and convertion
// functions between Viking and RawVik structs:

typedef struct RawVik {
    const char* name;
    const char* country;
} RawVik;

static inline bool RawVik_eq(const RawVik* rx, const RawVik* ry) {
    return strcmp(rx->name, ry->name) == 0 &&
           strcmp(rx->country, ry->country) == 0;
}

static inline size_t RawVik_hash(const RawVik* rp) {
    return c_hash_mix(c_hash_str(rp->name), c_hash_str(rp->country));
}

static inline Viking Viking_from(RawVik raw) { // note: parameter is by value
    Viking v = {cstr_from(raw.name), cstr_from(raw.country)}; return v;
}

static inline RawVik Viking_toraw(const Viking* vp) {
    RawVik rv = {cstr_str(&vp->name), cstr_str(&vp->country)}; return rv;
}

// With this in place, we define the Viking => int hash map type:
#define i_type      Vikings
#define i_keyclass  Viking  // binds _drop, _clone, _from, _toraw
#define i_rawclass  RawVik  // bind RawVik_cmp, RawVik_eq, RawVik_hash
#define i_val       int     // mapped type
#include "stc/hmap.h"


int main(void)
{
    Vikings vikings = {0};
    Vikings_emplace(&vikings, c_literal(RawVik){"Einar", "Norway"}, 20);
    Vikings_emplace(&vikings, c_literal(RawVik){"Olaf", "Denmark"}, 24);
    Vikings_emplace(&vikings, c_literal(RawVik){"Harald", "Iceland"}, 12);
    Vikings_emplace(&vikings, c_literal(RawVik){"Björn", "Sweden"}, 10);

    Vikings_value* vv = Vikings_get_mut(&vikings, c_literal(RawVik){"Einar", "Norway"});
    if (vv) vv->second += 3; // add 3 hp points to Einar

    for (c_each(i, Vikings, vikings)) {
        RawVik rv = Viking_toraw(&i.ref->first);
        printf("%s of %s has %d hp\n", rv.name, rv.country, i.ref->second);
    }

    Vikings_drop(&vikings);
}
