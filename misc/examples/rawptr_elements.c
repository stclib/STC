#include <stc/ccommon.h>
#include <stdio.h>

#include <stc/cstr.h>
// Map of cstr => int64 pointers
typedef int64_t inttype;

// Do it without cbox:
#define i_type SIPtrMap
#define i_key_str
#define i_val inttype*
#define i_valraw inttype
#define i_valfrom(raw) c_NEW(inttype, raw)
#define i_valto(x) **x
#define i_valclone(x) c_NEW(inttype, *x)
#define i_valdrop(x) c_FREE(*x)
#include <stc/cmap.h>

// With cbox:
#define i_type IBox
#define i_val int
#include <stc/cbox.h> //<stc/carc.h>

#define i_type SIBoxMap
#define i_key_str
#define i_valboxed IBox
#include <stc/cmap.h>

int main()
{
    c_AUTO (SIPtrMap, map, m1)
    c_AUTO (SIBoxMap, m2)
    {
        printf("\nMap with pointer elements:\n");
        SIPtrMap_insert(&map, cstr_from("testing"), c_NEW(inttype, 1));
        SIPtrMap_insert(&map, cstr_from("done"), c_NEW(inttype, 2));

        // Emplace: implicit key, val construction:
        SIPtrMap_emplace(&map, "hello", 3);
        SIPtrMap_emplace(&map, "goodbye", 4);

        m1 = SIPtrMap_clone(map);

        c_FORPAIR (name, number, SIPtrMap, m1)
            printf("%s: %" PRId64 "\n", cstr_str(_.name), **_.number);


        puts("\nIBox map:");
        SIBoxMap_insert(&m2, cstr_from("Hello"), IBox_make(123));
        SIBoxMap_emplace(&m2, "World", 999);
        c_FORPAIR (name, number, SIBoxMap, m2)
            printf("%s: %d\n", cstr_str(_.name), *_.number->get);
        puts("");
    }
}
