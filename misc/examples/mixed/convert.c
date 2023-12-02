#define i_implement
#include "stc/cstr.h"

#define i_key_str
#define i_val_str
#include "stc/hmap.h"

#define i_key_str
#include "stc/vec.h"

#define i_key_str
#include "stc/list.h"

int main(void)
{
    hmap_str map, mclone;
    vec_str keys = {0}, values = {0};
    list_str list = {0};

    c_defer(
        hmap_str_drop(&map),
        hmap_str_drop(&mclone),
        vec_str_drop(&keys),
        vec_str_drop(&values),
        list_str_drop(&list)
    ){
        map = c_init(hmap_str, {
            {"green", "#00ff00"},
            {"blue", "#0000ff"},
            {"yellow", "#ffff00"},
        });

        puts("MAP:");
        c_foreach (i, hmap_str, map)
            printf("  %s: %s\n", cstr_str(&i.ref->first), cstr_str(&i.ref->second));

        puts("\nCLONE MAP:");
        mclone = hmap_str_clone(map);
        c_foreach (i, hmap_str, mclone)
            printf("  %s: %s\n", cstr_str(&i.ref->first), cstr_str(&i.ref->second));

        puts("\nCOPY MAP TO VECS:");
        c_foreach (i, hmap_str, mclone) {
            vec_str_emplace_back(&keys, cstr_str(&i.ref->first));
            vec_str_emplace_back(&values, cstr_str(&i.ref->second));
        }
        c_forrange (i, vec_str_size(&keys))
            printf("  %s: %s\n", cstr_str(keys.data + i), cstr_str(values.data + i));

        puts("\nCOPY VEC TO LIST:");
        c_foreach (i, vec_str, keys)
            list_str_emplace_back(&list, cstr_str(i.ref));

        c_foreach (i, list_str, list)
            printf("  %s\n", cstr_str(i.ref));
    }
}
