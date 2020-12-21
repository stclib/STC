#include <stdio.h>
#include <time.h>
#include <deque>
#include <stc/cdeq.h>
#include <stc/crand.h>

enum {N = 200000000, M = 10000, P = 5000, R = 100};
using_cdeq(i, int);

void test1() {
    clock_t t1 = clock(), t2, t3;
    crand_t rng = crand_init(0);
    std::deque<int> deq;
    for (size_t i = 1; i < N; i++) {
        deq.push_front(crand_next(&rng));
        if (i % M == 0)
            for (int j = 0; j < P; j++)
                deq.pop_back();
    }
    size_t n = deq.size();
    t2 = clock();
    printf("std::deque push/pop: %5.2f sec, sz=%zu", (float)(t2 - t1) / CLOCKS_PER_SEC, n);
    fflush(stdout);
    size_t sum = 0;
    c_forrange (R) c_forrange (i, n)
        sum += deq[i];
    t3 = clock();
    printf(" sum: %5.2f sec, val=%zu\n", (float)(t3 - t2) / CLOCKS_PER_SEC, sum);
}

void test2() {
    clock_t t1 = clock(), t2, t3;
    crand_t rng = crand_init(0);
    cdeq_i deq = cdeq_inits;
    for (size_t i = 1; i < N; i++) {
        cdeq_i_push_front(&deq, crand_next(&rng));
        if (i % M == 0)
            for (int j = 0; j < P; j++)
                cdeq_i_pop_back(&deq);
    }
    size_t n = cdeq_i_size(deq);
    t2 = clock();
    printf("stc/cdeq_i push/pop: %5.2f sec, sz=%zu", (float)(t2 - t1) / CLOCKS_PER_SEC, n);
    size_t sum = 0;
    c_forrange (R) c_forrange (i, n)
        sum += deq.data[i];
    t3 = clock();
    printf(" sum: %5.2f sec, val=%zu\n", (float)(t3 - t2) / CLOCKS_PER_SEC, sum);
}

int main()
{
    test1();
    test2();
}
