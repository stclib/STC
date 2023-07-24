// printspan.c

#include <stdio.h>
#define i_implement
#include <stc/cstr.h>
#define i_key int
#include <stc/cvec.h>
#define i_key int
#include <stc/cstack.h>
#define i_key_str
#include <stc/csset.h>

#include <stc/cspan.h>
using_cspan(intspan, int);

void printMe(intspan container) {
    printf("%d:", (int)cspan_size(&container));
    c_foreach (e, intspan, container)
        printf(" %d", *e.ref);
    puts("");
}

int main(void)
{
    intspan sp1 = cspan_init(intspan, {1, 2});
    printMe( sp1 );

    printMe( c_init(intspan, {1, 2, 3}) );

    int arr[] = {1, 2, 3, 4, 5, 6};
    intspan sp2 = cspan_from_array(arr);
    printMe( c_LITERAL(intspan)cspan_subspan(&sp2, 1, 4) );

    cvec_int vec = c_init(cvec_int, {1, 2, 3, 4, 5});
    printMe( c_LITERAL(intspan)cspan_from(&vec) );

    printMe( sp2 );

    cstack_int stk = c_init(cstack_int, {1, 2, 3, 4, 5, 6, 7});
    printMe( c_LITERAL(intspan)cspan_from(&stk) );

    csset_str set = c_init(csset_str, {"5", "7", "4", "3", "8", "2", "1", "9", "6"});
    printf("%d:", (int)csset_str_size(&set));
    c_foreach (e, csset_str, set)
        printf(" %s", cstr_str(e.ref));
    puts("");

    // cleanup
    cvec_int_drop(&vec);
    cstack_int_drop(&stk);
    csset_str_drop(&set);
}
