// Example based on https://en.cppreference.com/w/cpp/container/mdspan
#define i_val int
#include <stc/cstack.h>
#define i_implement
#include <stc/cspan.h>
#include <stdio.h>

using_cspan3(ispan, int);

void print2d(ispan2 ms2) {
    for (int i=0; i < ms2.shape[0]; i++) {
        for (int j=0; j < ms2.shape[1]; j++)
            printf(" %3d", *cspan_at(&ms2, i, j));
        puts("");
    }
}

void print3d(ispan3 ms3) {
    for (int i=0; i < ms3.shape[0]; i++) {
        for (int j=0; j < ms3.shape[1]; j++) {
            for (int k=0; k < ms3.shape[2]; k++)
                printf(" %3d", *cspan_at(&ms3, i, j, k));
            puts("");
        }
        puts("");
    }
}

int main(void)
{
    cstack_int v = c_init(cstack_int, {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24});

    // Create 1d span from a compatibel container
    ispan ms1 = cspan_from(&v);

    // Create a 3D mdspan 2 x 3 x 4
    ispan3 ms3 = cspan_md(v.data, 2, 3, 4);

    puts("ms3:");
    print3d(ms3);

    // Take a slice of md3
    ispan3 ss3 = cspan_slice(ispan3, &ms3, {c_ALL}, {1,3}, {1,3});
    puts("ss3 = ms3[:, 1:3, 1:3]");
    print3d(ss3);

    puts("Iterate ss3 flat:");
    c_foreach (i, ispan3, ss3) printf(" %d", *i.ref);
    puts("");

    // submd3 span reduces rank depending on number of arguments
    ispan2 ms2 = cspan_submd3(&ms3, 1);

    // Change data on the 2d subspan
    for (int i=0; i != ms2.shape[0]; i++)
        for (int j=0; j != ms2.shape[1]; j++)
            *cspan_at(&ms2, i, j) = (i + 1)*100 + j;
    
    puts("\nms2 = ms3[1] with updated data:");    
    print2d(ms2);
    puts("");    

    puts("\nOriginal s1 span with updated data:");
    c_foreach (i, ispan, ms1) printf(" %d", *i.ref);
    puts("");

    puts("\nOriginal ms3 span with updated data:");
    print3d(ms3);

    puts("col = ms3[1, :, 2]");
    ispan col = cspan_slice(ispan, &ms3, {1}, {c_ALL}, {2});
    c_foreach (i, ispan, col) printf(" %d", *i.ref);
    puts("");

    cstack_int_drop(&v);
}
