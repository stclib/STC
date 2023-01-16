// Example based on https://en.cppreference.com/w/cpp/container/mdspan
#define i_val int
#include <stc/cstack.h>
#include <stc/cspan.h>
#include <stdio.h>

using_cspan3(ispan, int);
/*
using_cspan(ispan1, int, 1);
using_cspan(ispan2, int, 2);
using_cspan(ispan3, int, 3);
*/

int main()
{
  cstack_int v = {0};
  c_FORLIST (i, unsigned, {1,2,3,4,5,6,7,8,9,10,11,12})
    cstack_int_push(&v, *i.ref);

  // View data as contiguous memory representing 12 ints
  ispan1 ms1 = cspan_from(&v);
  // View data as contiguous memory representing 2 rows of 6 ints each
  ispan2 ms2 = cspan_make(v.data, 2, 6);
  // View the same data as a 3D array 2 x 3 x 2
  ispan3 ms3 = cspan_make(v.data, 2, 3, 2);

  // write data using 2D view
  for (unsigned i=0; i != ms2.dim[0]; i++)
    for (unsigned j=0; j != ms2.dim[1]; j++)
      *cspan_at(ms2, i, j) = i*1000 + j;

  // print all items using 1D view
  printf("all: ");
  for (unsigned i=0; i != ms1.dim[0]; i++)
    printf(" %d", *cspan_at(ms1, i));
  puts("");

  // or iterate a subspan...
  ispan2 sub = cspan_at3(ms3, 1);
  printf("sub: ");
  c_FOREACH (i, ispan2, sub)
    printf(" %d", *i.ref);
  puts("");

  // read back using 3D view
  for (unsigned i=0; i != ms3.dim[0]; i++)
  {
    printf("slice @ i = %u\n", i);
    for (unsigned j=0; j != ms3.dim[1]; j++)
    {
      for (unsigned k=0; k != ms3.dim[2]; k++)
        printf("%d ", *cspan_at(ms3, i, j, k));
      puts("");
    }
  }
  cstack_int_drop(&v);
}
