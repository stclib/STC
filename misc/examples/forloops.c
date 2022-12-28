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
    puts("c_FORRANGE:");
    c_FORRANGE (30) printf(" xx");
    puts("");

    c_FORRANGE (i, 30) printf(" %lld", i);
    puts("");

    c_FORRANGE (i, 30, 60) printf(" %lld", i);
    puts("");

    c_FORRANGE (i, 30, 90, 2) printf(" %lld", i);


    puts("\n\nc_forlist:");
    c_FORLIST (i, int, {12, 23, 453, 65, 676})
        printf(" %d", *i.ref);
    puts("");

    c_FORLIST (i, const char*, {"12", "23", "453", "65", "676"})
        printf(" %s", *i.ref);
    puts("");

    c_FORLIST (i, const char*, {"12", "23", "453", "65", "676"})
        printf(" %s", i.data[i.size - 1 - i.index]);


    c_AUTO (IVec, vec) 
    c_AUTO (IMap, map)
    {
        c_FORLIST (i, int, {12, 23, 453, 65, 113, 215, 676, 34, 67, 20, 27, 66, 189, 45, 280, 199})
            IVec_push(&vec, *i.ref);

        c_FORLIST (i, IMap_value, {{12, 23}, {453, 65}, {676, 123}, {34, 67}})
            IMap_push(&map, *i.ref);

        puts("\n\nc_foreach:");
        c_FOREACH (i, IVec, vec)
            printf(" %d", *i.ref);
        puts("");

        c_FOREACH (i, IMap, map)
            printf(" (%d %d)", i.ref->first, i.ref->second);

        puts("\n\nc_forpair:");
        c_FORPAIR (key, val, IMap, map)
            printf(" (%d %d)", *_.key, *_.val);

        puts("\n\nc_forwhile:");
        c_FORWHILE (i, IVec, IVec_begin(&vec), i.index < 3)
            printf(" %d", *i.ref);

        #define isOdd(i) (*i.ref & 1)

        puts("\n\nc_forfilter:");
        c_FORFILTER (i, IVec, vec
                      , c_FLT_SKIPWHILE(i, *i.ref != 65)
                     && c_FLT_TAKEWHILE(i, *i.ref != 280)
                     && c_FLT_SKIPWHILE(i, isOdd(i))
                     && isOdd(i)
                     && c_FLT_SKIP(i, 2)
                      , c_FLT_TAKE(i, 1))
            printf(" %d", *i.ref);
        puts("");
        // 189
    }
}
