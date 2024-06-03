#include "stc/common.h"
#include <stdio.h>
#define i_implement
#include "stc/cstr.h"

#define i_TYPE IBox,long
#include "stc/box.h" // unique_ptr<long> alike.

// hmap of cstr => IBox
#define i_type Boxmap
#define i_key_str
#define i_val_box IBox // i_val_box: use properties from IBox automatically
#include "stc/hmap.h"


int main(void)
{
    Boxmap map = {0};

    puts("Map cstr => IBox:");
    Boxmap_insert(&map, cstr_from("Test1"), IBox_make(1));
    Boxmap_insert(&map, cstr_from("Test2"), IBox_make(2));

    // Simpler: emplace() implicitly creates cstr from const char* and IBox from long!
    Boxmap_emplace(&map, "Test3", 3);
    Boxmap_emplace(&map, "Test4", 4);

    c_forpair (name, number, Boxmap, map)
        printf("%s: %ld\n", cstr_str(_.name), *_.number->get);
    puts("");

    Boxmap_drop(&map);
}
