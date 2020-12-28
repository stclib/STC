
#include <deque>
#include <vector>
#include <algorithm>
#include <iostream>
#include <ctime>
#include <stc/cmap.h>
#include <stc/cvec.h>
#include <stc/cdeq.h>
#include <stc/cstr.h>
#include <stc/crand.h>

typedef struct {const char* first; int second;} Si;
using_cvec(si, Si, c_default_del, c_no_compare);
cvec_si tm = cvec_inits;

void add(cvec_si* tm, const char* s, int n) { Si si = {s, n}; cvec_si_push_back(tm, si); }


void test_vector(const int num_iterations)
{
    std::vector<int> v;
    stc64_t rng = stc64_init(0);
    v.reserve(num_iterations + 2); //Ensure there is enough space reserved.

    // == PUSH_BACK
    {
        clock_t t1 = std::clock();

        for (int i=0; i<(num_iterations>>1); ++i)
        {
            v.push_back(stc64_rand(&rng));
        }

        clock_t t2 = std::clock();
        add(&tm, "vector::push_back", t2 - t1);
        printf("#");
    }

    // == INSERT FRONT
    {
        clock_t t1 = std::clock();

        for (int i=0; i<(num_iterations>>1); ++i)
        {
            // Rather add some more elements to back.
            v.push_back(stc64_rand(&rng));
        }

        clock_t t2 = std::clock();
        add(&tm, "vector::push_front", t2 - t1);
        printf("#");
    }

    // == SORT
    {/*
        clock_t t1 = std::clock();

        std::sort(v.begin(), v.end());

        clock_t t2 = std::clock();
        add(&tm, "vector::sort", t2 - t1);
        printf("#");*/
    }

    // == ITERATE
    {
        clock_t t1 = std::clock();

        for (int i=0; i<num_iterations; ++i)
        {
            v[i] = 2;
        }

        clock_t t2 = std::clock();
        add(&tm, "vector::iterate", t2 - t1);
        printf("#");
    }

    // == POP BACK
    {
        clock_t t1 = std::clock();
        volatile int number_sink = 0;

        for (int i=0; i<(num_iterations>>1); ++i)
        {
            number_sink += v.back();
            v.pop_back();
        }
        clock_t t2 = std::clock();
        add(&tm, "vector::pop_back", t2 - t1);
        printf("#");
    }
}

void test_deque(const int num_iterations)
{
    std::deque<int> d;
    stc64_t rng = stc64_init(0);

    // == PUSH_BACK
    {
        clock_t t1 = std::clock();

        for (int i=0; i<(num_iterations>>1); ++i)
        {
            d.push_back(stc64_rand(&rng));
        }

        clock_t t2 = std::clock();
        add(&tm, "deque::push_back", t2 - t1);
        printf("#");
    }

    // == INSERT FRONT
    {
        clock_t t1 = std::clock();

        for (int i=0; i<(num_iterations>>1); ++i)
        {
            d.push_front(stc64_rand(&rng));
        }

        clock_t t2 = std::clock();
        add(&tm, "deque::push_front", t2 - t1);
        printf("#");
    }

    // == SORT
    {/*
        clock_t t1 = std::clock();

        std::sort(d.begin(), d.end());

        clock_t t2 = std::clock();
        add(&tm, "deque::sort", t2 - t1);
        printf("#");*/
    }

    // == ITERATE
    {
        clock_t t1 = std::clock();

        for (int i=0; i<num_iterations; ++i)
        {
            d[i] = 2;
        }

        clock_t t2 = std::clock();
        add(&tm, "deque::iterate", t2 - t1);
        printf("#");
    }

    // == POP BACK
    {
        clock_t t1 = std::clock();
        volatile int number_sink = 0;

        for (int i=0; i<(num_iterations>>1); ++i)
        {
            number_sink += d.back();
            d.pop_back();
        }

        clock_t t2 = std::clock();
        add(&tm, "deque::pop_back", t2 - t1);
        printf("#");
    }
}




using_cvec(i, int);


