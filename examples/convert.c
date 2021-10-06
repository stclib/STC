#include <stc/cstr.h>

#define i_key_str
#define i_val_str
#include <stc/cmap.h>

#define i_val_str
#include <stc/cvec.h>

#define i_val_str
#include <stc/clist.h>

int main()
{
    c_auto (cmap_str, map, mclone)
    c_auto (cvec_str, keys, values)
    c_auto (clist_str, list)
    {
        c_apply_pair(cmap_str, emplace, &map, {
            {"green", "#00ff00"},
            {"blue", "#0000ff"},
            {"yellow", "#ffff00"},
        });
        puts("MAP:");
        c_foreach (i, cmap_str, map)
            printf("  %s: %s\n", i.ref->first.str, i.ref->second.str);

        puts("\nCLONE MAP:");
        mclone = cmap_str_clone(map);
        c_foreach (i, cmap_str, mclone)
            printf("  %s: %s\n", i.ref->first.str, i.ref->second.str);

        puts("\nCOPY MAP TO VECS:");
        c_foreach (i, cmap_str, mclone) {
            cvec_str_emplace_back(&keys, i.ref->first.str);
            cvec_str_emplace_back(&values, i.ref->second.str);
        }
        c_forrange (i, cvec_str_size(keys))
            printf("  %s: %s\n", keys.data[i].str, values.data[i].str);

        puts("\nCOPY VEC TO LIST:");
        c_foreach (i, cvec_str, keys)
            clist_str_emplace_back(&list, i.ref->str);

        c_foreach (i, clist_str, list)
            printf("  %s\n", i.ref->str);
    }
}
