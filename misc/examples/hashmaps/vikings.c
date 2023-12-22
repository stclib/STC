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

static inline bool RViking_eq(const RViking* rx, const RViking* ry) {
    return strcmp(rx->name, ry->name) == 0 &&
           strcmp(rx->country, ry->country) == 0;
}

static inline Viking Viking_from(RViking raw) { // note: parameter is by value
    Viking v = {cstr_from(raw.name), cstr_from(raw.country)}; return v;
}

static inline RViking Viking_toraw(const Viking* vp) {
    RViking rv = {cstr_str(&vp->name), cstr_str(&vp->country)}; return rv;
}

// With this in place, we define the Viking => int hash map type:
#define i_type      Vikings
#define i_raw_class RViking     // lookup type ; binds _cmp, _hash (unless overridden)
#define i_key_class Viking      // key type    ; binds _drop, _clone, _from, _toraw
#define i_hash(rp)  c_hash_mix(c_hash_str(rp->name), c_hash_str(rp->country))
#define i_val       int         // mapped type
#include "stc/hmap.h"

int main(void)
{
    Vikings vikings = {0};
    Vikings_emplace(&vikings, c_LITERAL(RViking){"Einar", "Norway"}, 20);
    Vikings_emplace(&vikings, c_LITERAL(RViking){"Olaf", "Denmark"}, 24);
    Vikings_emplace(&vikings, c_LITERAL(RViking){"Harald", "Iceland"}, 12);
    Vikings_emplace(&vikings, c_LITERAL(RViking){"Björn", "Sweden"}, 10);

    Vikings_value* v = Vikings_get_mut(&vikings, c_LITERAL(RViking){"Einar", "Norway"});
    v->second += 3; // add 3 hp points to Einar

    c_forpair (vk, hp, Vikings, vikings) {
        printf("%s of %s has %d hp\n", cstr_str(&_.vk->name), cstr_str(&_.vk->country), *_.hp);
    }
    Vikings_drop(&vikings);
}
