// Implements c++ example: https://en.cppreference.com/w/cpp/container/priority_queue
#include <stdio.h>
#include <stdbool.h>

// Example of dynamic compare function

static bool (*less_fn)(const int* x, const int* y);

#define i_val int
#define i_less less_fn
#define i_type ipque
#include <stc/cpque.h>

void print_queue(ipque q) {
    ipque copy = ipque_clone(q);
    while (!ipque_empty(&copy)) {
        printf("%d ", *ipque_top(&copy));
        ipque_pop(&copy);
    }
    puts("");
    ipque_drop(&copy);
}

static bool int_less(const int* x, const int* y) { return *x < *y; }
static bool int_greater(const int* x, const int* y) { return *x > *y; }
static bool int_lambda(const int* x, const int* y) { return (*x ^ 1) < (*y ^ 1); }

int main()
{
    const int data[] = {1,8,5,6,3,4,0,9,7,2}, n = c_arraylen(data);
    c_auto (ipque, q, q2, q3)  // init() and defered drop()
    {
        less_fn = int_less;
        c_forrange (i, n) 
            ipque_push(&q, data[i]);

        print_queue(q);

        less_fn = int_greater;
        c_apply_array(v, ipque_push(&q2, *v), const int, data, n);
        print_queue(q2);

        less_fn = int_lambda;
        c_apply_array(v, ipque_push(&q3, *v), const int, data, n);
        print_queue(q3);
    }
}
