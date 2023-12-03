#include <stdio.h>
#include <time.h>
#define i_static
#include "stc/crand.h"

#define i_TYPE pque_flt, float
#define i_cmp -c_default_cmp
#include "stc/pque.h"

#include <queue>

static const uint32_t seed = 1234;
static const int N = 2500000;

void std_test()
{
    std::priority_queue<float, std::vector<float>, std::greater<float>> pq;
    csrand(seed);
    clock_t start = clock();
    c_forrange (i, N)
        pq.push((float) crandf()*100000.0);

    printf("Built priority queue: %f secs\n", (float)(clock() - start)/(float)CLOCKS_PER_SEC);
    printf("%g ", pq.top());

    start = clock();
    c_forrange (i, N) {
        pq.pop();
    }

    printf("\npopped PQ: %f secs\n\n", (float)(clock() - start)/(float)CLOCKS_PER_SEC);
}


void stc_test()
{
    int N = 10000000;

    pque_flt pq = {0};
    c_defer(pque_flt_drop(&pq))
    {
        csrand(seed);
        clock_t start = clock();
        c_forrange (i, N) {
            pque_flt_push(&pq, (float) crandf()*100000);
        }

        printf("Built priority queue: %f secs\n", (float)(clock() - start)/(float)CLOCKS_PER_SEC);
        printf("%g ", *pque_flt_top(&pq));

        start = clock();
        c_forrange (i, N) {
            pque_flt_pop(&pq);
        }

        printf("\npopped PQ: %f secs\n", (clock() - start) / (float) CLOCKS_PER_SEC);
    }
}


int main(void)
{
    puts("STD P.QUEUE:");
    std_test();
    puts("\nSTC P.QUEUE:");
    stc_test();
}
