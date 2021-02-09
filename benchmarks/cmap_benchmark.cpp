#include <stdio.h>
#include <time.h>
#include <stc/crandom.h>
#include <stc/cmap.h>

#ifdef __cplusplus
#include <unordered_map>
#endif

enum {INSERT, ERASE, FIND, ITER, DESTRUCT, N_TESTS};
typedef struct { time_t t1, t2; uint64_t sum; float fac; } Range;
typedef struct { const char* name; Range test[N_TESTS]; } Sample;
enum {SAMPLES = 3, N = 10000000};

uint64_t seed = 1, mask1 = 0xffffffff;

static float secs(Range s) { return (float)(s.t2 - s.t1) / CLOCKS_PER_SEC; }

static inline uint32_t hash64(const void* data, size_t len) {
    uint64_t x = *(const uint64_t *)data * 11400714819323198485ull;
    return x ^ (x >> 24);
}
using_cmap(x, size_t, size_t, c_default_equals, c_default_hash32);

#ifdef __cplusplus
Sample test_std_map() {
    typedef std::unordered_map<size_t, size_t> container;
    Sample s = {"std-unordered_map"};
    {
        stc64_srandom(seed);
        s.test[INSERT].t1 = clock();
        container con;
        c_forrange (i, N) con.emplace(stc64_random() & mask1, i);
        s.test[INSERT].t2 = clock();
        s.test[INSERT].sum = con.size();
        stc64_srandom(seed);
        s.test[ERASE].t1 = clock();
        c_forrange (N) con.erase(stc64_random() & mask1);
        s.test[ERASE].t2 = clock();
        s.test[ERASE].sum = con.size();
     }{
        container con;
        stc64_srandom(seed);
        c_forrange (i, N) con.emplace(stc64_random() & mask1, i);
        stc64_srandom(seed);
        s.test[FIND].t1 = clock();
        size_t sum = 0;
        c_forrange (N) sum += con.find(stc64_random() & mask1)->second;
        s.test[FIND].t2 = clock();
        s.test[FIND].sum = sum;
        s.test[ITER].t1 = clock();
        sum = 0;
        for (auto i: con) sum += i.second;
        s.test[ITER].t2 = clock();
        s.test[ITER].sum = sum;
        s.test[DESTRUCT].t1 = clock();
     }
     s.test[DESTRUCT].t2 = clock();
     s.test[DESTRUCT].sum = 0;
     return s;
}
#endif


Sample test_stc_map() {
    typedef cmap_x container;
    Sample s = {"stc-unordered_map"};
    {
        stc64_srandom(seed);
        s.test[INSERT].t1 = clock();
        container con = cmap_x_init();
        c_forrange (i, N) cmap_x_emplace(&con, stc64_random() & mask1, i);
        s.test[INSERT].t2 = clock();
        s.test[INSERT].sum = cmap_x_size(con);
        stc64_srandom(seed);
        s.test[ERASE].t1 = clock();
        c_forrange (N) cmap_x_erase(&con, stc64_random() & mask1);
        s.test[ERASE].t2 = clock();
        s.test[ERASE].sum = cmap_x_size(con);
        cmap_x_del(&con);
     }{
        container con = cmap_x_init();
        stc64_srandom(seed);
        c_forrange (i, N) cmap_x_emplace(&con, stc64_random() & mask1, i);
        stc64_srandom(seed);
        s.test[FIND].t1 = clock();
        size_t sum = 0;
        c_forrange (N) sum += cmap_x_find(&con, stc64_random() & mask1).ref->second;
        s.test[FIND].t2 = clock();
        s.test[FIND].sum = sum;
        s.test[ITER].t1 = clock();
        sum = 0;
        c_foreach (i, cmap_x, con) sum += i.ref->second;
        s.test[ITER].t2 = clock();
        s.test[ITER].sum = sum;
        s.test[DESTRUCT].t1 = clock();
        cmap_x_del(&con);
     }
     s.test[DESTRUCT].t2 = clock();
     s.test[DESTRUCT].sum = 0;
     return s;
}

int main()
{
    Sample std_s[SAMPLES + 1], stc_s[SAMPLES + 1];
    c_forrange (i, int, SAMPLES) {
        printf("unordered_map benchmark sample %d\n", i);
        std_s[i] = test_std_map();
        stc_s[i] = test_stc_map();
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
