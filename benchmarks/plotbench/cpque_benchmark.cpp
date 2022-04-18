#include <stdio.h>
#include <time.h>
#include <stc/crandom.h>

#define i_val float
#define i_cmp -c_default_cmp
#define i_tag f
#include <stc/cpque.h>

#include <queue>

static const uint32_t seed = 1234;

void std_test()
{
    stc64_t rng;
    int N = 10000000, M = 10;

    std::priority_queue<float, std::vector<float>, std::greater<float>> pq;
    rng = stc64_new(seed);
    clock_t start = clock();
    c_forrange (i, N)
        pq.push((float) stc64_randf(&rng)*100000);

    printf("Built priority queue: %f secs\n", (clock() - start) / (float) CLOCKS_PER_SEC);
    printf("%g ", pq.top());

    start = clock();
    c_forrange (i, N) {
        pq.pop();
    }

    printf("\npopped PQ: %f secs\n\n", (clock() - start) / (float) CLOCKS_PER_SEC);
}


void stc_test()
{
    stc64_t rng;
    int N = 10000000, M = 10;

    c_auto (cpque_f, pq)
    {
        rng = stc64_new(seed);
        clock_t start = clock();
        c_forrange (i, N)
            cpque_f_push(&pq, (float) stc64_randf(&rng)*100000);

        printf("Built priority queue: %f secs\n", (clock() - start) / (float) CLOCKS_PER_SEC);
        printf("%g ", *cpque_f_top(&pq));
 
        c_forrange (i, int, M) {
            cpque_f_pop(&pq);
        }

        start = clock();
        c_forrange (i, int, M, N)
            cpque_f_pop(&pq);
        printf("\npopped PQ: %f secs\n", (clock() - start) / (float) CLOCKS_PER_SEC);
    }
}


int main()
{
    puts("STD P.QUEUE:");
    std_test();
    puts("\nSTC P.QUEUE:");
    stc_test();
}
