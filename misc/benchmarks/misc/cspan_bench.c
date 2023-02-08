#define STC_NDEBUG
#include <stc/cspan.h>
#include <stdio.h>
#include <time.h>

using_cspan3(MD, double);

// define the dimensions of a 3d-array
enum {
    nx = 64,
    ny = 64,
    nz = 64
};
int lx = 15, ly = 10, lz = 5;
int hx = 20, hy = 15, hz = 15;
intptr_t n = 1000000;

// define the contents of two nx x ny x nz arrays in and out
double Vout[nx * ny * nz];
double Vin[nx * ny * nz]; //, 1.23;

// define some slice indices for each dimension

static void MDRanges_setup(intptr_t state)
{
    double sum = 0;
    clock_t t = clock();

    for (intptr_t s = 0; s < state; ++s)
    {
        MD3 r_in = cspan_md(Vin, nx, ny, nz);
        MD3 r_out = cspan_md(Vout, nx, ny, nz);

        r_in = cspan_slice(MD3, &r_in, {lx, hx}, {ly, hy}, {lz, hz});
        r_out = cspan_slice(MD3, &r_out, {lx, hx}, {ly, hy}, {lz, hz});
        MD3_iter i = MD3_begin(&r_in); // can be iterated "flat".
        MD3_iter o = MD3_begin(&r_out);
        sum += Vin[s % nx];
    }
    t = clock() - t;
    printf("setup: %.1f ms, %f\n", 1000.0f * t / CLOCKS_PER_SEC, sum);
}

static void TraditionalForLoop(intptr_t state)
{
    clock_t t = clock();
    double sum = 0;

    for (int s = 0; s < state; ++s) {
        for (int x = lx; x < hx; ++x) {
            for (int y = ly; y < hy; ++y) {
                for (int z = lz; z < hz; ++z)
                {
                    double d = Vin[nz*(ny*x + y) + z];
                    Vout[nz*(ny*x + y) + z] += d;
                    sum += d;
                }
            }
        }
    }
    t = clock() - t;
    printf("forloop: %.1f ms, %f\n", 1000.0f * t / CLOCKS_PER_SEC, sum);
}

static void MDRanges_nested_loop(intptr_t state)
{
    MD3 r_in = cspan_md(Vin, nx, ny, nz);
    MD3 r_out = cspan_md(Vout, nx, ny, nz);
    r_in = cspan_slice(MD3, &r_in, {lx, hx}, {ly, hy}, {lz, hz});
    r_out = cspan_slice(MD3, &r_out, {lx, hx}, {ly, hy}, {lz, hz});

    // C++23: for (auto [o, i] : std::views::zip(flat(r_out), flat(r_in))) { o = i; }
    clock_t t = clock();
    double sum = 0;

    for (intptr_t s = 0; s < state; ++s) {
        for (int x = 0; x < r_in.shape[0]; ++x) {
            for (int y = 0; y < r_in.shape[1]; ++y) {
                for (int z = 0; z < r_in.shape[2]; ++z)
                {
                    double d = *cspan_at(&r_in, x, y, z);
                    *cspan_at(&r_out, x, y, z) += d;
                    sum += d;
                }
            }
        }
    }
    t = clock() - t;
    printf("nested: %.1f ms, %f\n", 1000.0f * t / CLOCKS_PER_SEC, sum);
}

static void MDRanges_loop_over_joined(intptr_t state)
{
    MD3 r_in = cspan_md(Vin, nx, ny, nz);
    MD3 r_out = cspan_md(Vout, nx, ny, nz);
    r_in = cspan_slice(MD3, &r_in, {lx, hx}, {ly, hy}, {lz, hz});
    r_out = cspan_slice(MD3, &r_out, {lx, hx}, {ly, hy}, {lz, hz});

    // C++23: for (auto [o, i] : std::views::zip(flat(r_out), flat(r_in))) { o = i; }
    double sum = 0;
    clock_t t = clock();

    for (intptr_t s = 0; s < state; ++s) {
        MD3_iter i = MD3_begin(&r_in);
        MD3_iter o = MD3_begin(&r_out);

        for (; i.ref; MD3_next(&i), MD3_next(&o))
        {
            *o.ref += *i.ref;
            sum += *i.ref;
        }
    }
    t = clock() - t;
    printf("joined: %.1f ms, %f\n", 1000.0f * t / CLOCKS_PER_SEC, sum);
}

int main()
{
    for (int i = 0; i < nx * ny * nz; ++i)
        Vin[i] = i + 1.23;

    MDRanges_setup(n);
    TraditionalForLoop(n);
    MDRanges_nested_loop(n);
    MDRanges_loop_over_joined(n);
}
