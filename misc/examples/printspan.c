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

using_cspan(intspan, int, 1);

void printMe(intspan container) {
    printf("%d:", (int)cspan_size(&container));
    c_FOREACH (e, intspan, container) printf(" %d", *e.ref);
    puts("");
}

int main()
{
    c_AUTO (cvec_int, vec)
    c_AUTO (cstack_int, stk)
    c_AUTO (cdeq_int, deq)
    c_AUTO (cset_str, set)
    {
        printMe( c_init(intspan, {1, 2, 3}) );

        int arr[] = {1, 2, 3, 4, 5, 6};
        intspan span = cspan_from_array(arr);
        printMe( (intspan)cspan_subspan(&span, 1, 4) );

        c_FORLIST (i, int, {1, 2, 3, 4, 5})
            cvec_int_push(&vec, *i.ref);
        printMe( (intspan)cspan_from(&vec) );

        printMe( span );

        stk = c_init(cstack_int, {1, 2, 3, 4, 5, 6, 7});
        printMe( (intspan)cspan_from(&stk) );

        deq = c_init(cdeq_int, {1, 2, 3, 4, 5, 6, 7, 8});
        printMe( (intspan)cspan_from(&deq) );

        set = c_init(cset_str, {"1", "2", "3", "4", "5", "6", "7", "8", "9"});
        printf("%d:", (int)cset_str_size(&set));
        c_FOREACH (e, cset_str, set)
            printf(" %s", cstr_str(e.ref));
        puts("");
    }
}
