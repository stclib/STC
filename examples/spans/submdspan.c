// https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2022/p2630r0.html
// C99:
#include <stdio.h>
#include <stc/cspan.h>

use_cspan(span2, double, 2);
use_cspan(span3, double, 3);

// Set all elements of a rank-2 mdspan to zero.
void zero_2d(span2 grid2d) {
    c_static_assert(cspan_rank(&grid2d) == 2);
    for (int i = 0; i < grid2d.shape[0]; ++i) {
        for (int j = 0; j < grid2d.shape[1]; ++j) {
            *cspan_at(&grid2d, i,j) = 0;
        }
    }
}

void zero_surface(span3 grid3d) {
    c_static_assert(cspan_rank(&grid3d) == 3);
    zero_2d(cspan_slice(&grid3d, span2, {0}, {c_ALL}, {c_ALL}));
    zero_2d(cspan_slice(&grid3d, span2, {c_ALL}, {0}, {c_ALL}));
    zero_2d(cspan_slice(&grid3d, span2, {c_ALL}, {c_ALL}, {0}));
    zero_2d(cspan_slice(&grid3d, span2, {grid3d.shape[0]-1}, {c_ALL}, {c_ALL}));
    zero_2d(cspan_slice(&grid3d, span2, {c_ALL}, {grid3d.shape[1]-1}, {c_ALL}));
    zero_2d(cspan_slice(&grid3d, span2, {c_ALL}, {c_ALL}, {grid3d.shape[2]-1}));
}

int main(void) {
    double arr[4*4*5];
    for (c_range32(i, c_countof(arr)))
        arr[i] = i + i/77.0;

    span3 md = cspan_md(arr, 4, 4, 5);

    zero_surface(md);

    cspan_print(span3, "%.2f", md);
    puts("done");
}
