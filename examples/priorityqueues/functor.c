// Implements c++ example: https://en.cppreference.com/w/cpp/container/priority_queue
// Example of per-instance less-function on a single priority queue type
//

#include <stdio.h>

#define i_type IPQueue, int
#define i_aux bool(*less)(const int*, const int*);
#define i_less(x, y) self->aux.less(x, y)
#include "stc/pqueue.h"

void print_queue(const char* name, IPQueue q) {
    // Make a clone, because there is no way to traverse
    // priority queues ordered without erasing the queue.

    // NB! A clone function for the extended container struct is provided.
    // It assumes that the extended member(s) are POD/trivial type(s).
    IPQueue copy = IPQueue_clone(q);
    printf("%s: \t", name);
    while (!IPQueue_is_empty(&copy)) {
        printf("%d ", IPQueue_pull(&copy));
    }
    puts("");

    IPQueue_drop(&copy);
}

static bool int_less(const int* x, const int* y) { return *x < *y; }
static bool int_greater(const int* x, const int* y) { return *x > *y; }
static bool int_lambda(const int* x, const int* y) { return (*x ^ 1) < (*y ^ 1); }

int main(void)
{
    const int data[] = {1,8,5,6,3,4,0,9,7,2}, n = c_arraylen(data);
    printf("data: \t");
    for (c_range(i, n)) printf("%d ", data[i]);
    puts("");

    // Max priority queue
    IPQueue q1 = {.aux={.less=int_less}};
    IPQueue_put_n(&q1, data, n);
    print_queue("q1", q1);

    // Min priority queue
    IPQueue minq1 = {.aux={.less=int_greater}};
    IPQueue_put_n(&minq1, data, n);
    print_queue("minq1", minq1);

    // Using lambda to compare elements.
    IPQueue q5 = {.aux={.less=int_lambda}};
    IPQueue_put_n(&q5, data, n);
    print_queue("q5", q5);

    c_drop(IPQueue, &q1, &minq1, &q5);
}
