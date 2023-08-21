// https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2022/p2630r0.html
// C99:
#include <stdio.h>
#include <stc/cspan.h>

using_cspan3(span, double); // shorthand for defining span, span2, span3

// Set all elements of a rank-2 mdspan to zero.
void zero_2d(span2 grid2d) {
  (void)c_static_assert(cspan_rank(&grid2d) == 2);
  for (int i = 0; i < grid2d.shape[0]; ++i) {
    for (int j = 0; j < grid2d.shape[1]; ++j) {
      *cspan_at(&grid2d, i,j) = 0;
    }
  }
}

void zero_surface(span3 grid3d) {
  (void)c_static_assert(cspan_rank(&grid3d) == 3);
  zero_2d(cspan_slice(span2, &grid3d, {0}, {c_ALL}, {c_ALL}));
  zero_2d(cspan_slice(span2, &grid3d, {c_ALL}, {0}, {c_ALL}));
  zero_2d(cspan_slice(span2, &grid3d, {c_ALL}, {c_ALL}, {0}));
  zero_2d(cspan_slice(span2, &grid3d, {grid3d.shape[0]-1}, {c_ALL}, {c_ALL}));
  zero_2d(cspan_slice(span2, &grid3d, {c_ALL}, {grid3d.shape[1]-1}, {c_ALL}));
  zero_2d(cspan_slice(span2, &grid3d, {c_ALL}, {c_ALL}, {grid3d.shape[2]-1}));
}

int main() {
  double arr[3*4*5];
  for (int i=0; i<c_arraylen(arr); ++i) arr[i] = i + 1.0;

  span3 md = cspan_md(arr, 3, 4, 5);

  zero_surface(md);

  for (int i = 0; i < md.shape[0]; i++) {
    for (int j = 0; j < md.shape[1]; j++) {
      for (int k = 0; k < md.shape[2]; k++)
        printf(" %2g", *cspan_at(&md, i,j,k));
      puts("");
    }
    puts("");
  }
}
