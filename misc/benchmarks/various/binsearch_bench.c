#define i_type ivec,int
#define i_opt c_use_cmp | c_more
#include "stc/vec.h"
#include "stc/algo/quicksort.h"
#include "stc/crand.h"

#include <stdio.h>
#include <time.h>
#ifdef __cplusplus
#include <algorithm>
#endif

// Use quicksort and binary_search from "quicksort.h"

int main(int argc, char const *argv[])
{
    isize N = 5000000;
    unsigned mask = (1 << 23) - 1;
    ivec v = {0};
    c_forrange (i, N)
        ivec_push(&v, crand() & mask);

    ivec_quicksort(&v);

    long long count = 0;
    clock_t t = clock();
    //csrand(0);
    c_forrange (i, N) {
        uint64_t r = crand() & mask;
        #ifdef __cplusplus
          #define LABEL "std::binary_search"
          count += std::binary_search(v.data, v.data + ivec_size(&v), r);
        #else
          #define LABEL "ivec_binary_search"
          count += ivec_binary_search(&v, r) != -1;
        #endif
    }
    t = clock() - t;
    printf("elements: %d\n", (int)N);
    printf("%s: found %lld %f\n", LABEL, count, (float)t/CLOCKS_PER_SEC);
    ivec_drop(&v);
}
