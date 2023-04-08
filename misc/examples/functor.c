// Implements c++ example: https://en.cppreference.com/w/cpp/container/priority_queue
// Example of per-instance less-function on a single priority queue type
//
// Note: i_less: has self for cpque types only
//       i_cmp: has self for csmap and csset types only
//       i_hash/i_eq: has self for cmap and cset types only

#include <stdio.h>

#define i_type IPQue
#define i_val int
#define i_extend bool (*less)(const int*, const int*);
#define i_less(x, y) c_getcon(self)->less(x, y)
#define i_con cpque
#include <stc/extend.h>

void print_queue(const char* name, IPQue_ext q) {
    // NB: make a clone because there is no way to traverse
    // priority_queue's content without erasing the queue.
    IPQue_ext copy = {q.less, IPQue_clone(q.get)};
    
    for (printf("%s: \t", name); !IPQue_empty(&copy.get); IPQue_pop(&copy.get))
        printf("%d ", *IPQue_top(&copy.get));
    puts("");

    IPQue_drop(&copy.get);
}

static bool int_less(const int* x, const int* y) { return *x < *y; }
static bool int_greater(const int* x, const int* y) { return *x > *y; }
static bool int_lambda(const int* x, const int* y) { return (*x ^ 1) < (*y ^ 1); }

int main()
{
    const int data[] = {1,8,5,6,3,4,0,9,7,2}, n = c_arraylen(data);
    printf("data: \t");
    c_forrange (i, n)
        printf("%d ", data[i]);
    puts("");

    IPQue_ext q1 = {int_less};       // Max priority queue
    IPQue_ext minq1 = {int_greater}; // Min priority queue
    IPQue_ext q5 = {int_lambda};     // Using lambda to compare elements.

    c_forrange (i, n)
        IPQue_push(&q1.get, data[i]);
    print_queue("q1", q1);

    c_forrange (i, n)
        IPQue_push(&minq1.get, data[i]);
    print_queue("minq1", minq1);

    c_forrange (i, n)
        IPQue_push(&q5.get, data[i]);
    print_queue("q5", q5);

    c_drop(IPQue, &q1.get, &minq1.get, &q5.get);
}
