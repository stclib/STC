#include <stc/cpque.h>
#include <stdio.h>

// Implements c++ example: https://en.cppreference.com/w/cpp/container/priority_queue

using_cvec(i, int);
#define imix_less(x, y) ((*(x) ^ 1) < (*(y) ^ 1))
#define imix_cmp(x, y) c_less_compare(imix_less, x, y)

using_cpque(imax, cvec_i);
using_cpque(imin, cvec_i, -c_default_compare);
using_cpque(imix, cvec_i, imix_cmp);

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


int main()
{
    const int data[] = {1,8,5,6,3,4,0,9,7,2};

    c_forvar (cpque_imax q = cpque_imax_init(), cpque_imax_del(&q))  // init() and defered del()
    c_forvar (cpque_imin q2 = cpque_imin_init(), cpque_imin_del(&q2))
    c_forvar (cpque_imix q3 = cpque_imix_init(), cpque_imix_del(&q3))
    {
        c_forrange (n, c_arraylen(data))
            cpque_imax_push(&q, n);

        print_cpque_imax(q);

        cpque_imin_emplace_items(&q2, data, c_arraylen(data));
        print_cpque_imin(q2);

        // Using imix_less to compare elements.
        c_forrange (n, c_arraylen(data))
            cpque_imix_push(&q3, n);

        print_cpque_imix(q3);
    }
}