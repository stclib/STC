#include <stdio.h>
#include <time.h>
#include <stc/crandom.h>

#ifdef __cplusplus
#include <unordered_map>
#endif

enum {INSERT, ERASE, FIND, ITER, DESTRUCT, N_TESTS};
const char* operations[] = {"insert", "erase", "find", "iter", "destruct"};
typedef struct { time_t t1, t2; uint64_t sum; float fac; } Range;
typedef struct { const char* name; Range test[N_TESTS]; } Sample;
enum {SAMPLES = 2, N = 8000000, R = 4};
uint64_t seed = 1, mask1 = 0xffffffff;

static float secs(Range s) { return (float)(s.t2 - s.t1) / CLOCKS_PER_SEC; }

#define i_key uint64_t
#define i_val uint64_t
#define i_hash c_hash64
#define i_tag x
#include <stc/cmap.h>

#ifdef __cplusplus
Sample test_std_unordered_map() {
    typedef std::unordered_map<uint64_t, uint64_t> container;
    Sample s = {"std,unordered_map"};
    {
        csrandom(seed);
        s.test[INSERT].t1 = clock();
        container con;
        c_forrange (i, N/2) con.emplace(crandom() & mask1, i);
        c_forrange (i, N/2) con.emplace(i, i);
        s.test[INSERT].t2 = clock();
        s.test[INSERT].sum = con.size();
        csrandom(seed);
        s.test[ERASE].t1 = clock();
        c_forrange (N) con.erase(crandom() & mask1);
        s.test[ERASE].t2 = clock();
        s.test[ERASE].sum = con.size();
     }{
        container con;
        csrandom(seed);
        c_forrange (i, N/2) con.emplace(crandom() & mask1, i);
        c_forrange (i, N/2) con.emplace(i, i);
        csrandom(seed);
        s.test[FIND].t1 = clock();
        size_t sum = 0;
        container::iterator it;
        c_forrange (N) if ((it = con.find(crandom() & mask1)) != con.end()) sum += it->second;
        s.test[FIND].t2 = clock();
        s.test[FIND].sum = sum;
        s.test[ITER].t1 = clock();
        sum = 0;
        c_forrange (R) for (auto i: con) sum += i.second;
        s.test[ITER].t2 = clock();
        s.test[ITER].sum = sum;
        s.test[DESTRUCT].t1 = clock();
     }
     s.test[DESTRUCT].t2 = clock();
     s.test[DESTRUCT].sum = 0;
     return s;
}
#else
Sample test_std_unordered_map() { Sample s = {"std-unordered_map"}; return s;}
#endif


Sample test_stc_unordered_map() {
    typedef cmap_x container;
    Sample s = {"STC,unordered_map"};
    {
        csrandom(seed);
        s.test[INSERT].t1 = clock();
        container con = cmap_x_init();
        c_forrange (i, N/2) cmap_x_insert(&con, crandom() & mask1, i);
        c_forrange (i, N/2) cmap_x_insert(&con, i, i);
        s.test[INSERT].t2 = clock();
        s.test[INSERT].sum = cmap_x_size(con);
        csrandom(seed);
        s.test[ERASE].t1 = clock();
        c_forrange (N) cmap_x_erase(&con, crandom() & mask1);
        s.test[ERASE].t2 = clock();
        s.test[ERASE].sum = cmap_x_size(con);
        cmap_x_drop(&con);
     }{
        container con = cmap_x_init();
        csrandom(seed);
        c_forrange (i, N/2) cmap_x_insert(&con, crandom() & mask1, i);
        c_forrange (i, N/2) cmap_x_insert(&con, i, i);
        csrandom(seed);
        s.test[FIND].t1 = clock();
        size_t sum = 0;
        const cmap_x_value* val;
        c_forrange (N) 
            if ((val = cmap_x_get(&con, crandom() & mask1)))
                sum += val->second;
        s.test[FIND].t2 = clock();
        s.test[FIND].sum = sum;
        s.test[ITER].t1 = clock();
        sum = 0;
        c_forrange (R) c_foreach (i, cmap_x, con) sum += i.ref->second;
        s.test[ITER].t2 = clock();
        s.test[ITER].sum = sum;
        s.test[DESTRUCT].t1 = clock();
        cmap_x_drop(&con);
     }
     s.test[DESTRUCT].t2 = clock();
     s.test[DESTRUCT].sum = 0;
     return s;
}

int main(int argc, char* argv[])
{
    Sample std_s[SAMPLES + 1], stc_s[SAMPLES + 1];
    c_forrange (i, int, SAMPLES) {
        std_s[i] = test_std_unordered_map();
        stc_s[i] = test_stc_unordered_map();
        if (i > 0) c_forrange (j, int, N_TESTS) {
            if (secs(std_s[i].test[j]) < secs(std_s[0].test[j])) std_s[0].test[j] = std_s[i].test[j];
            if (secs(stc_s[i].test[j]) < secs(stc_s[0].test[j])) stc_s[0].test[j] = stc_s[i].test[j];
            if (stc_s[i].test[j].sum != stc_s[0].test[j].sum) printf("Error in sum: test %d, sample %d\n", i, j);
        }
    }
    const char* comp = argc > 1 ? argv[1] : "test";
    bool header = (argc > 2 && argv[2][0] == '1');
    float std_sum = 0, stc_sum = 0;
    c_forrange (j, N_TESTS) { std_sum += secs(std_s[0].test[j]); stc_sum += secs(stc_s[0].test[j]); }
    if (header) printf("Compiler,Library,C,Method,Seconds,Ratio\n");
    c_forrange (j, N_TESTS) printf("%s,%s n:%d,%s,%.3f,%.3f\n", comp, std_s[0].name, N, operations[j], secs(std_s[0].test[j]), 1.0f);
                            printf("%s,%s n:%d,%s,%.3f,%.3f\n", comp, std_s[0].name, N, "total", std_sum, 1.0f);
    c_forrange (j, N_TESTS) printf("%s,%s n:%d,%s,%.3f,%.3f\n", comp, stc_s[0].name, N, operations[j], secs(stc_s[0].test[j]), secs(std_s[0].test[j]) ? secs(stc_s[0].test[j])/secs(std_s[0].test[j]) : 1.0f);
                            printf("%s,%s n:%d,%s,%.3f,%.3f\n", comp, stc_s[0].name, N, "total", stc_sum, stc_sum/std_sum);
}