// https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2023/p2642r2.html
// C99:
#include <stdio.h>
#include <time.h>
#include "stc/cspan.h"

using_cspan(Mat, double, 2);
using_cspan(Mat3, double, 3);
typedef Mat OutMat;
typedef struct { Mat m00, m01, m10, m11; } Partition;

Partition partition(Mat A)
{
  int M = A.shape[0];
  int N = A.shape[1];
  return (Partition){
    .m00 = cspan_slice(Mat, &A, {0, M/2}, {0, N/2}),
    .m01 = cspan_slice(Mat, &A, {0, M/2}, {N/2, N}),
    .m10 = cspan_slice(Mat, &A, {M/2, M}, {0, N/2}),
    .m11 = cspan_slice(Mat, &A, {M/2, M}, {N/2, N}),
  };
}

// Slow generic implementation
void base_case_matrix_product(Mat A, Mat B, OutMat C)
{
  for (int j = 0; j < C.shape[1]; ++j) {
    for (int i = 0; i < C.shape[0]; ++i) {
      Mat_value C_ij = 0;
      for (int k = 0; k < A.shape[1]; ++k) {
        C_ij += *cspan_at(&A, i,k) * *cspan_at(&B, k,j);
      }
      *cspan_at(&C, i,j) += C_ij;
    }
  }
}

void recursive_matrix_product(Mat A, Mat B, OutMat C)
{
  // Some hardware-dependent constant
  enum {recursion_threshold = 16};
  if (C.shape[0] <= recursion_threshold || C.shape[1] <= recursion_threshold) {
    base_case_matrix_product(A, B, C);
  } else {
    Partition c = partition(C),
              a = partition(A),
              b = partition(B);
    recursive_matrix_product(a.m00, b.m00, c.m00);
    recursive_matrix_product(a.m01, b.m10, c.m00);
    recursive_matrix_product(a.m10, b.m00, c.m10);
    recursive_matrix_product(a.m11, b.m10, c.m10);
    recursive_matrix_product(a.m00, b.m01, c.m01);
    recursive_matrix_product(a.m01, b.m11, c.m01);
    recursive_matrix_product(a.m10, b.m01, c.m11);
    recursive_matrix_product(a.m11, b.m11, c.m11);
  }
}


#define i_TYPE Values,double
#include "stc/stack.h"
#include "stc/crand.h"

int main(void)
{
  enum {N = 10, D1 = 256, D2 = D1};

  Values values = {0};
  for (int i=0; i < N*D1*D2; ++i)
      Values_push(&values, (crandf() - 0.5)*4.0);

  double out[D1*D2];
  Mat3 data = cspan_md_layout(c_ROWMAJOR, values.data, N, D1, D2);
  OutMat c = cspan_md_layout(c_COLMAJOR, out, D1, D2);
  Mat a = cspan_submd3(&data, 0);
  double sum = 0.0;
  clock_t t = clock();

  for (int i=1; i<N; ++i) {
    Mat b = cspan_submd3(&data, i);
    memset(out, 0, sizeof out);
    recursive_matrix_product(a, b, c); // > 2x faster
    //base_case_matrix_product(a, b, c);
    sum += *cspan_at(&c, 0, 1);
  }

  t = clock() - t;

  puts("a 8x8 sub-matrix of input a");
  cspan_print(Mat, cspan_slice(Mat, &a, {12,20}, {12,20}), "%.4f");
  puts("\na 8x8 sub-matrix of result c");
  cspan_print(Mat, cspan_slice(Mat, &c, {12,20}, {12,20}), "%.4f");

  puts("checksum and time");
  printf("%.16g: %f ms\n", sum, (double)t*1000.0/CLOCKS_PER_SEC);
  Values_drop(&values);
}
