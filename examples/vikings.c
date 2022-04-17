#include <stdio.h>
#include <stc/cstr.h>

typedef struct Viking {
    cstr name;
    cstr country;
} Viking;

void Viking_drop(Viking* vk) {
    cstr_drop(&vk->name);
    cstr_drop(&vk->country);
}

// Define Viking raw struct with hash, equals, and convertion functions between Viking and RViking structs:

typedef struct RViking {
    const char* name;
    const char* country;
} RViking;

uint64_t RViking_hash(const RViking* raw, size_t ignore) {
    uint64_t hash = c_strhash(raw->name) ^ (c_strhash(raw->country) >> 15);
    return hash;
}
static inline bool RViking_eq(const RViking* rx, const RViking* ry) {
    return strcmp(rx->name, ry->name) == 0 && strcmp(rx->country, ry->country) == 0;
}

static inline Viking Viking_from(RViking raw) { // note: parameter is by value
    return c_make(Viking){cstr_from(raw.name), cstr_from(raw.country)};
}
static inline RViking Viking_toraw(const Viking* vk) {
    return c_make(RViking){cstr_str(&vk->name), cstr_str(&vk->country)};
}

// With this in place, we define the Viking => int hash map type:
#define i_type      Vikings
#define i_key_bind  Viking
#define i_keyraw    RViking
#define i_val       int
// i_key_bind auto-binds these functions:
//   i_hash     => Viking_hash
//   i_eq       => Viking_eq
//   i_keyfrom  => Viking_from // not _clone because i_keyraw is defined
//   i_keyto    => Viking_toraw
//   i_keydrop  => Viking_drop
#include <stc/cmap.h>

int main()
{
    c_auto (Vikings, vikings) {
        c_apply(v, Vikings_emplace(&vikings, c_pair(v)), Vikings_raw, {
            {{"Einar", "Norway"}, 20},
            {{"Olaf", "Denmark"}, 24},
            {{"Harald", "Iceland"}, 12},
        });
        RViking bjorn = {"Bjorn", "Sweden"};
        Vikings_emplace_or_assign(&vikings, bjorn, 10);

        RViking einar = {"Einar", "Norway"};
        Vikings_value* v = Vikings_get_mut(&vikings, einar);
        v->second += 3; // add 3 hp points to Einar
        Vikings_emplace(&vikings, einar, 0).ref->second += 5; // add 5 more to Einar

        c_forpair (viking, hp, Vikings, vikings) {
            printf("%s of %s has %d hp\n", cstr_str(&_.viking.name), cstr_str(&_.viking.country), _.hp);
        }
    }
}
