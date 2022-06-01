#include <stc/cstr.h>

typedef struct Viking {
    cstr name;
    cstr country;
} Viking;

void Viking_drop(Viking* vk) {
    cstr_drop(&vk->name);
    cstr_drop(&vk->country);
}

// Define Viking lookup struct with hash, cmp, and convertion functions between Viking and RViking structs:

typedef struct RViking {
    const char* name;
    const char* country;
} RViking;

static inline int RViking_cmp(const RViking* rx, const RViking* ry) {
    int c = strcmp(rx->name, ry->name);
    return c ? c : strcmp(rx->country, ry->country);
}

static inline Viking Viking_from(RViking raw) { // note: parameter is by value
    return c_make(Viking){cstr_from(raw.name), cstr_from(raw.country)};
}
static inline Viking Viking_clone(Viking vk) { // note: parameter is by value
    vk.name = cstr_clone(vk.name), vk.country = cstr_clone(vk.country);
    return vk;
}
static inline RViking Viking_toraw(const Viking* vp) {
    return c_make(RViking){cstr_str(&vp->name), cstr_str(&vp->country)};
}

// With this in place, we define the Viking => int hash map type:
#define i_type      Vikings
#define i_key_bind  Viking      // key type
#define i_keyraw    RViking     // lookup type
#define i_keyfrom   Viking_from // convert from lookup type (enables emplace)
#define i_hash(rp)  c_strhash(rp->name) ^ c_strhash(rp->country)
#define i_val       int         // mapped type
// i_key_bind auto-binds these functions (unless they are defined by i_...):
//   i_cmp      => RViking_cmp
//   i_hash     => RViking_hash
//   i_keyclone => Viking_clone
//   i_keyto    => Viking_toraw // because i_keyraw is defined
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

        c_forpair (vik, hp, Vikings, vikings) {
            printf("%s of %s has %d hp\n", cstr_str(&_.vik->name), cstr_str(&_.vik->country), *_.hp);
        }
    }
}
