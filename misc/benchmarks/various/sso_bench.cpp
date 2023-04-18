#include <string>
#include <iostream>
#include <chrono>

#include <stc/crand.h>
#include <stc/cstr.h>

#define i_type StcVec
#define i_val_str
#include <stc/cstack.h>

#define i_type StcSet
#define i_expandby 2
#define i_val_str
#include <stc/cset.h>

#include <vector>
using StdVec = std::vector<std::string>;

//#include "../external/ankerl/robin_hood.h"
//using StdSet = robin_hood::unordered_flat_set<std::string>;

#include <unordered_set>
using StdSet = std::unordered_set<std::string>;


static const int BENCHMARK_SIZE = 250000;
static const int MAX_STRING_SIZE = 100;
static const char CHARS[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz=+-";
using time_point = std::chrono::high_resolution_clock::time_point;


static inline const char* randomString(int strsize) {
    static char str[256];
    union { uint64_t u8; uint8_t b[8]; } r;
    for (int i = 0; i < strsize; ++i) {
        if ((i & 7) == 0) r.u8 = crand() & 0x3f3f3f3f3f3f3f3f;
        str[i] = CHARS[r.b[i & 7]];
    }
    str[strsize] = 0;
    return str;
}



static inline void addRandomString(StdVec& vec, int strsize) {
    vec.push_back(randomString(strsize));
}

static inline void addRandomString(StcVec& vec, int strsize) {
    StcVec_emplace(&vec, randomString(strsize));
}

static inline void addRandomString(StdSet& set, int strsize) {
    set.insert(randomString(strsize));
}

static inline void addRandomString(StcSet& set, int strsize) {
    StcSet_emplace(&set, randomString(strsize));
}

static inline bool getRandomString(const StdSet& set, int strsize) {
    return set.find(randomString(strsize)) != set.end();
}

static inline bool getRandomString(const StcSet& set, int strsize) {
    return StcSet_contains(&set, randomString(strsize));
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

template <class C>
int benchmark_lookup(C& container, const int n, const int strsize) {
    for (int i = 0; i < n; i++)
        addRandomString(container, strsize);

    time_point t1 = std::chrono::high_resolution_clock::now();
    int found = 0;
    for (int i = 0; i < n; i++)
        found += (int)getRandomString(container, strsize);

    time_point t2 = std::chrono::high_resolution_clock::now();
    const auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
    std::cerr << (strsize ? strsize : 32) << "\t" << duration << '\t' << found;
    return (int)duration;
}


int main() {
    uint64_t seed = 4321;
    int sum, n;

    // VECTOR WITH STRINGS

    csrand(seed);
    sum = 0, n = 0;
    std::cerr << "\nstrsize\tmsecs\tstd::vector<std::string>, size=" << BENCHMARK_SIZE << "\n";
    for (int strsize = 1; strsize <= MAX_STRING_SIZE; strsize += 4) {
        StdVec vec; vec.reserve(BENCHMARK_SIZE);
        sum += benchmark(vec, BENCHMARK_SIZE, strsize), ++n;
        std::cout << '\t' << vec.front() << '\n';
    }
    std::cout << "Avg:\t" << sum/n << "ms\n";

    csrand(seed);
    sum = 0, n = 0;
    std::cerr << "\nstrsize\tmsecs\tcvec<cstr>, size=" << BENCHMARK_SIZE << "\n";
    for (int strsize = 1; strsize <= MAX_STRING_SIZE; strsize += 4) {
        StcVec vec = StcVec_with_capacity(BENCHMARK_SIZE);
        sum += benchmark(vec, BENCHMARK_SIZE, strsize), ++n;
        std::cout << '\t' << cstr_str(&vec.data[0]) << '\n';
        StcVec_drop(&vec);
    }
    std::cout << "Avg:\t" << sum/n << "ms\n";

    // INSERT: SORTED SET WITH STRINGS

    csrand(seed);
    sum = 0, n = 0;
    std::cerr << "\nstrsize\tmsecs\tinsert: robin_hood::unordered_flat_set<std::string>, size=" << BENCHMARK_SIZE/2 << "\n";
    for (int strsize = 1; strsize <= MAX_STRING_SIZE; strsize += 4) {
        StdSet set; set.reserve(BENCHMARK_SIZE/2);
        sum += benchmark(set, BENCHMARK_SIZE/2, strsize), ++n;
        std::cout << '\t' << *set.begin() << '\n';
    }
    std::cout << "Avg:\t" << sum/n << "ms\n";

    
    csrand(seed);
    sum = 0, n = 0;
    std::cerr << "\nstrsize\tmsecs\tinsert: cset<cstr>, size=" << BENCHMARK_SIZE/2 << "\n";
    for (int strsize = 1; strsize <= MAX_STRING_SIZE; strsize += 4) {
        StcSet set = StcSet_with_capacity(BENCHMARK_SIZE/2);
        sum += benchmark(set, BENCHMARK_SIZE/2, strsize), ++n;
        std::cout << '\t' << cstr_str(StcSet_begin(&set).ref) << '\n';
        StcSet_drop(&set);
    }
    std::cout << "Avg:\t" << sum/n << "ms\n";

    // LOOKUP: SORTED SET WITH STRINGS

    csrand(seed);
    sum = 0, n = 0;
    std::cerr << "\nstrsize\tmsecs\tfind: robin_hood::unordered_flat_set<std::string>, size=" << BENCHMARK_SIZE/2 << "\n";
    for (int strsize = 1; strsize <= MAX_STRING_SIZE; strsize += 4) {
        StdSet set; set.reserve(BENCHMARK_SIZE/2);
        sum += benchmark_lookup(set, BENCHMARK_SIZE/2, strsize), ++n;
        std::cout << '\t' << *set.begin() << '\n';
    }
    std::cout << "Avg:\t" << sum/n << "ms\n";

    csrand(seed);
    sum = 0, n = 0;
    std::cerr << "\nstrsize\tmsecs\tfind: cset<cstr>, size=" << BENCHMARK_SIZE/2 << "\n";
    for (int strsize = 1; strsize <= MAX_STRING_SIZE; strsize += 4) {
        StcSet set = StcSet_with_capacity(BENCHMARK_SIZE/2);
        sum += benchmark_lookup(set, BENCHMARK_SIZE/2, strsize), ++n;
        std::cout << '\t' << cstr_str(StcSet_begin(&set).ref) << '\n';
        StcSet_drop(&set);
    }
    std::cout << "Avg:\t" << sum/n << "ms\n";


    std::cerr << "sizeof(std::string) : " << sizeof(std::string) << std::endl
              << "sizeof(cstr)        : " << sizeof(cstr) << std::endl
              << "sizeof(StdSet)      : " << sizeof(StdSet) << std::endl
              << "sizeof(StcSet)      : " << sizeof(StcSet) << std::endl;
    return 0;
}
