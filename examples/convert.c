
#include "stc/cmap.h"
#include "stc/csmap.h"
#include "stc/cvec.h"
#include "stc/clist.h"
#include "stc/cstr.h"
#include <stdio.h>

using_cmap_str();
using_cvec_str();
using_clist_str();

int main()
{
    cmap_str map = cmap_str_init();
    cmap_str_emplace(&map, "green", "#00ff00");
    cmap_str_emplace(&map, "blue", "#0000ff");
    cmap_str_emplace(&map, "yellow", "#ffff00");

    puts("MAP:");
    c_foreach (i, cmap_str, map)
        printf("  %s: %s\n", i.ref->first.str, i.ref->second.str);

    puts("\nCLONE MAP:");
    cmap_str mclone = cmap_str_clone(map);
    c_foreach (i, cmap_str, mclone)
        printf("  %s: %s\n", i.ref->first.str, i.ref->second.str);


    puts("\nMAP TO VECS:");
    cvec_str vec1 = cvec_str_init(), vec2 = cvec_str_init();
    c_foreach (i, cmap_str, mclone) {
        cvec_str_emplace_back(&vec1, i.ref->first.str);
        cvec_str_emplace_back(&vec2, i.ref->second.str);
    }
    c_forrange (i, cvec_str_size(vec1))
        printf("  %s: %s\n", vec1.data[i].str, vec2.data[i].str);

    puts("\nVEC TO LIST:");
    clist_str list = clist_str_init();
    c_foreach (i, cvec_str, vec1) clist_str_emplace_back(&list, i.ref->str);
    c_foreach (i, clist_str, list) printf("  %s\n", i.ref->str);

    c_del(cmap_str, &map, &mclone);
    c_del(cvec_str, &vec1, &vec2);
    clist_str_del(&list);
}