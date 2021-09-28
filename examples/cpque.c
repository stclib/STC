// Implements c++ example: https://en.cppreference.com/w/cpp/container/priority_queue
#include <stdio.h>
#include <stc/ccommon.h>

// Example of dynamic compare function

static int (*icmp_fn)(const int* x, const int* y);

#define i_val int
#define i_cnt ipque
#define i_cmp icmp_fn
#include <stc/cpque.h>

#define imix_less(x, y) ((*(x) ^ 1) < (*(y) ^ 1))
static int imax_cmp(const int* x, const int* y) { return *x - *y; }
static int imin_cmp(const int* x, const int* y) { return *y - *x; }
static int imix_cmp(const int* x, const int* y) { return c_less_compare(imix_less, x, y); }

void print_pque(ipque q) {
    ipque copy = ipque_clone(q);
    while (!ipque_empty(copy)) {
        printf("%d ", *ipque_top(&copy));
        ipque_pop(&copy);
    }
    puts("");
    ipque_del(&copy);
}

int main()
{
    const int data[] = {1,8,5,6,3,4,0,9,7,2}, n = c_arraylen(data);
    c_auto (ipque, q, q2, q3)  // init() and defered del()
    {
        icmp_fn = imax_cmp;
        c_apply_n(ipque, push, &q, data, n);
        print_pque(q);

        icmp_fn = imin_cmp;
        c_apply_n(ipque, push, &q2, data, n);
        print_pque(q2);

        icmp_fn = imix_cmp;
        c_apply_n(ipque, push, &q3, data, n);
        print_pque(q3);
    }
}