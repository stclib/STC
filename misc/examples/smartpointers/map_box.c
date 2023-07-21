#include <stc/ccommon.h>
#include <stdio.h>
#define i_implement
#include <stc/cstr.h>

#define i_type IBox
#define i_key long
#include <stc/cbox.h> // unique_ptr<long> alike.

// cmap of cstr => IBox
#define i_type Boxmap
#define i_key_str
#define i_valboxed IBox // i_valboxed: use properties from IBox automatically
#include <stc/cmap.h>


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
