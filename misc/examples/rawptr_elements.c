#include <stc/ccommon.h>
#include <stdio.h>

#include <stc/cstr.h>

// Create cmap of cstr => long*
#define i_type SIPtrMap
#define i_key_str
#define i_val long*
#define i_valraw long
#define i_valfrom(raw) c_new(long, raw)
#define i_valto(x) **x
#define i_valclone(x) c_new(long, *x)
#define i_valdrop(x) c_free(*x)
#include <stc/cmap.h>

// Alternatively, using cbox:
#define i_type IBox
#define i_val long
#include <stc/cbox.h> // unique_ptr<long> alike.

// cmap of cstr => IBox
#define i_type SIBoxMap
#define i_key_str
#define i_valboxed IBox // i_valboxed: use properties from IBox automatically
#include <stc/cmap.h>

int main()
{
    // These have the same behaviour, except IBox has a get member:
    SIPtrMap map1 = {0};
    SIBoxMap map2 = {0};

    printf("\nMap cstr => long*:\n");
    SIPtrMap_insert(&map1, cstr_from("Test1"), c_new(long, 1));
    SIPtrMap_insert(&map1, cstr_from("Test2"), c_new(long, 2));
    
    // Emplace implicitly creates cstr from const char* and an owned long* from long!
    SIPtrMap_emplace(&map1, "Test3", 3);
    SIPtrMap_emplace(&map1, "Test4", 4);

    c_forpair (name, number, SIPtrMap, map1)
        printf("%s: %ld\n", cstr_str(_.name), **_.number);

    puts("\nMap cstr => IBox:");
    SIBoxMap_insert(&map2, cstr_from("Test1"), IBox_make(1));
    SIBoxMap_insert(&map2, cstr_from("Test2"), IBox_make(2));
    
    // Emplace implicitly creates cstr from const char* and IBox from long!
    SIBoxMap_emplace(&map2, "Test3", 3);
    SIBoxMap_emplace(&map2, "Test4", 4);

    c_forpair (name, number, SIBoxMap, map2)
        printf("%s: %ld\n", cstr_str(_.name), *_.number->get);
    puts("");

    SIPtrMap_drop(&map1);
    SIBoxMap_drop(&map2);
}
