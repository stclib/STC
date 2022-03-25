// Implements c++ example: https://en.cppreference.com/w/cpp/container/priority_queue
#include <stdio.h>

// Example of dynamic compare function

static int (*icmp_fn)(const int* x, const int* y);

#define i_val int
#define i_cmp icmp_fn
#define i_type ipque
#include <stc/cpque.h>

#define imix_less(left, right) ((*(left) ^ 1) < (*(right) ^ 1))
static int imax_cmp(const int* x, const int* y) { return *x - *y; }
static int imin_cmp(const int* x, const int* y) { return *y - *x; }
static int imix_cmp(const int* x, const int* y) { return c_less_cmp(imix_less, x, y); }

void print_queue(ipque q) {
    ipque copy = ipque_clone(q);
    while (!ipque_empty(copy)) {
        printf("%d ", *ipque_top(&copy));
        ipque_pop(&copy);
    }
    puts("");
    ipque_drop(&copy);
}

int main()
{
    const int data[] = {1,8,5,6,3,4,0,9,7,2}, n = c_arraylen(data);
    c_auto (ipque, q, q2, q3)  // init() and defered drop()
    {
        icmp_fn = imax_cmp;
        c_forrange (i, n) 
            ipque_push(&q, data[i]);

        print_queue(q);

        icmp_fn = imin_cmp;
        c_apply_arr(v, ipque_push(&q2, *v), const int, data, n);
        print_queue(q2);

        icmp_fn = imix_cmp;
        c_apply_arr(v, ipque_push(&q3, *v), const int, data, n);
        print_queue(q3);
    }
}
