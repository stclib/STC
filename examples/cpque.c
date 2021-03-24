#include <stc/cpque.h>
#include <stdio.h>

// Implements c++ example: https://en.cppreference.com/w/cpp/container/priority_queue

using_cvec(i, int);
using_cpque(imax, cvec_i);
using_cpque(imin, cvec_i, >);

#define myless(left, right) ((left ^ 1) < (right ^ 1))
static int cmp(const int *x, const int *y) { return myless(*y, *x) - myless(*x, *y); }
using_cpque(imix, cvec_i, <, cmp);

#define PRINT_Q(Q) \
    void print_##Q(Q q) { \
        Q copy = Q##_clone(q); \
        while (!Q##_empty(copy)) { \
            printf("%d ", *Q##_top(&copy)); \
            Q##_pop(&copy); \
        } \
        puts(""); \
        Q##_del(&copy); \
    }

PRINT_Q(cpque_imin)
PRINT_Q(cpque_imax)
PRINT_Q(cpque_imix)


int main() {
    cpque_imax q = cpque_imax_init();

    const int data[] = {1,8,5,6,3,4,0,9,7,2};

    c_forrange (n, c_arraylen(data))
        cpque_imax_push(&q, n);

    print_cpque_imax(q);

    cpque_imin q2 = cpque_imin_init(); 
    cpque_imin_emplace_n(&q2, data, c_arraylen(data));

    print_cpque_imin(q2);

    // Using myless cmp to compare elements.
    cpque_imix q3 = cpque_imix_init(); 

    c_forrange (n, c_arraylen(data))
        cpque_imix_push(&q3, n);

    print_cpque_imix(q3);

    cpque_imin_del(&q);
    cpque_imax_del(&q2);
    cpque_imix_del(&q3);
}