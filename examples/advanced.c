#include <stdio.h>
#include <stc/cmap.h>
#include <stc/cstr.h>

typedef struct Viking {
    cstr name;
    cstr country;
} Viking;

void viking_del(Viking* vk) {
    cstr_del(&vk->name);
    cstr_del(&vk->country);
}

// Define Viking raw struct with hash, equals, and convertion functions between Viking and VikingRaw structs:

typedef struct VikingRaw {
    const char* name;
    const char* country;
} VikingRaw;

uint64_t vikingraw_hash(const VikingRaw* raw, size_t ignore) {
    uint64_t hash = c_string_hash(raw->name) ^ (c_string_hash(raw->country) >> 15);
    return hash;
}
static inline int vikingraw_equals(const VikingRaw* rx, const VikingRaw* ry) {
    return strcmp(rx->name, ry->name) == 0 && strcmp(rx->country, ry->country) == 0;
}

static inline Viking viking_fromRaw(VikingRaw raw) { // note: parameter is by value
    return c_make(Viking){cstr_from(raw.name), cstr_from(raw.country)};
}
static inline VikingRaw viking_toRaw(const Viking* vk) {
    return c_make(VikingRaw){vk->name.str, vk->country.str};
}

// With this in place, we use the using_cmap_keydef() macro to define {Viking -> int} hash map type:

using_cmap_keydef(vk, Viking, int, vikingraw_equals, vikingraw_hash,
                      viking_del, viking_fromRaw, viking_toRaw, VikingRaw, c_true);

int main()
{
    c_forvar (cmap_vk vikings = cmap_vk_init(), cmap_vk_del(&vikings)) {
        c_emplace(cmap_vk, vikings, {
            { {"Einar", "Norway"}, 20},
            { {"Olaf", "Denmark"}, 24},
            { {"Harald", "Iceland"}, 12},
        });
        VikingRaw bjorn = {"Bjorn", "Sweden"};
        cmap_vk_emplace_or_assign(&vikings, bjorn, 10);

        VikingRaw einar = {"Einar", "Norway"};
        cmap_vk_value_t *e = cmap_vk_find(&vikings, einar).ref;
        e->second += 3; // add 3 hp points to Einar
        cmap_vk_emplace(&vikings, einar, 0).ref->second += 5; // add 5 more to Einar

        c_foreach (k, cmap_vk, vikings) {
            printf("%s of %s has %d hp\n", k.ref->first.name.str, k.ref->first.country.str, k.ref->second);
        }
    }
}