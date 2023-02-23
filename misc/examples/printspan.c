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
    c_foreach (e, intspan, container) printf(" %d", *e.ref);
    puts("");
}

int main()
{
    c_auto (cvec_int, vec)
    c_auto (cstack_int, stk)
    c_auto (cdeq_int, deq)
    c_auto (csset_str, set)
    {
        intspan sp1 = cspan_make(intspan, {1, 2});
        printMe( sp1 );
    
        printMe( c_make(intspan, {1, 2, 3}) );

        int arr[] = {1, 2, 3, 4, 5, 6};
        intspan sp2 = cspan_from_array(arr);
        printMe( (intspan)cspan_subspan(&sp2, 1, 4) );

        c_forlist (i, int, {1, 2, 3, 4, 5})
            cvec_int_push(&vec, *i.ref);
        printMe( (intspan)cspan_from(&vec) );

        printMe( sp2 );

        stk = c_make(cstack_int, {1, 2, 3, 4, 5, 6, 7});
        printMe( (intspan)cspan_from(&stk) );

        deq = c_make(cdeq_int, {1, 2, 3, 4, 5, 6, 7, 8});
        printMe( (intspan)cspan_from(&deq) );

        set = c_make(csset_str, {"5", "7", "4", "3", "8", "2", "1", "9", "6"});
        printf("%d:", (int)csset_str_size(&set));
        c_foreach (e, csset_str, set)
            printf(" %s", cstr_str(e.ref));
        puts("");
    }
}
