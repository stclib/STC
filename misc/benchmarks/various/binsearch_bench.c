#define NDEBUG
#include <stdio.h>
#include <time.h>
#ifdef __cplusplus
#include <algorithm>
#endif

#define i_type ivec, int
#define i_use_cmp
#include "stc/stack.h"
#include "stc/random.h"

// Use quicksort and binary_search from "sort.h"

int main(int argc, char const *argv[])
{
    isize N = 5000000;
    unsigned mask = (1 << 23) - 1;
    ivec v = {0};
    c_forrange (i, N)
        ivec_push(&v, crand64_uint() & mask);

    ivec_sort(&v);

    long long count = 0;
    clock_t t = clock();
    //crand64_seed(1);
    c_forrange (i, N) {
        uint64_t r = crand64_uint() & mask;
        #ifdef __cplusplus
          #define LABEL "std::binary_search"
          count += std::binary_search(v.data, v.data + ivec_size(&v), r);
        #else
          #define LABEL "ivec_binary_search"
          count += ivec_binary_search(&v, r) != c_NPOS;
        #endif
    }
    t = clock() - t;
    printf("elements: %d\n", (int)N);
    printf("%s: found %d %f\n", LABEL, (int)count, (float)t/CLOCKS_PER_SEC);
    ivec_drop(&v);
}
