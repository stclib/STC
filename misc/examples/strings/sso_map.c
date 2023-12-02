#define i_implement
#include "stc/cstr.h"
#define i_key_str
#define i_val_str
#include "stc/hmap.h"

int main(void)
{
    hmap_str m = {0};
    hmap_str_emplace(&m, "Test short", "This is a short string");
    hmap_str_emplace(&m, "Test long ", "This is a longer string");

    c_forpair (k, v, hmap_str, m)
        printf("%s: '%s' Len=%" c_ZI ", Is long: %s\n",
                cstr_str(_.k), cstr_str(_.v), cstr_size(_.v),
                cstr_is_long(_.v) ? "true" : "false");

    hmap_str_drop(&m);
}
