#define i_implement
#include "stc/cstr.h"
#include "stc/algorithm.h"

#define i_key_str
#define i_val_str
#include "stc/hmap.h"

#define i_key_str
#include "stc/vec.h"

#define i_key_str
#include "stc/list.h"

int main(void)
{
    hmap_str map = {0}, mclone = {0};
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
        c_filter(hmap_str, map,
            printf("  %s: %s\n", cstr_str(&value->first), cstr_str(&value->second)));

        puts("\nCLONE MAP:");
        mclone = hmap_str_clone(map);
        // print
        c_filter(hmap_str, mclone,
            printf("  %s: %s\n", cstr_str(&value->first), cstr_str(&value->second)));

        puts("\nCOPY MAP TO VECS:");
        c_filter(hmap_str, mclone, (vec_str_push(&keys, cstr_clone(value->first)),
                                    vec_str_push(&values, cstr_clone(value->second))));
        // print both keys and values zipped
        c_filter_zip(vec_str, keys, values,
            printf("  %s: %s\n", cstr_str(value1), cstr_str(value2)));

        puts("\nCOPY VEC TO LIST:");
        c_copy(vec_str, keys, list_str, &list);
        // print
        c_filter(list_str, list, printf("  %s\n", cstr_str(value)));

        puts("\nCOPY VEC AND LIST TO MAP:");
        hmap_str_clear(&map);
        c_filter_zip(vec_str, values, list_str, list,
            hmap_str_emplace(&map, cstr_str(value1), cstr_str(value2)));
        // print inverted map
        c_filter(hmap_str, map,
            printf("  %s: %s\n", cstr_str(&value->first), cstr_str(&value->second)));
    }
}
