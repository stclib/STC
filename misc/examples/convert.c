#include <stc/cstr.h>

#define i_key_str
#define i_val_str
#include <stc/cmap.h>

#define i_val_str
#include <stc/cvec.h>

#define i_val_str
#define i_extern // define _clist_mergesort() once
#include <stc/clist.h>

int main()
{
    cmap_str map, mclone;
    cvec_str keys = {0}, values = {0};
    clist_str list = {0};

    c_defer(
        cmap_str_drop(&map),
        cmap_str_drop(&mclone),
        cvec_str_drop(&keys),
        cvec_str_drop(&values),
        clist_str_drop(&list)
    ){
        map = c_make(cmap_str, {
            {"green", "#00ff00"},
            {"blue", "#0000ff"},
            {"yellow", "#ffff00"},
        });

        puts("MAP:");
        c_foreach (i, cmap_str, map)
            printf("  %s: %s\n", cstr_str(&i.ref->first), cstr_str(&i.ref->second));

        puts("\nCLONE MAP:");
        mclone = cmap_str_clone(map);
        c_foreach (i, cmap_str, mclone)
            printf("  %s: %s\n", cstr_str(&i.ref->first), cstr_str(&i.ref->second));

        puts("\nCOPY MAP TO VECS:");
        c_foreach (i, cmap_str, mclone) {
            cvec_str_emplace_back(&keys, cstr_str(&i.ref->first));
            cvec_str_emplace_back(&values, cstr_str(&i.ref->second));
        }
        c_forrange (i, cvec_str_size(&keys))
            printf("  %s: %s\n", cstr_str(keys.data + i), cstr_str(values.data + i));

        puts("\nCOPY VEC TO LIST:");
        c_foreach (i, cvec_str, keys)
            clist_str_emplace_back(&list, cstr_str(i.ref));

        c_foreach (i, clist_str, list)
            printf("  %s\n", cstr_str(i.ref));
    }
}
