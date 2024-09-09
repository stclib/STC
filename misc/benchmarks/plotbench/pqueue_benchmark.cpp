#include <stdio.h>
#include <time.h>
#include "stc/random.h"

#define i_type PrQue,float
#define i_cmp -c_default_cmp
#include "stc/pqueue.h"

#include <queue>

static const uint32_t seed = 1234;
static const int N = 2500000;

void std_test(void)
{
    std::priority_queue<float, std::vector<float>, std::greater<float>> pq;
    crand64_seed(seed);
    clock_t start = clock();
    c_forrange (i, N)
        pq.push((float) crand64_real()*100000.0);

    printf("Built priority queue: %f secs\n", (float)(clock() - start)/(float)CLOCKS_PER_SEC);
    printf("%g ", pq.top());

    start = clock();
    c_forrange (i, N) {
        pq.pop();
    }

    printf("\npopped PQ: %f secs\n\n", (float)(clock() - start)/(float)CLOCKS_PER_SEC);
}


void stc_test(void)
{
    int N = 10000000;

    PrQue pq = {0};
    c_defer(PrQue_drop(&pq))
    {
        crand64_seed(seed);
        clock_t start = clock();
        c_forrange (i, N) {
            PrQue_push(&pq, (float)crand64_real()*100000);
        }

        printf("Built priority queue: %f secs\n", (float)(clock() - start)/(float)CLOCKS_PER_SEC);
        printf("%g ", *PrQue_top(&pq));

        start = clock();
        c_forrange (i, N) {
            PrQue_pop(&pq);
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
