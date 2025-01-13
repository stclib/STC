#include "stc/common.h"
#include <stdio.h>
#include "stc/cstr.h"

#define i_type IBox,long
#include "stc/box.h" // unique_ptr<long> alike.

// hmap of cstr => IBox
#define i_type Boxmap
#define i_keypro cstr
#define i_valpro IBox // i_valpro: use properties from IBox automatically
#include "stc/hmap.h"


int main(void)
{
    Boxmap map = {0};

    puts("Map cstr => IBox:");
    Boxmap_insert(&map, cstr_lit("Test1"), IBox_make(1));
    Boxmap_insert(&map, cstr_lit("Test2"), IBox_make(2));

    // Simpler: emplace() implicitly creates cstr from const char* and IBox from long!
    Boxmap_emplace(&map, "Test3", 3);
    Boxmap_emplace(&map, "Test4", 4);

    for (c_each_kv(name, number, Boxmap, map))
        printf("%s: %ld\n", cstr_str(name), *number->get);
    puts("");

    Boxmap_drop(&map);
}
