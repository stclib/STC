#include <stc/ccommon.h>
#include <stdio.h>
#define i_implement
#include <stc/cstr.h>

// cmap of cstr => long*
#define i_type Ptrmap
#define i_key_str
#define i_val long*
#define i_valraw long
#define i_valfrom(raw) c_new(long, raw)
#define i_valto(x) **x
#define i_valclone(x) c_new(long, *x)
#define i_valdrop(x) c_free(*x)
#include <stc/cmap.h>

int main(void)
{
    Ptrmap map = {0};

    puts("Map cstr => long*:");
    Ptrmap_insert(&map, cstr_from("Test1"), c_new(long, 1));
    Ptrmap_insert(&map, cstr_from("Test2"), c_new(long, 2));
    
    // Simple: emplace() implicitly creates cstr from const char* and an owned long* from long!
    Ptrmap_emplace(&map, "Test3", 3);
    Ptrmap_emplace(&map, "Test4", 4);

    c_forpair (name, number, Ptrmap, map)
        printf("%s: %ld\n", cstr_str(_.name), **_.number);
    puts("");

    Ptrmap_drop(&map);
}
