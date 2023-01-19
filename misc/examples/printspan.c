// printspan.c

#include <stdio.h>
#include <stc/cstr.h>
#define i_val int
#include <stc/cvec.h>
#define i_val int
#include <stc/cstack.h>
#define i_val int
#include <stc/cdeq.h>
#define i_val_str
#include <stc/cset.h>
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
    c_AUTO (cset_str, set)
    {
        int arr[] = {1, 2, 3, 4, 5};
        ispan sp = cspan_from_array(arr);
        printMe((ispan)cspan_subspan(&sp, 1, 3));

        printMe(c_initialize(ispan, {1, 2, 3, 4}));

        printMe((ispan)cspan_from_array(arr));

        c_FORLIST (i, int, {1, 2, 3, 4, 5, 6}) cvec_int_push(&vec, *i.ref);
        printMe((ispan)cspan_from(&vec));

        stk = c_initialize(cstack_int, {1, 2, 3, 4, 5, 6, 7});
        printMe((ispan)cspan_from(&stk));

        deq = c_initialize(cdeq_int, {1, 2, 3, 4, 5, 6, 7, 8});
        printMe((ispan)cspan_from(&deq));

        set = c_initialize(cset_str, {"1", "2", "3", "4", "5", "6", "7", "8", "9"});
        printf("%d\n", (int)cset_str_size(&set));
        c_FOREACH (e, cset_str, set) printf("%s ", cstr_str(e.ref));
        puts("");
    }
}
