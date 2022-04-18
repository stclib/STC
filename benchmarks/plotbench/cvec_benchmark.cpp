#include <stdio.h>
#include <time.h>
#include <stc/crandom.h>

#ifdef __cplusplus
#include <vector>
#include <algorithm>
#endif

enum {INSERT, ERASE, FIND, ITER, DESTRUCT, N_TESTS};
const char* operations[] = {"insert", "erase", "find", "iter", "destruct"};
typedef struct { time_t t1, t2; uint64_t sum; float fac; } Range;
typedef struct { const char* name; Range test[N_TESTS]; } Sample;
enum {SAMPLES = 2, N = 150000000, S = 0x3ffc, R = 4};
uint64_t seed = 1, mask1 = 0xfffffff, mask2 = 0xffff;

static float secs(Range s) { return (float)(s.t2 - s.t1) / CLOCKS_PER_SEC; }

#define i_val size_t
#define i_tag x
#include <stc/cvec.h>

#ifdef __cplusplus
Sample test_std_vector() {
    typedef std::vector<size_t> container;
    Sample s = {"std,vector"};
    {
        s.test[INSERT].t1 = clock();
        container con;
        csrandom(seed);
        c_forrange (N) con.push_back(crandom() & mask1);
        s.test[INSERT].t2 = clock();
        s.test[INSERT].sum = con.size();
        s.test[ERASE].t1 = clock();
        c_forrange (N) con.pop_back();
        s.test[ERASE].t2 = clock();
        s.test[ERASE].sum = con.size();
     }{
        container con;
        csrandom(seed);
        c_forrange (N) con.push_back(crandom() & mask2);
        s.test[FIND].t1 = clock();
        size_t sum = 0;
        container::iterator it;
        // Iteration - not inherent find - skipping
        //c_forrange (S) if ((it = std::find(con.begin(), con.end(), crandom() & mask2)) != con.end()) sum += *it;
        s.test[FIND].t2 = clock();
        s.test[FIND].sum = sum;
        s.test[ITER].t1 = clock();
        sum = 0;
        c_forrange (R) c_forrange (i, N) sum += con[i];
        s.test[ITER].t2 = clock();
        s.test[ITER].sum = sum;
        s.test[DESTRUCT].t1 = clock();
     }
     s.test[DESTRUCT].t2 = clock();
     s.test[DESTRUCT].sum = 0;
     return s;
}
#else
Sample test_std_vector() { Sample s = {"std-vector"}; return s;}
#endif



Sample test_stc_vector() {
    typedef cvec_x container;
    Sample s = {"STC,vector"};
    {
        s.test[INSERT].t1 = clock();
        container con = cvec_x_init();
        csrandom(seed);
        c_forrange (N) cvec_x_push_back(&con, crandom() & mask1);
        s.test[INSERT].t2 = clock();
        s.test[INSERT].sum = cvec_x_size(con);
        s.test[ERASE].t1 = clock();
        c_forrange (N) { cvec_x_pop_back(&con); }
        s.test[ERASE].t2 = clock();
        s.test[ERASE].sum = cvec_x_size(con);
        cvec_x_drop(&con);
     }{
        csrandom(seed);
        container con = cvec_x_init();
        c_forrange (N) cvec_x_push_back(&con, crandom() & mask2);
        s.test[FIND].t1 = clock();
        size_t sum = 0;
        //cvec_x_iter it, end = cvec_x_end(&con);
        //c_forrange (S) if ((it = cvec_x_find(&con, crandom() & mask2)).ref != end.ref) sum += *it.ref;
        s.test[FIND].t2 = clock();
        s.test[FIND].sum = sum;
        s.test[ITER].t1 = clock();
        sum = 0;
        c_forrange (R) c_forrange (i, N) sum += con.data[i];
        s.test[ITER].t2 = clock();
        s.test[ITER].sum = sum;
        s.test[DESTRUCT].t1 = clock();
        cvec_x_drop(&con);
     }
     s.test[DESTRUCT].t2 = clock();
     s.test[DESTRUCT].sum = 0;
     return s;
}

int main(int argc, char* argv[])
{
    Sample std_s[SAMPLES + 1] = {0}, stc_s[SAMPLES + 1] = {0};
    c_forrange (i, int, SAMPLES) {
        std_s[i] = test_std_vector();
        stc_s[i] = test_stc_vector();
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
