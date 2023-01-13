// Example from https://en.cppreference.com/w/cpp/container/mdspan
#define i_val int
#include <stc/cstack.h>
#include <stc/algo/cmspan.h>
#include <stdio.h>
using_cmspan(ispan1, int, 1);
using_cmspan(ispan2, int, 2);
using_cmspan(ispan3, int, 3);
 
int main()
{
  cstack_int v = {0};
  c_FORLIST (i, unsigned, {1,2,3,4,5,6,7,8,9,10,11,12})
    cstack_int_push(&v, *i.ref);

  // View data as contiguous memory representing 12 ints
  ispan1 ms1 = cmspan_make(v.data, 12);
  // View data as contiguous memory representing 2 rows of 6 ints each
  ispan2 ms2 = cmspan_make(v.data, 2, 6);
  // View the same data as a 3D array 2 x 3 x 2
  ispan3 ms3 = cmspan_make(v.data, 2, 3, 2);

  // write data using 2D view
  for(unsigned i=0; i != ms2.dim[0]; i++)
    for(unsigned j=0; j != ms2.dim[1]; j++)
      *cmspan_at(&ms2, i, j) = i*1000 + j;

  // print data using 1D view
  for(unsigned i=0; i != ms1.dim[0]; i++)
    printf(" %d", *cmspan_at(&ms1, i));
  puts("");

  c_FOREACH (i, ispan1, ms1)
    printf(" %d", *i.ref);
  puts("");

  // read back using 3D view
  for(unsigned i=0; i != ms3.dim[0]; i++)
  {
    printf("slice @ i = %u\n", i);
    for(unsigned j=0; j != ms3.dim[1]; j++)
    {
      for(unsigned k=0; k != ms3.dim[2]; k++)
        printf("%d ", *cmspan_at(&ms3, i, j, k));
      puts("");
    }
  }
  cstack_int_drop(&v);
}
