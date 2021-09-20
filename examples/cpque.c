// Implements c++ example: https://en.cppreference.com/w/cpp/container/priority_queue
#include <stdio.h>

#define i_tag imax
#define i_val int
#include <stc/cpque.h>

#define i_tag imin
#define i_val int
#define i_cmp -c_default_compare
#include <stc/cpque.h>

#define imix_less(x, y) ((*(x) ^ 1) < (*(y) ^ 1))
#define i_tag imix
#define i_val int
#define i_cmp(x, y) c_less_compare(imix_less, x, y)
#include <stc/cpque.h>

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

    c_auto (cpque_imax, q)  // init() and defered del()
    c_auto (cpque_imin, q2)
    c_auto (cpque_imix, q3)
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