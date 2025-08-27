// https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2023/p2642r2.html
// https://vaibhaw-vipul.medium.com/matrix-multiplication-optimizing-the-code-from-6-hours-to-1-sec-70889d33dcfa
// Compile with: gcc -O3 -march=native -fopenmp matmult.c -lstc  # Multiplies two 4K matrices in 0.5 seconds on a Ryzen 5700X CPU.
#define NDEBUG
#include <stdio.h>
#include <time.h>
#include <stc/cspan.h>

use_cspan(Mat, float, 2);
typedef Mat OutMat;

// Default matrix multiplication
void base_case_matrix_product(Mat A, Mat B, OutMat C) {
    #ifdef __GNUC__
      #pragma omp parallel for schedule(runtime)
    #endif
    for (int i = 0; i < A.shape[0]; ++i) {
        for (int k = 0; k < A.shape[1]; k++) {
            for (int j = 0; j < B.shape[1]; j++) {
                *cspan_at(&C, i,j) += *cspan_at(&A, i,k) * *cspan_at(&B, k,j);
            }
        }
    }
}

// Recursive implementation
typedef struct { Mat r00, r01, r10, r11; } Partition;

Partition partition(Mat A) {
    int m = A.shape[0];
    int n = A.shape[1];
    return (Partition){
        .r00 = cspan_slice(&A, Mat, {0, m/2}, {0, n/2}),
        .r01 = cspan_slice(&A, Mat, {0, m/2}, {n/2, n}),
        .r10 = cspan_slice(&A, Mat, {m/2, m}, {0, n/2}),
        .r11 = cspan_slice(&A, Mat, {m/2, m}, {n/2, n}),
    };
}

void recursive_matrix_product(Mat A, Mat B, OutMat C) {
    // Some hardware-dependent constant
    if (C.shape[0]*C.shape[1] <= 2048*2048) {
        base_case_matrix_product(A, B, C);
    } else {
        Partition c = partition(C),
                  a = partition(A),
                  b = partition(B);
        recursive_matrix_product(a.r00, b.r00, c.r00);
        recursive_matrix_product(a.r01, b.r10, c.r00);
        recursive_matrix_product(a.r10, b.r00, c.r10);
        recursive_matrix_product(a.r11, b.r10, c.r10);
        recursive_matrix_product(a.r00, b.r01, c.r01);
        recursive_matrix_product(a.r01, b.r11, c.r01);
        recursive_matrix_product(a.r10, b.r01, c.r11);
        recursive_matrix_product(a.r11, b.r11, c.r11);
    }
}


#define T Data, float
#include <stc/stack.h>
#include <stc/random.h>

int main(int argc, char* argv[]) {
    int M = 128, P, N;
    if (argc > 1)
        M = atoi(argv[1]);
    if (argc > 3) {
        P = atoi(argv[2]);
        N = atoi(argv[3]);
    } else
        P = N = M;

    printf("Recursive Matrix Multiplication (%dx%d * %dx%d)\n", M, P, P, N);
    printf("Usage: %s [m [p n]]\n", argv[0]);

    Data values = Data_with_size(M*P + P*N + M*N, 0);
    for (c_each_n(i, Data, values, M*P + P*N))
        *i.ref = (Data_value)((crand64_real() - 0.5)*10.0);

    Mat a = cspan_md(values.data, M, P);
    Mat b = cspan_md(values.data + M*P, P, N);
    OutMat c = cspan_md(values.data + M*P + P*N, M, N);

    clock_t t = clock();

    //base_case_matrix_product(a, b, c);
    recursive_matrix_product(a, b, c); // > gcc 2x faster

    t = clock() - t;

    puts("an 8x8 sub-matrix of result c");
    cspan_print(Mat, "%.4f", cspan_slice(&c, Mat, {0, 8}, {0, 8}));

    printf("\ntime %f ms\n", (double)t*1000.0/CLOCKS_PER_SEC);
    Data_drop(&values);
}
