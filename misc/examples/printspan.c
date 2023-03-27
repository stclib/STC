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
#include <stc/csset.h>
#include <stc/cspan.h>

using_cspan(intspan, int, 1);

void printMe(intspan container) {
    printf("%d:", (int)cspan_size(&container));
    c_foreach (e, intspan, container)
        printf(" %d", *e.ref);
    puts("");
}

int main()
{
    intspan sp1 = cspan_make(intspan, {1, 2});
    printMe( sp1 );

    printMe( c_make(intspan, {1, 2, 3}) );

    int arr[] = {1, 2, 3, 4, 5, 6};
    intspan sp2 = cspan_from_array(arr);
    printMe( (intspan)cspan_subspan(&sp2, 1, 4) );

    cvec_int vec = c_make(cvec_int, {1, 2, 3, 4, 5});
    printMe( (intspan)cspan_from(&vec) );

    printMe( sp2 );

    cstack_int stk = c_make(cstack_int, {1, 2, 3, 4, 5, 6, 7});
    printMe( (intspan)cspan_from(&stk) );

    cdeq_int deq = c_make(cdeq_int, {1, 2, 3, 4, 5, 6, 7, 8});
    printMe( (intspan)cspan_from(&deq) );

    csset_str set = c_make(csset_str, {"5", "7", "4", "3", "8", "2", "1", "9", "6"});
    printf("%d:", (int)csset_str_size(&set));
    c_foreach (e, csset_str, set)
        printf(" %s", cstr_str(e.ref));
    puts("");

    // cleanup
    cvec_int_drop(&vec);
    cstack_int_drop(&stk);
    cdeq_int_drop(&deq);
    csset_str_drop(&set);
}
