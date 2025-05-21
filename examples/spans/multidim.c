// Example based on https://en.cppreference.com/w/cpp/container/mdspan
#include <stdio.h>
#define T Vec, int
#include <stc/stack.h>
#include <stc/cspan.h>

use_cspan3(ISpan, int); // define ISpan, ISpan2, ISpan3

int main(void)
{
    Vec vec = c_make(Vec, {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24});

    // Create 1d span from a compatibel container
    ISpan ms1 = cspan_from_vec(&vec);

    // Create a 3D mdspan 2 x 3 x 4
    ISpan3 ms3 = cspan_md(vec.data, 2, 3, 4);

    puts("ms3:");
    cspan_print(ISpan3, "%d", ms3);

    // Take a slice of md3
    ISpan3 ss3 = cspan_slice(&ms3, ISpan3, {c_ALL}, {1,3}, {1,3});
    puts("\nss3 = ms3[:, 1:3, 1:3]");
    cspan_print(ISpan3, "%d", ss3);

    puts("\nIterate ss3 flat:");
    for (c_each(i, ISpan3, ss3))
        printf(" %d", *i.ref);
    puts("");

    // submd3 span reduces rank depending on number of arguments
    ISpan2 ms2 = cspan_submd3(&ms3, 1);

    // Change data on the 2d subspan
    for (int i=0; i != ms2.shape[0]; i++)
        for (int j=0; j != ms2.shape[1]; j++)
            *cspan_at(&ms2, i, j) = (i + 1)*100 + j;

    puts("\nms2 = ms3[1] with updated data:");
    cspan_print(ISpan2, "%d", ms2);

    puts("\nOriginal s1 span with updated data:");
    for (c_each(i, ISpan, ms1))
        printf(" %d", *i.ref);
    puts("");

    puts("\nOriginal ms3 span with updated data:");
    cspan_print(ISpan3, "%d", ms3);

    puts("\ncol = ms3[1, :, 2]");
    ISpan col = cspan_slice(&ms3, ISpan, {1}, {c_ALL}, {2});
    for (c_each(i, ISpan, col))
        printf(" %d", *i.ref);
    puts("");

    Vec_drop(&vec);
}
