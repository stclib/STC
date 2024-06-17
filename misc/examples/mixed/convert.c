#define i_implement
#include "stc/cstr.h"
#include "stc/algorithm.h"

#define i_key_cstr
#define i_val_cstr
#include "stc/hmap.h"

#define i_key_cstr
#include "stc/vec.h"

#define i_key_cstr
#include "stc/list.h"

int main(void)
{
    hmap_cstr map = {0}, mclone = {0};
    vec_cstr keys = {0}, values = {0};
    list_cstr list = {0};

    c_defer(
        hmap_cstr_drop(&map),
        hmap_cstr_drop(&mclone),
        vec_cstr_drop(&keys),
        vec_cstr_drop(&values),
        list_cstr_drop(&list)
    ){
        map = c_init(hmap_cstr, {
            {"green", "#00ff00"},
            {"blue", "#0000ff"},
            {"yellow", "#ffff00"},
        });

        puts("MAP:");
        c_filter(hmap_cstr, map,
            printf("  %s: %s\n", cstr_str(&value->first), cstr_str(&value->second)));

        puts("\nCLONE MAP:");
        mclone = hmap_cstr_clone(map);
        // print
        c_filter(hmap_cstr, mclone,
            printf("  %s: %s\n", cstr_str(&value->first), cstr_str(&value->second)));

        puts("\nCOPY MAP TO VECS:");
        c_filter(hmap_cstr, mclone, (vec_cstr_push(&keys, cstr_clone(value->first)),
                                    vec_cstr_push(&values, cstr_clone(value->second))));
        // print both keys and values zipped
        c_filter_zip(vec_cstr, keys, values,
            printf("  %s: %s\n", cstr_str(value1), cstr_str(value2)));

        puts("\nCOPY VEC TO LIST:");
        c_copy(vec_cstr, keys, list_cstr, &list);
        // print
        c_filter(list_cstr, list, printf("  %s\n", cstr_str(value)));

        puts("\nCOPY VEC AND LIST TO MAP:");
        hmap_cstr_clear(&map);
        c_filter_zip(vec_cstr, values, list_cstr, list,
            hmap_cstr_emplace(&map, cstr_str(value1), cstr_str(value2)));
        // print inverted map
        c_filter(hmap_cstr, map,
            printf("  %s: %s\n", cstr_str(&value->first), cstr_str(&value->second)));
    }
}
