#include <stdio.h>
#include <stdlib.h>
#include <stc/cspan.h>

use_cspan(DSpan2, double, 2);
use_cspan(DSpan3, double, 3);

int main(void) {
    const int nx=3, ny=4, nz=5;
    double* data = c_new_n(double, nx*ny*nz);

    printf("\nMultidim span ms[3, 4, 5], fortran ordered");
    DSpan3 ms = cspan_md_layout(c_COLMAJOR, data, nx, ny, nz);

    int idx = 0;
    for (int i = 0; i < ms.shape[0]; ++i)
        for (int j = 0; j < ms.shape[1]; ++j)
            for (int k = 0; k < ms.shape[2]; ++k)
                *cspan_at(&ms, i, j, k) = ++idx;

    cspan_transpose(&ms);

    printf(", transposed:\n\n");
    cspan_print(DSpan3, "%.2f", ms);

    puts("Slicing:");
    printf("ms[0, :, :]\n");
    cspan_print(DSpan2, "%g", cspan_slice(&ms, DSpan2, {0}, {c_ALL}, {c_ALL}));

    printf("ms[:, 0, :]\n");
    cspan_print(DSpan2, "%g", cspan_slice(&ms, DSpan2, {c_ALL}, {0}, {c_ALL}));

    printf("ms[:, :, 0]\n");
    cspan_print(DSpan2, "%g", cspan_slice(&ms, DSpan2, {c_ALL}, {c_ALL}, {0}));

    c_free_n(data, nx*ny*nz);
}
