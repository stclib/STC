// Implements c++ example: https://en.cppreference.com/w/cpp/container/priority_queue
// Example of dynamic compare function

#include <stdio.h>
#include <stdbool.h>
#include <stc/forward.h>
#include <stc/views.h>

// predeclare
declare_cpque(ipque, int);

struct {
    ipque Q;
    bool (*less)(const int*, const int*);
} typedef IPQueue;

#define i_type ipque
#define i_val int
#define i_opt c_declared // avoid redeclaring container type
#define i_less_functor(self, x, y) c_container_of(self, IPQueue, Q)->less(x, y) // <== This.
#include <stc/cpque.h>

#define print(name, q, n) do { \
    printf("%s: \t", name); \
    c_forrange (i, n) printf("%d ", q[i]); \
    puts(""); \
} while(0)

void print_queue(const char* name, IPQueue q) {
    // NB: make a copy because there is no way to traverse
    // priority_queue's content without erasing the queue.
    IPQueue copy = {ipque_clone(q.Q), q.less};
    
    for (printf("%s: \t", name); !ipque_empty(&copy.Q); ipque_pop(&copy.Q))
        printf("%d ", *ipque_top(&copy.Q));
    puts("");
    ipque_drop(&copy.Q);
}

static bool int_less(const int* x, const int* y) { return *x < *y; }
static bool int_greater(const int* x, const int* y) { return *x > *y; }
static bool int_lambda(const int* x, const int* y) { return (*x ^ 1) < (*y ^ 1); }

int main()
{
    const int data[] = {1,8,5,6,3,4,0,9,7,2}, n = c_arraylen(data);
    print("data", data, n);

    IPQueue q1 = {ipque_init(), int_less};  // Max priority queue

    c_forrange (i, n)
        ipque_push(&q1.Q, data[i]);

    print_queue("q1", q1);

    // Min priority queue
    // std::greater<int> makes the max priority queue act as a min priority queue
    IPQueue minq1 = {ipque_init(), int_greater};
    c_forrange (i, n) ipque_push(&minq1.Q, data[i]);

    print_queue("minq1", minq1);

    // Using lambda to compare elements.
    IPQueue q5 = {ipque_init(), int_lambda};

    c_forrange (i, n)
        ipque_push(&q5.Q, data[i]);
    
    print_queue("q5", q5);

    c_drop(ipque, &q1.Q, &minq1.Q, &q5.Q);
}
