#include <stdio.h>
#include <time.h>
#include <stc/crandom.h>
#include <stc/cvec.h>

#ifdef __cplusplus
#include <vector>
#endif

enum {INSERT, ERASE, FIND, ITER, DESTRUCT, N_TESTS};
typedef struct { time_t t1, t2; uint64_t sum; float fac; } Range;
typedef struct { const char* name; Range test[N_TESTS]; } Sample;
enum {SAMPLES = 3, N = 100000000};

uint64_t seed = 1, mask1 = 0xfffffff;

static float secs(Range s) { return (float)(s.t2 - s.t1) / CLOCKS_PER_SEC; }

using_cvec(x, size_t);

#ifdef __cplusplus
Sample test_std_vector() {
    typedef std::vector<size_t> container;
    Sample s = {"std-vector"};
    {
        s.test[INSERT].t1 = clock();
        container con;
        stc64_srandom(seed);
        c_forrange (N) con.push_back(stc64_random() & mask1);
        s.test[INSERT].t2 = clock();
        s.test[INSERT].sum = con.size();
        s.test[ERASE].t1 = clock();
        c_forrange (N) con.pop_back();
        s.test[ERASE].t2 = clock();
        s.test[ERASE].sum = con.size();
     }{
        container con;
        stc64_srandom(seed);
        c_forrange (N) con.push_back(stc64_random() & mask1);
        s.test[ITER].t1 = clock();
        size_t sum = 0;
        c_forrange (i, N) sum += con[i];
        s.test[ITER].t2 = clock();
        s.test[ITER].sum = sum;
        s.test[DESTRUCT].t1 = clock();
     }
     s.test[DESTRUCT].t2 = clock();
     s.test[DESTRUCT].sum = 0;
     return s;
}
#endif


Sample test_stc_vector() {
    typedef cvec_x container;
    Sample s = {"stc-vector"};
    {
        s.test[INSERT].t1 = clock();
        container con = cvec_x_init();
        stc64_srandom(seed);
        c_forrange (N) cvec_x_push_back(&con, stc64_random() & mask1);
        s.test[INSERT].t2 = clock();
        s.test[INSERT].sum = cvec_x_size(con);
        s.test[ERASE].t1 = clock();
        c_forrange (N) { cvec_x_pop_back(&con); }
        s.test[ERASE].t2 = clock();
        s.test[ERASE].sum = cvec_x_size(con);
        cvec_x_del(&con);
     }{
        stc64_srandom(seed);
        container con = cvec_x_init();
        c_forrange (N) cvec_x_push_back(&con, stc64_random() & mask1);
        s.test[ITER].t1 = clock();
        size_t sum = 0;
        c_forrange (i, N) sum += *cvec_x_at(&con, i);
        s.test[ITER].t2 = clock();
        s.test[ITER].sum = sum;
        s.test[DESTRUCT].t1 = clock();
        cvec_x_del(&con);
     }
     s.test[DESTRUCT].t2 = clock();
     s.test[DESTRUCT].sum = 0;
     return s;
}

int main()
{
    Sample std_s[SAMPLES + 1] = {0}, stc_s[SAMPLES + 1] = {0};
    c_forrange (i, int, SAMPLES) {
        printf("vector benchmark sample %d\n", i);
        std_s[i] = test_std_vector();
        stc_s[i] = test_stc_vector();
        if (i > 0) c_forrange (j, int, N_TESTS) {
            if (secs(std_s[i].test[j]) < secs(std_s[0].test[j])) std_s[0].test[j] = std_s[i].test[j];
            if (secs(stc_s[i].test[j]) < secs(stc_s[0].test[j])) stc_s[0].test[j] = stc_s[i].test[j];
            if (stc_s[i].test[j].sum != stc_s[0].test[j].sum) printf("Error in sum: test %d, sample %d\n", i, j);
        }
    }
    float std_sum = 0, stc_sum = 0;
    c_forrange (j, N_TESTS) { std_sum += secs(std_s[0].test[j]); stc_sum += secs(stc_s[0].test[j]); }
    printf("Test-name, Insert, Erase, Find, Iter, Destruct, Total, Ratio\n");

    printf("%s", std_s[0].name); c_forrange (j, N_TESTS) printf(", %.3f", secs(std_s[0].test[j]));
    printf(", %.3f, 1.000\n", std_sum);

    printf("%s", stc_s[0].name); c_forrange (j, N_TESTS) printf(", %.3f", secs(stc_s[0].test[j]));
    printf(", %.3f, %.3f\n", stc_sum, std_sum/stc_sum);
}
