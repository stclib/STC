#include <stc/cstr.h>

typedef struct Viking {
    cstr name;
    cstr country;
} Viking;

Viking Viking_make(cstr_raw name, cstr_raw country) {
    return (Viking){.name = cstr_from(name), .country = cstr_from(country)};
}

void Viking_drop(Viking* vk) {
    cstr_drop(&vk->name);
    cstr_drop(&vk->country);
}

Viking Viking_clone(Viking v) {
    v.name = cstr_clone(v.name);
    v.country = cstr_clone(v.country);
    return v;
}

// Define Viking_raw, a Viking lookup struct with eq, hash and conversion functions between them:
typedef struct {
    const char* name;
    const char* country;
} Viking_raw;

bool Viking_raw_eq(const Viking_raw* rx, const Viking_raw* ry) {
    return strcmp(rx->name, ry->name)==0 && strcmp(rx->country, ry->country)==0;
}

size_t Viking_raw_hash(const Viking_raw* rv) {
    return c_hash_mix(c_hash_str(rv->name), c_hash_str(rv->country));
}

Viking Viking_from(Viking_raw raw) { // note: parameter is by value
    Viking v = {cstr_from(raw.name), cstr_from(raw.country)}; return v;
}

Viking_raw Viking_toraw(const Viking* vp) {
    Viking_raw rv = {cstr_str(&vp->name), cstr_str(&vp->country)}; return rv;
}

// Define the map. Viking is now a "pro"-type:
#define T Players, Viking, int, (c_keypro)
#include <stc/hashmap.h>

int main(void)
{
    Players vikings = c_make(Players, {
        {{"Einar", "Norway"}, 25},
        {{"Olaf", "Denmark"}, 24},
        {{"Harald", "Iceland"}, 12},
    });

    // Now lookup is using Viking_raw, not Viking:
    printf("Lookup: Olaf of Denmark has %d hp\n\n", *Players_at(&vikings, (Viking_raw){"Olaf", "Denmark"}));

    for (c_each(v, Players, vikings)) {
        Players_raw r = Players_value_toraw(v.ref);
        printf("%s of %s has %d hp\n", r.first.name, r.first.country, r.second);
    }
    Players_drop(&vikings);
}