void test_cvec(const int num_iterations)
{
    cvec_i v = cvec_inits;
    stc64_t rng = stc64_init(0);
    //v.reserve(num_iterations + 2); //Ensure there is enough space reserved.

    // == PUSH_BACK
    {
        clock_t t1 = std::clock();

        for (int i=0; i<(num_iterations>>1); ++i)
        {
            cvec_i_push_back(&v, stc64_rand(&rng));
        }

        clock_t t2 = std::clock();
        add(&tm, "cvec_push_back", t2 - t1);
        printf("#");
    }

    // == INSERT FRONT
    {
        clock_t t1 = std::clock();

        for (int i=0; i<(num_iterations>>1); ++i)
        {
            // Rather add some more elements to back.
            cvec_i_push_back(&v, stc64_rand(&rng));
        }

        clock_t t2 = std::clock();
        add(&tm, "cvec_push_front", t2 - t1);
        printf("#");
    }

    // == SORT
    {/*
        clock_t t1 = std::clock();

        cvec_i_sort(&v);

        clock_t t2 = std::clock();
        add(&tm, "cvec_sort", t2 - t1);
        printf("#");*/
    }

    // == ITERATE
    {
        clock_t t1 = std::clock();

        for (int i=0; i<num_iterations; ++i)
        {
            v.data[i] = 2;
        }

        clock_t t2 = std::clock();
        add(&tm, "cvec_iterate", t2 - t1);
        printf("#");
    }

    // == POP BACK
    {
        clock_t t1 = std::clock();
        volatile int number_sink = 0;

        for (int i=0; i<(num_iterations>>1); ++i)
        {
            number_sink += *cvec_i_back(&v);
            cvec_i_pop_back(&v);
        }
        clock_t t2 = std::clock();
        add(&tm, "cvec_pop_back", t2 - t1);
        printf("#");
    }
    cvec_i_del(&v);
}


using_cdeq(i, int);

void test_cdeq(const int num_iterations)
{
    cdeq_i d = cdeq_i_with_capacity(num_iterations + 2);
    stc64_t rng = stc64_init(0);

    // == PUSH_BACK
    {
        clock_t t1 = std::clock();

        for (int i=0; i<(num_iterations>>1); ++i)
        {
            cdeq_i_push_back(&d, stc64_rand(&rng));
        }

        clock_t t2 = std::clock();
        add(&tm, "cdeq_push_back", t2 - t1);
        printf("#");
    }

    // == INSERT FRONT
    {
        clock_t t1 = std::clock();

        for (int i=0; i<(num_iterations>>1); ++i)
        {
            cdeq_i_push_front(&d, stc64_rand(&rng));
        }

        clock_t t2 = std::clock();
        add(&tm, "cdeq_push_front", t2 - t1);
        printf("#");
    }

    // == SORT
    {/*
        clock_t t1 = std::clock();

        std::sort(d.begin(), d.end());

        clock_t t2 = std::clock();
        add(&tm, "deque_sort", t2 - t1);
        printf("#");*/
    }

    // == ITERATE
    {
        clock_t t1 = std::clock();

        for (int i=0; i<num_iterations; ++i)
        {
            d.data[i] = 2;
        }

        clock_t t2 = std::clock();
        add(&tm, "cdeq_iterate", t2 - t1);
        printf("#");
    }

    // == POP BACK
    {
        clock_t t1 = std::clock();
        volatile int number_sink = 0;

        for (int i=0; i<(num_iterations>>1); ++i)
        {
            number_sink += *cdeq_i_back(&d);
            cdeq_i_pop_back(&d);
        }

        clock_t t2 = std::clock();
        add(&tm, "cdeq_pop_back", t2 - t1);
        printf("#");
    }
    cdeq_i_del(&d);
}



int main(void)
{
    int num_iterations = 500000000; // 100M elements * 4 bytes.
    num_iterations = (num_iterations >> 1) << 1; // Make sure num_iterations is even.

    std::cout << "Doing tests..." << std::endl;

    test_vector(num_iterations);
    test_deque(num_iterations);

    test_cvec(num_iterations);
    test_cdeq(num_iterations);

    std::cout << "done.\n";

    c_foreach (i, cvec_si, tm) {
        std::cout << i.ref->first << ": " << ((float) i.ref->second / CLOCKS_PER_SEC) << std::endl;
    }
    cvec_si_del(&tm);
    return 0;
}