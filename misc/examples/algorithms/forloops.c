#include <stdio.h>
#include "stc/algorithm.h"

#define i_TYPE IVec,int
#include "stc/stack.h"

#define i_TYPE IMap,int,int
#include "stc/hmap.h"


int main(void)
{
    puts("c_forrange:");
    c_forrange (30) printf(" xx");
    puts("");

    c_forrange (i, 30) printf(" %d", (int)i);
    puts("");

    c_forrange (i, 30, 60) printf(" %d", (int)i);
    puts("");

    c_forrange (i, 30, 90, 2) printf(" %d", (int)i);

    puts("\n\nc_forlist:");
    c_foritems (i, int, {12, 23, 453, 65, 676})
        printf(" %d", *i.ref);
    puts("");

    c_foritems (i, const char*, {"12", "23", "453", "65", "676"})
        printf(" %s", *i.ref);
    puts("");

    IVec vec = c_init(IVec, {12, 23, 453, 65, 113, 215, 676, 34, 67, 20, 27, 66, 189, 45, 280, 199});
    IMap map = c_init(IMap, {{12, 23}, {453, 65}, {676, 123}, {34, 67}});

    puts("\n\nc_foreach:");
    c_foreach (i, IVec, vec)
        printf(" %d", *i.ref);

    puts("\n\nc_foreach in map:");
    c_foreach (i, IMap, map)
        printf(" (%d %d)", i.ref->first, i.ref->second);

    puts("\n\nc_forpair:");
    c_forpair (key, val, IMap, map)
        printf(" (%d %d)", *_.key, *_.val);

    #define f_isOdd() (*value & 1)

    puts("\n\nc_filter:");
    c_filter(IVec, vec
         , f_isOdd()
        && c_flt_skip(4)
        && (printf(" %d", *value), c_flt_take(4))
    );

    IVec_drop(&vec);
    IMap_drop(&map);
}
