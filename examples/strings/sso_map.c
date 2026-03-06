#include <stc/cstr.h>
#define T Map, cstr, cstr, (c_pro_key | c_pro_val)
#include <stc/hashmap.h>

int main(void)
{
    Map map = {0};
    Map_emplace(&map, "Test short", "This is a short string");
    Map_emplace(&map, "Test long ", "This is a longer string");

    for (c_each_kv(k, v, Map, map)) {
        printf("%s: '%s' Len=%d, Is long: %s\n",
               cstr_str(k), cstr_str(v), (int)cstr_size(v),
               cstr_is_long(v) ? "true" : "false");
    }
    Map_drop(&map);
}
