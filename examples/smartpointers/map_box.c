#include <stdio.h>
#include <stc/common.h>
#include <stc/cstr.h>

#define T IBox, long
#include <stc/box.h> // unique_ptr<long> alike.

// hashmap of cstr => IBox (both cstr and box are "pro")
#define T Boxmap, cstr, IBox, (c_keypro | c_valpro)
#include <stc/hashmap.h>


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
