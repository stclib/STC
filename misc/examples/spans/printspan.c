// https://www.modernescpp.com/index.php/c-20-std-span/

#include <stdio.h>
#define i_key int
#include <stc/cvec.h>

#define i_key int
#include <stc/cstack.h>

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
    printMe( c_init(intspan, {1, 2, 3, 4}) );

    int arr[] = {1, 2, 3, 4, 5};
    printMe( (intspan)cspan_from_array(arr) );

    cvec_int vec = c_init(cvec_int, {1, 2, 3, 4, 5, 6});
    printMe( (intspan)cspan_from(&vec) );

    cstack_int stk = c_init(cstack_int, {1, 2, 3, 4, 5, 6, 7});
    printMe( (intspan)cspan_from(&stk) );

    intspan spn = c_init(intspan, {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12});
    printMe( (intspan)cspan_subspan(&spn, 2, 8) );

    // cleanup
    cvec_int_drop(&vec);
    cstack_int_drop(&stk);
}
