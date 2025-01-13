// Example based on https://en.cppreference.com/w/cpp/container/mdspan
#include <stdio.h>
#define i_key int
#include "stc/stack.h"
#include "stc/cspan.h"

using_cspan3(ISpan, int); // define ISpan, ISpan2, ISpan3

int main(void)
{
    stack_int v = c_make(stack_int, {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24});

    // Create 1d span from a compatibel container
    ISpan ms1 = cspan_from(&v);

    // Create a 3D mdspan 2 x 3 x 4
    ISpan3 ms3 = cspan_md(v.data, 2, 3, 4);

    puts("ms3:");
    cspan_print(ISpan3, ms3, "%d");

    // Take a slice of md3
    ISpan3 ss3 = cspan_slice(ISpan3, &ms3, {c_ALL}, {1,3}, {1,3});
    puts("\nss3 = ms3[:, 1:3, 1:3]");
    cspan_print(ISpan3, ss3, "%d");

    puts("\nIterate ss3 flat:");
    for (c_each(i, ISpan3, ss3)) printf(" %d", *i.ref);
    puts("");

    // submd3 span reduces rank depending on number of arguments
    ISpan2 ms2 = cspan_submd3(&ms3, 1);

    // Change data on the 2d subspan
    for (int i=0; i != ms2.shape[0]; i++)
        for (int j=0; j != ms2.shape[1]; j++)
            *cspan_at(&ms2, i, j) = (i + 1)*100 + j;

    puts("\nms2 = ms3[1] with updated data:");
    cspan_print(ISpan2, ms2, "%d");

    puts("\nOriginal s1 span with updated data:");
    for (c_each(i, ISpan, ms1)) printf(" %d", *i.ref);
    puts("");

    puts("\nOriginal ms3 span with updated data:");
    cspan_print(ISpan3, ms3, "%d");

    puts("\ncol = ms3[1, :, 2]");
    ISpan col = cspan_slice(ISpan, &ms3, {1}, {c_ALL}, {2});
    for (c_each(i, ISpan, col)) printf(" %d", *i.ref);
    puts("");

    stack_int_drop(&v);
}
