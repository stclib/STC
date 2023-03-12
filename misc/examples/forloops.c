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

    c_forlist (i, const char*, {"12", "23", "453", "65", "676"})
        printf(" %s", i.data[i.size - 1 - i.index]);


    c_auto (IVec, vec) 
    c_auto (IMap, map)
    {
        c_forlist (i, int, {12, 23, 453, 65, 113, 215, 676, 34, 67, 20, 27, 66, 189, 45, 280, 199})
            IVec_push(&vec, *i.ref);

        c_forlist (i, IMap_value, {{12, 23}, {453, 65}, {676, 123}, {34, 67}})
            IMap_push(&map, *i.ref);

        puts("\n\nc_foreach:");
        c_foreach (i, IVec, vec)
            printf(" %d", *i.ref);
        puts("");

        c_foreach (i, IMap, map)
            printf(" (%d %d)", i.ref->first, i.ref->second);

        puts("\n\nc_forpair:");
        c_forpair (key, val, IMap, map)
            printf(" (%d %d)", *_.key, *_.val);

        puts("\n\nc_forfilter 1:");
        c_forfilter (i, IVec, vec, c_flt_take(i, 3))
            printf(" %d", *i.ref);

        #define isOdd(i) (*i.ref & 1)

        puts("\n\nc_forfilter 2:");
        c_forfilter (i, IVec, vec,
                        c_flt_skipwhile(i, *i.ref != 65)  &&
                        c_flt_takewhile(i, *i.ref != 280) &&
                        c_flt_skipwhile(i, isOdd(i))      &&
                        isOdd(i)                          &&
                        c_flt_skip(i, 2)                  &&
                        c_flt_take(i, 2))
            printf(" %d", *i.ref);
        puts("");
        // 189
    }
}
