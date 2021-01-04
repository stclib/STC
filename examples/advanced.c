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

uint32_t vikingraw_hash(const VikingRaw* raw, size_t ignore) {
    uint32_t hash = c_string_hash(raw->name) ^ (c_string_hash(raw->country) << 3);
    return hash;
}
static inline int vikingraw_equals(const VikingRaw* rx, const VikingRaw* ry) {
    return strcmp(rx->name, ry->name) == 0 && strcmp(rx->country, ry->country) == 0;
}

static inline Viking viking_fromRaw(VikingRaw raw) { // note: parameter is by value
    Viking vk = {cstr_from(raw.name), cstr_from(raw.country)}; return vk;
}

static inline VikingRaw viking_toRaw(Viking* vk) {
    VikingRaw raw = {vk->name.str, vk->country.str}; return raw;
}

// With this in place, we use the full using_cmap() macro to define {Viking -> int} hash map type:

using_cmap(vk, Viking, int, c_default_del, c_default_clone,
                            vikingraw_equals, vikingraw_hash,
                            viking_del, viking_fromRaw, viking_toRaw, VikingRaw);

int main()
{
    cmap_vk vikings = cmap_vk_init();
    c_push_items(&vikings, cmap_vk, {
        { {"Einar", "Norway"}, 20},
        { {"Olaf", "Denmark"}, 24},
        { {"Harald", "Iceland"}, 12},
    });
    cmap_vk_put(&vikings, (VikingRaw){"Bjorn", "Sweden"}, 10);

    VikingRaw lookup = {"Einar", "Norway"};

    cmap_vk_entry_t *e = cmap_vk_find(&vikings, lookup);
    e->second += 3; // add 3 hp points to Einar
    cmap_vk_emplace(&vikings, lookup, 0).first->second += 5; // add 5 more to Einar

    c_foreach (k, cmap_vk, vikings) {
        printf("%s of %s has %d hp\n", k.ref->first.name.str, k.ref->first.country.str, k.ref->second);
    }
    cmap_vk_del(&vikings);
}