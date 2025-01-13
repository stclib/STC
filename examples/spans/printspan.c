// https://www.modernescpp.com/index.php/c-20-std-span/

#include <stdio.h>
#define i_key int
#include "stc/vec.h"
#define i_key int
#include "stc/stack.h"
#include "stc/cspan.h"

using_cspan(intspan, const int);


void printMe(intspan container) {
    printf("%d:", (int)cspan_size(&container));
    for (c_each(e, intspan, container))
        printf(" %d", *e.ref);
    puts("");
}


int main(void)
{
    printMe( c_make(intspan, {1, 2, 3, 4}) );

    int arr[] = {1, 2, 3, 4, 5};
    printMe( (intspan)cspan_from_array(arr) );

    vec_int vec = c_make(vec_int, {1, 2, 3, 4, 5, 6});
    printMe( (intspan)cspan_from(&vec) );

    stack_int stk = c_make(stack_int, {1, 2, 3, 4, 5, 6, 7});
    printMe( (intspan)cspan_from(&stk) );

    intspan spn = c_make(intspan, {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12});
    printMe( (intspan)cspan_subspan(&spn, 2, 8) );

    // cleanup
    vec_int_drop(&vec);
    stack_int_drop(&stk);
}
