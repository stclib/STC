#define STC_USE_SSO 1
#include <stc/cstr.h>
#define i_key_str
#define i_val_str
#include <stc/cmap.h>


int main()
{
    c_auto (cmap_str, m) {
        cmap_str_emplace(&m, "Test short", "This is a short string.");
        cmap_str_emplace(&m, "Test long ", "This is a longer string.");

        c_forpair (k, v, cmap_str, m)
            printf("%s: '%s' Len=%" PRIuMAX ", Is long: %s\n", 
                   cstr_str(&_.k), cstr_str(&_.v), cstr_size(_.v),
                   cstr_is_long(&_.v)?"true":"false");
    }
}
