#include <stdio.h>
#include <stc/cspan.h>
#include <stdlib.h>

using_cspan3(DSpan, double);

int main(void) {
    const int nx=5, ny=4, nz=3;
    double* data = c_new_n(double, nx*ny*nz);

    printf("\nMultidim span ms[5, 4, 3], fortran ordered");
    DSpan3 ms = cspan_md_order('F', data, nx, ny, nz); // Fortran, not 'C'

    int idx = 0;
    c_forrange (i, ms.shape[0])
        c_forrange (j, ms.shape[1])
            c_forrange (k, ms.shape[2]) 
                *cspan_at(&ms, i, j, k) = ++idx;

    cspan_transpose(&ms);

    printf(", transposed:\n\n");
    c_forrange (i, ms.shape[0]) {
        c_forrange (j, ms.shape[1]) {
            c_forrange (k, ms.shape[2])
                printf(" %3g", *cspan_at(&ms, i, j, k));
            puts("");
        }
        puts("");
    }

    DSpan2 sub;

    puts("Slicing:");
    printf("\nms[0, :, :] ");
    sub = cspan_slice(DSpan2, &ms, {0}, {c_ALL}, {c_ALL});
    c_foreach (i, DSpan2, sub) printf(" %g", *i.ref);
    puts("");

    printf("\nms[:, 0, :] ");
    sub = cspan_slice(DSpan2, &ms, {c_ALL}, {0}, {c_ALL});
    c_foreach (i, DSpan2, sub) printf(" %g", *i.ref);
    puts("");

    sub = cspan_slice(DSpan2, &ms, {c_ALL}, {c_ALL}, {0});
    printf("\nms[:, :, 0] ");
    c_foreach (i, DSpan2, sub) printf(" %g", *i.ref);
    puts("");

    free(data);
}
