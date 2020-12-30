#include <stdio.h>
#include <time.h>
#include <deque>
#include <vector>
#include <stc/cdeq.h>
#include <stc/crand.h>

enum {N = 1000000000, M = 12345, P = 5000, R = 2000};
using_cdeq(i, int);

void test1() {
    clock_t t1 = clock(), t2, t3;
    stc64_t rng = stc64_init(0);
    {
        std::deque<int> deq;
        for (size_t i = 1; i < N; i++) {
            deq.push_front(stc64_rand(&rng));
            if (i % M == 0)
                for (int j = 5; j < M; j++)
                    deq.pop_back();
        }
        size_t n = deq.size();
        t2 = clock();
        printf("std pushf/popb  : %5.2f sec, sz=%zu\n", (float)(t2 - t1) / CLOCKS_PER_SEC, n);
        fflush(stdout);
        size_t sum = 0;
        c_forrange (R) c_forrange (i, n)
            sum += deq[i];
        t3 = clock();
        printf("std access      : %5.2f sec, sum=%zu\n", (float)(t3 - t2) / CLOCKS_PER_SEC, sum);
     }{
        std::deque<int> deq;
        for (size_t i = 1; i < N/10; i++) {
            if (i & 1) deq.push_front(stc64_rand(&rng));
            else deq.push_back(stc64_rand(&rng));
        }
        t2 = clock();
        printf("std pushf/pushb : %5.2f sec\n", (float)(t2 - t3) / CLOCKS_PER_SEC);
    }{
        std::deque<int> deq;
        for (size_t i = 1; i < N/2; i++) {
            deq.push_back(stc64_rand(&rng));
        }
        t3 = clock();
        printf("std pushb       : %5.2f sec\n", (float)(t3 - t2) / CLOCKS_PER_SEC);
    }    
}


void test2() {
    clock_t t1 = clock(), t2, t3;
    stc64_t rng = stc64_init(0);
    {
        cdeq_i deq = cdeq_inits;
        for (size_t i = 1; i < N; i++) {
            cdeq_i_push_front(&deq, stc64_rand(&rng));
            if (i % M == 0)
                for (int j = 5; j < M; j++)
                    cdeq_i_pop_back(&deq);
        }
        size_t n = cdeq_i_size(deq);
        t2 = clock();
        printf("stc pushf/popb  : %5.2f sec, sz=%zu\n", (float)(t2 - t1) / CLOCKS_PER_SEC, n);
        size_t sum = 0;
        c_forrange (R) c_forrange (i, n)
            sum += deq.data[i];
        t3 = clock();
        printf("stc access      : %5.2f sec, sum=%zu\n", (float)(t3 - t2) / CLOCKS_PER_SEC, sum);
        cdeq_i_del(&deq);
    }{
        cdeq_i deq = cdeq_inits;
        for (size_t i = 1; i < N/10; i++) {
            if (i & 1) cdeq_i_push_front(&deq, stc64_rand(&rng));
            else cdeq_i_push_back(&deq, stc64_rand(&rng));
        }
        t2 = clock();
        printf("stc pushf/pushb : %5.2f sec\n", (float)(t2 - t3) / CLOCKS_PER_SEC);
        cdeq_i_del(&deq);
    }{
        cdeq_i deq = cdeq_inits;
        for (size_t i = 1; i < N/2; i++) {
            cdeq_i_push_back(&deq, stc64_rand(&rng));
        }
        t3 = clock();
        printf("stc pushb       : %5.2f sec\n", (float)(t3 - t2) / CLOCKS_PER_SEC);
        cdeq_i_del(&deq);
    }
}

int main()
{
    test1();
    puts("");
    test2();
}
