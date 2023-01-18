// printspan.c

#include <stdio.h>
#define i_val int
#include <stc/cvec.h>
#define i_val int
#include <stc/cstack.h>
#define i_val int
#include <stc/cdeq.h>
#include <stc/cspan.h>

using_cspan(ispan, int, 1);

void printMe(ispan container) {
    printf("%d\n", (int)cspan_size(&container));
    c_FOREACH (e, ispan, container) printf("%d ", *e.ref);
    puts("");
}

int main() {
    c_AUTO (cvec_int, vec)
    c_AUTO (cstack_int, stk)
    c_AUTO (cdeq_int, deq)
    {
        int arr[] = {1, 2, 3, 4, 5};
        ispan sp = cspan_from_array(arr);
        printMe((ispan)cspan_subspan(&sp, 1, 3));

        printMe((ispan)cspan_from_list(int, {1, 2, 3, 4}));

        printMe((ispan)cspan_from_array(arr));

        c_FORLIST (i, int, {1, 2, 3, 4, 5, 6}) cvec_int_push(&vec, *i.ref);
        printMe((ispan)cspan_from(&vec));

        c_FORLIST (i, int, {1, 2, 3, 4, 5, 6, 7}) cstack_int_push(&stk, *i.ref);
        printMe((ispan)cspan_from(&stk));

        c_FORLIST (i, int, {1, 2, 3, 4, 5, 6, 7, 8}) cdeq_int_push_front(&deq, *i.ref);
        printMe((ispan)cspan_from(&deq));
    }
}
