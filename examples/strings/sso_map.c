#include "stc/cstr.h"
#define i_keypro cstr
#define i_valpro cstr
#include "stc/hmap.h"

int main(void)
{
    hmap_cstr m = {0};
    hmap_cstr_emplace(&m, "Test short", "This is a short string");
    hmap_cstr_emplace(&m, "Test long ", "This is a longer string");

    for (c_each_kv(k, v, hmap_cstr, m))
        printf("%s: '%s' Len=%d, Is long: %s\n",
                cstr_str(k), cstr_str(v), (int)cstr_size(v),
                cstr_is_long(v) ? "true" : "false");

    hmap_cstr_drop(&m);
}
