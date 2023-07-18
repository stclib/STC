// Implements c++ example: https://en.cppreference.com/w/cpp/container/priority_queue
// Example of per-instance less-function on a single priority queue type
//

#include <stdio.h>

#define i_type IPQue
#define i_base cpque
#define i_key int
#define i_extend bool(*less)(const int*, const int*);
#define i_less(x, y) c_extend()->less(x, y)
// Note: i_less: c_extend() accessible for cpque types
//       i_cmp: c_extend() accessible for csmap and csset types
//       i_hash/i_eq: c_extend() accessible for cmap and cset types
#include <stc/extend.h>

void print_queue(const char* name, IPQue_ext q) {
    // NB: make a clone because there is no way to traverse
    // priority queue's content without erasing the queue.
    IPQue_ext copy = {q.less, IPQue_clone(q.get)};
    
    for (printf("%s: \t", name); !IPQue_empty(&copy.get); IPQue_pop(&copy.get))
        printf("%d ", *IPQue_top(&copy.get));
    puts("");

    IPQue_drop(&copy.get);
}

static bool int_less(const int* x, const int* y) { return *x < *y; }
static bool int_greater(const int* x, const int* y) { return *x > *y; }
static bool int_lambda(const int* x, const int* y) { return (*x ^ 1) < (*y ^ 1); }

int main(void)
{
    const int data[] = {1,8,5,6,3,4,0,9,7,2}, n = c_arraylen(data);
    printf("data: \t");
    c_forrange (i, n) printf("%d ", data[i]);
    puts("");


    // Max priority queue
    IPQue_ext q1 = {.less=int_less};
    IPQue_put_n(&q1.get, data, n);
    print_queue("q1", q1);

    // Min priority queue
    IPQue_ext minq1 = {.less=int_greater};
    IPQue_put_n(&minq1.get, data, n);
    print_queue("minq1", minq1);

    // Using lambda to compare elements.
    IPQue_ext q5 = {.less=int_lambda};
    IPQue_put_n(&q5.get, data, n);
    print_queue("q5", q5);

    c_drop(IPQue, &q1.get, &minq1.get, &q5.get);
}
