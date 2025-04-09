// https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2023/p2642r2.html
// C99:
#include <stdio.h>
#include <time.h>
#include "stc/cspan.h"

using_cspan(Mat, float, 2);
using_cspan(Mat3, float, 3);
typedef Mat OutMat;

enum {N = 3, D1 = 512, D2 = D1, D3 = D1};

void native_matrix_product(float (*A)[D2], float (*B)[D3], float (*C)[D3]) {
    for (int j = 0; j < D3; ++j) {
        for (int i = 0; i < D1; ++i) {
            Mat_value C_ij = 0;
            for (int k = 0; k < D2; ++k) {
                C_ij += A[i][k] * B[k][j];
            }
            C[i][j] += C_ij;
        }
    }
}

// Generic implementation
void base_case_matrix_product(Mat A, Mat B, OutMat C) {
    for (c_range(j, C.shape[1])) {
        for (c_range(i, C.shape[0])) {
            Mat_value C_ij = 0;
            for (c_range(k, A.shape[1])) {
                C_ij += *cspan_at(&A, i,k) * *cspan_at(&B, k,j);
            }
            *cspan_at(&C, i,j) += C_ij;
        }
    }
}

// Recursive implementation
typedef struct { Mat m00, m01, m10, m11; } Partition;

Partition partition(Mat A) {
    int M = A.shape[0];
    int N = A.shape[1];
    return (Partition){
        .m00 = cspan_slice(&A, Mat, {0, M/2}, {0, N/2}),
        .m01 = cspan_slice(&A, Mat, {0, M/2}, {N/2, N}),
        .m10 = cspan_slice(&A, Mat, {M/2, M}, {0, N/2}),
        .m11 = cspan_slice(&A, Mat, {M/2, M}, {N/2, N}),
    };
}

void recursive_matrix_product(Mat A, Mat B, OutMat C) {
    // Some hardware-dependent constant
    if (C.shape[0] <= 16) {
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


#define i_type Data, float
#include "stc/stack.h"
#include "stc/random.h"

int main(void) {
    Data values = Data_with_size(D1*D2 + N*D2*D3, 0);
    for (c_each(i, Data, values))
        *i.ref = (Data_value)((crand64_real() - 0.5)*8.0);

    Mat a = cspan_md(values.data, D1, D2);
    Mat3 bvec = cspan_md(values.data + D1*D2, N, D2, D3);

    static Data_value out[D1*D3];
    OutMat c = cspan_md(out, D1, D3);
    //Data output = Data_with_size(D1*D3, 0);
    //OutMat c = cspan_md(output.data, D1, D3); // slower with malloced ram

    float sum = 0.0;
    clock_t t = clock();

    for (c_range(i, N)) {
        Mat b = cspan_submd3(&bvec, i);
        memset(c.data, 0, (size_t)Mat_size(&c)*sizeof *c.data);
        recursive_matrix_product(a, b, c); // > gcc 2-3x faster
        //base_case_matrix_product(a, b, c);
        //native_matrix_product((float(*)[D2])a.data, (float(*)[D3])b.data, (float(*)[D3])c.data);
        sum += *cspan_at(&c, i+1, i+1);
    }

    t = clock() - t;

    puts("an 8x8 sub-matrix of result c");
    cspan_print(Mat, cspan_slice(&c, Mat, {12,20}, {12,20}), "%.4f");

    puts("checksum and time");
    printf("%.16g: %f ms\n", (double)sum, (double)t*1000.0/CLOCKS_PER_SEC);
    Data_drop(&values);
    //Data_drop(&output);
}
