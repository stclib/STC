#include <stdio.h>
#include <time.h>
#include <stc/crandom.h>
#include <stc/cfmt.h>
#ifdef __cplusplus
#include <random>
#endif


#define NN 300000000

int main(void)
{
    clock_t difference, before;
    uint64_t v;

    crand_rng64_t stc = crand_rng64_init(time(NULL));
    crand_uniform_i64_t idist = crand_uniform_i64_init(10, 20);
    crand_uniform_f64_t fdist = crand_uniform_f64_init(10, 20);

    c_forrange (30) c_print(1, "{:02} ", crand_uniform_i64(&stc, &idist));
    puts("");

    crand_rng32_t pcg = crand_rng32_init(time(NULL));
    crand_uniform_i32_t i32dist = crand_uniform_i32_init(10, 20);
    crand_uniform_f32_t f32dist = crand_uniform_f32_init(10, 20);

    before = clock(); \
    v = 0;
    c_forrange (NN) {
        //v += crand_i32(&pcg);
        v += crand_uniform_i32(&pcg, &i32dist);
    }
    difference = clock() - before;
    c_print(1, "pcg32: {:.02f}, {}\n", (float) difference / CLOCKS_PER_SEC, v);

    before = clock(); \
    v = 0;
    c_forrange (NN) {
        //v += crand_i64(&stc) & 0xffffffff;
        v += crand_uniform_i64(&stc, &idist);
    }
    difference = clock() - before;
    c_print(1, "stc64: {:.02f}, {}\n", (float) difference / CLOCKS_PER_SEC, v);

    c_forrange (8) c_print(1, "{} ", crand_uniform_i32(&pcg, &i32dist));
    puts("");


    c_forrange (8) c_print(1, "{} ", crand_uniform_f32(&pcg, &f32dist));
    puts("");

    c_forrange (8) c_print(1, "{} ", crand_uniform_f64(&stc, &fdist));
    puts("");
}