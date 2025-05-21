#include <stdio.h>
#include <stc/cspan.h>
use_cspan3(Span, int);

int main(void)
{
    Span3 md3 = cspan_md(cspan_zeros(Span, 3*4*5).data, 3, 4, 5);
    for (c_range32(i, 3*4*5)) md3.data[i] = i;

    Span2 tmp2 = cspan_submd3(&md3, 2);
    cspan_transpose(&tmp2);
    Span last_col = cspan_submd2(&tmp2, 4);

    Span2 sliced = cspan_slice(&md3, Span2, {c_ALL}, {c_ALL}, {0});

    Span2 img = cspan_md(md3.data, 6, 10);
    Span2 half = {img.data, cspan_shape(img.shape[0]/2, img.shape[1]/2),
                            cspan_strides(img.stride.d[0]*2, img.stride.d[1]*2)};

    Span2 half_tr = Span2_transposed(half);

    puts("\n3D SPAN (md3):");
    cspan_print(Span3, "%d", md3);

    puts("\n3D SPAN LAST 1D COLUMN:");
    cspan_print(Span, "%d", last_col);

    puts("\n3D SPAN SLICED 2D:");
    cspan_print(Span2, "%d", sliced);

    puts("\nROWMAJOR (img):");
    cspan_print(Span2, "%d", img);

    puts("\nSTRIDED (half):");
    cspan_print(Span2, "%d", half);

    puts("\nCOLMAJOR (half):");
    cspan_print(Span2, "%d", half_tr);
}
