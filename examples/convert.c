
#include "stc/cmap.h"
#include "stc/cvec.h"
#include "stc/clist.h"
#include "stc/cstr.h"
#include <stdio.h>

using_cmap_str();
using_cvec(ss, cmap_str_value_t, c_no_compare, cmap_str_value_del, cmap_str_value_clone);
using_clist(ss, cmap_str_value_t, c_no_compare, cmap_str_value_del, cmap_str_value_clone);

int main()
{
    cmap_str map = cmap_inits;
    cmap_str_emplace(&map, "green", "#00ff00");
    cmap_str_emplace(&map, "blue", "#0000ff");
    cmap_str_emplace(&map, "yellow", "#ffff00");

    puts("cmap_str:");
    c_foreach (i, cmap_str, map)
        printf("  %s: %s\n", i.ref->first.str, i.ref->second.str);

    cmap_str clone = cmap_str_clone(map);

    puts("\ncmap_str clone:");
    c_foreach (i, cmap_str, clone)
        printf("  %s: %s\n", i.ref->first.str, i.ref->second.str);

    cvec_ss vec = cvec_ss_init();
    c_convert(cmap_str, map, cvec_ss, push_back, &vec);

    puts("\nvec_ss:");
    c_foreach (i, cvec_ss, vec)
        printf("  %s: %s\n", i.ref->first.str, i.ref->second.str);    

    clist_ss list = clist_ss_init();
    c_convert(cmap_str, map, clist_ss, push_back, &list);

    puts("\nclist_ss:");
    c_foreach (i, clist_ss, list)
        printf("  %s: %s\n", i.ref->first.str, i.ref->second.str);    

    c_del(cmap_str, &map, &clone);
    cvec_ss_del(&vec);
    clist_ss_del(&list);
}