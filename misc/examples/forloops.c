#include <stdio.h>
#include <stc/algo/filter.h>

#define i_type IVec
#define i_val int
#include <stc/cstack.h>

#define i_type IMap
#define i_key int
#define i_val int
#include <stc/cmap.h>


int main()
{
    puts("c_forrange:");
    c_forrange (30) printf(" xx");
    puts("");

    c_forrange (i, 30) printf(" %lld", i);
    puts("");

    c_forrange (i, 30, 60) printf(" %lld", i);
    puts("");

    c_forrange (i, 30, 90, 2) printf(" %lld", i);

    puts("\n\nc_forlist:");
    c_forlist (i, int, {12, 23, 453, 65, 676})
        printf(" %d", *i.ref);
    puts("");

    c_forlist (i, const char*, {"12", "23", "453", "65", "676"})
        printf(" %s", *i.ref);
    puts("");

    IVec vec = c_make(IVec, {12, 23, 453, 65, 113, 215, 676, 34, 67, 20, 27, 66, 189, 45, 280, 199});
    IMap map = c_make(IMap, {{12, 23}, {453, 65}, {676, 123}, {34, 67}});

    puts("\n\nc_foreach:");
    c_foreach (i, IVec, vec)
        printf(" %d", *i.ref);

    puts("\n\nc_foreach_r: reverse");
    c_foreach_rv (i, IVec, vec)
        printf(" %d", *i.ref);

    puts("\n\nc_foreach in map:");
    c_foreach (i, IMap, map)
        printf(" (%d %d)", i.ref->first, i.ref->second);

    puts("\n\nc_forpair:");
    c_forpair (key, val, IMap, map)
        printf(" (%d %d)", *_.key, *_.val);
    
    #define isOdd(i) (*i.ref & 1)

    puts("\n\nc_forfilter:");
    c_forfilter (i, IVec, vec, 
        isOdd(i)          &&
        c_flt_skip(i, 4)  &&
        c_flt_take(i, 4)
    ){
        printf(" %d", *i.ref);
    }

    IVec_drop(&vec);
    IMap_drop(&map);
}
