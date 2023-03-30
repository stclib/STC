#include <string>
#include <iostream>
#include <chrono>

#include <stc/crand.h>
#include <stc/cstr.h>

#define i_type StcVec
#define i_val_str
#include <stc/cstack.h>

#define i_type StcSet
#define i_val_str
#include <stc/csset.h>

#include <vector>
using StdVec = std::vector<std::string>;
#include <set>
using StdSet = std::set<std::string>;


static const int BENCHMARK_SIZE = 2000000;
static const int MAX_STRING_SIZE = 50;
static const char CHARS[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz=+-";
using time_point = std::chrono::high_resolution_clock::time_point;


static inline std::string randomString_STD(int strsize) {
    std::string s(strsize, 0);
    char* p = &s[0];
    union { uint64_t u8; uint8_t b[8]; } r;
    for (int i = 0; i < strsize; ++i) {
        if ((i & 7) == 0) r.u8 = crand() & 0x3f3f3f3f3f3f3f3f;
        p[i] = CHARS[r.b[i & 7]];
    }
    return s;
}

static inline cstr randomString_STC(int strsize) {
    cstr s = cstr_with_size(strsize, 0);
    char* p = cstr_data(&s);
    union { uint64_t u8; uint8_t b[8]; } r;
    for (int i = 0; i < strsize; ++i) {
        if ((i & 7) == 0) r.u8 = crand() & 0x3f3f3f3f3f3f3f3f;
        p[i] = CHARS[r.b[i & 7]];
    }
    return s;
}


void addRandomString(StdVec& vec, int strsize) {
    vec.push_back(std::move(randomString_STD(strsize)));
}

void addRandomString(StcVec& vec, int strsize) {
    StcVec_push(&vec, randomString_STC(strsize));
}

void addRandomString(StdSet& set, int strsize) {
    set.insert(std::move(randomString_STD(strsize)));
}

void addRandomString(StcSet& set, int strsize) {
    StcSet_insert(&set, randomString_STC(strsize));
}


template <class C>
int benchmark(C& container, const int n, const int strsize) {
    time_point t1 = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < n; i++)
        addRandomString(container, strsize);

    time_point t2 = std::chrono::high_resolution_clock::now();
    const auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
    std::cerr << (strsize ? strsize : 32) << "\t" << duration;
    return (int)duration;
}


int main() {
    uint64_t seed = 4321;
    int sum, n;

    // VECTOR WITH STRINGS

    csrand(seed);
    sum = 0, n = 0;
    std::cerr << "\nstrsize\tmsecs\tstd::vector<std::string>, size=" << BENCHMARK_SIZE << "\n";
    for (int strsize = 1; strsize <= MAX_STRING_SIZE; strsize += 2) {
        StdVec vec; vec.reserve(BENCHMARK_SIZE);
        sum += benchmark(vec, BENCHMARK_SIZE, strsize), ++n;
        std::cout << '\t' << vec.front() << '\n';
    }
    std::cout << "Avg:\t" << sum/n << '\n';

    csrand(seed);
    sum = 0, n = 0;
    std::cerr << "\nstrsize\tmsecs\tcvec<cstr>, size=" << BENCHMARK_SIZE << "\n";
    for (int strsize = 1; strsize <= MAX_STRING_SIZE; strsize += 2) {
        StcVec vec = StcVec_with_capacity(BENCHMARK_SIZE);
        sum += benchmark(vec, BENCHMARK_SIZE, strsize), ++n;
        std::cout << '\t' << cstr_str(&vec.data[0]) << '\n';
        StcVec_drop(&vec);
    }
    std::cout << "Avg:\t" << sum/n << '\n';

    // SORTED SET WITH STRINGS
    
    csrand(seed);
    sum = 0, n = 0;
    std::cerr << "\nstrsize\tmsecs\tstd::set<std::string>, size=" << BENCHMARK_SIZE/16 << "\n";
    for (int strsize = 1; strsize <= MAX_STRING_SIZE; strsize += 2) {
        StdSet set;
        sum += benchmark(set, BENCHMARK_SIZE/16, strsize), ++n;
        std::cout << '\t' << *set.begin() << '\n';
    }
    std::cout << "Avg:\t" << sum/n << '\n';

    csrand(seed);
    sum = 0, n = 0;
    std::cerr << "\nstrsize\tmsecs\tcsset<cstr>, size=" << BENCHMARK_SIZE/16 << "\n";
    for (int strsize = 1; strsize <= MAX_STRING_SIZE; strsize += 2) {
        StcSet set = StcSet_with_capacity(BENCHMARK_SIZE/16);
        sum += benchmark(set, BENCHMARK_SIZE/16, strsize), ++n;
        std::cout << '\t' << cstr_str(StcSet_front(&set)) << '\n';
        StcSet_drop(&set);
    }
    std::cout << "Avg:\t" << sum/n << '\n';

    std::cerr << "sizeof(std::string) : " << sizeof(std::string) << std::endl
              << "sizeof(cstr)        : " << sizeof(cstr) << std::endl;
    return 0;
}
