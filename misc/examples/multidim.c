// Example based on https://en.cppreference.com/w/cpp/container/mdspan
#define i_val int
#include <stc/cstack.h>
#include <stc/cspan.h>
#include <stdio.h>

using_cspan3(ispan, int);

int main()
{
    cstack_int v = c_make(cstack_int, {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24});

    // View data as contiguous memory representing 24 ints
    ispan ms1 = cspan_from(&v);

    // View the same data as a 3D array 2 x 3 x 4
    ispan3 ms3 = cspan_md(v.data, 2, 3, 4);

    puts("ms3:");
    for (int i=0; i != ms3.shape[0]; i++) {
        for (int j=0; j != ms3.shape[1]; j++) {
            for (int k=0; k != ms3.shape[2]; k++) {
                printf(" %2d", *cspan_at(&ms3, i, j, k));
            }
            puts("");
        }
        puts("");
    }
    puts("ss3 = ms3[:, 1:3, 1:3]");
    ispan3 ss3 = ms3;
    //cspan_slice(&ss3, {c_ALL}, {1,3}, {1,3});
    ss3 = cspan_slice(ispan3, &ms3, {c_ALL}, {1,3}, {1,3});

    for (int i=0; i != ss3.shape[0]; i++) {
        for (int j=0; j != ss3.shape[1]; j++) {
            for (int k=0; k != ss3.shape[2]; k++) {
                printf(" %2d", *cspan_at(&ss3, i, j, k));
            }
            puts("");
        }
        puts("");
    }

    puts("Iterate ss3 flat:");
    c_foreach (i, ispan3, ss3)
        printf(" %d", *i.ref);
    puts("");

    ispan2 ms2 = cspan_submd3(&ms3, 0);

    // write data using 2D view
    for (int i=0; i != ms2.shape[0]; i++)
        for (int j=0; j != ms2.shape[1]; j++)
            *cspan_at(&ms2, i, j) = i*1000 + j;

    puts("\nview data as 1D view:");
    for (int i=0; i != cspan_size(&ms1); i++)
        printf(" %d", *cspan_at(&ms1, i));
    puts("");

    puts("iterate subspan ms3[1]:");
    ispan2 sub = cspan_submd3(&ms3, 1);
    c_foreach (i, ispan2, sub)
        printf(" %d", *i.ref);
    puts("");

    cstack_int_drop(&v);
}
