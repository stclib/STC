#define i_TYPE ivec,int
#define i_opt c_use_cmp | c_more
#include "stc/vec.h"
#include "stc/algo/quicksort.h"
#include "stc/crand.h"

#include <stdio.h>
#include <time.h>
#ifdef __cplusplus
#include <algorithm>
#endif


int main(int argc, char const *argv[])
{
    intptr_t N = 5000000;
    unsigned mask = (1 << 23) - 1;
    ivec v = {0};
    c_forrange (i, N)
        ivec_push(&v, crand() & mask);

    ivec_sort(&v);

    long long count = 0;
    clock_t t = clock();
    //csrand(0);
    c_forrange (i, N) {
        uint64_t r = crand() & mask;
        #ifdef __cplusplus
        #define LABEL "std::binary_search"
        count += std::binary_search(v.data, v.data + ivec_size(&v), r);
        #elif defined BSEARCH
        #define LABEL "bsearch"
        count += ivec_bsearch(&v, r) != NULL;
        #else
        #define LABEL "ivec_binary_search"
        count += ivec_binary_search(&v, r) != -1;
        #endif
    }
    t = clock() - t;
    printf("%s: count %lld %f\n", LABEL, count, (float)t/CLOCKS_PER_SEC);
    ivec_drop(&v);
}
