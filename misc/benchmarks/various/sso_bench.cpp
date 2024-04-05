#include <string>
#include <iostream>
#include <chrono>

#include "stc/crand.h"
#define i_static
#include "stc/cstr.h"

#define i_type StcVec
#define i_val_str
#include "stc/stack.h"

#include <vector>
using StdVec = std::vector<std::string>;


#include <unordered_set>
#include "../external/ankerl/robin_hood.h"

struct string_hash {
  using is_transparent = void;
  [[nodiscard]] size_t operator()(const char *txt) const {
    return std::hash<std::string_view>{}(txt);
  }
  [[nodiscard]] size_t operator()(std::string_view txt) const {
    return std::hash<std::string_view>{}(txt);
  }
  [[nodiscard]] size_t operator()(const std::string &txt) const {
    return std::hash<std::string>{}(txt);
  }
};
//using StdSet = robin_hood::unordered_flat_set<std::string, string_hash, std::equal_to<>>;
using StdSet = std::unordered_set<std::string, string_hash, std::equal_to<>>;

#define i_type StcSet
#define i_val_str
//#define i_hash(txtp) std::hash<std::string_view>{}(*txtp)
//#include "stc/hset.h"
#define _i_is_set
#include "stc/hmap-robin.h"



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



static inline void addRandomString(StdVec& vec, const char* str) {
    vec.push_back(str);
}

static inline void addRandomString(StcVec& vec, const char* str) {
    StcVec_emplace(&vec, str);
}

static inline void addRandomString(StdSet& set, const char* str) {
    set.insert(str);
}

static inline void addRandomString(StcSet& set, const char* str) {
    StcSet_emplace(&set, str);
}

static inline bool getRandomString(const StdSet& set, const char* str) {
    return set.find(str) != set.end();
}

static inline bool getRandomString(const StcSet& set, const char* str) {
    return StcSet_contains(&set, str);
}


template <class C>
int benchmark(C& container, const int n, const int strsize) {
    time_point t1 = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < n; i++)
        addRandomString(container, randomString(strsize));

    time_point t2 = std::chrono::high_resolution_clock::now();
    const auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
    std::cerr << (strsize ? strsize : 32) << "\t" << duration;
    return (int)duration;
}

template <class C>
int benchmark_lookup(C& container, const int n, const int strsize) {
    for (int i = 0; i < n; i++)
        addRandomString(container, randomString(strsize));

    time_point t1 = std::chrono::high_resolution_clock::now();
    int found = 0;
    for (int i = 0; i < n; i++)
        found += (int)getRandomString(container, randomString(strsize));

    time_point t2 = std::chrono::high_resolution_clock::now();
    const auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
    std::cerr << (strsize ? strsize : 32) << "\t" << duration << '\t' << found;
    return (int)duration;
}

#include <time.h>
int main(void) {
    uint64_t seed = time(NULL); // 4321;
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
    std::cerr << "\nstrsize\tmsecs\tvec<cstr>, size=" << BENCHMARK_SIZE << "\n";
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
    std::cerr << "\nstrsize\tmsecs\tinsert: hset<cstr>, size=" << BENCHMARK_SIZE/2 << "\n";
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
    for (int strsize = 1; strsize <= MAX_STRING_SIZE; strsize += 2) {
        StdSet set;
        sum += benchmark_lookup(set, BENCHMARK_SIZE/2, strsize), ++n;
        std::cout << '\t' << *set.begin() << '\n';
    }
    std::cout << "Avg:\t" << sum/n << "ms\n";

    csrand(seed);
    sum = 0, n = 0;
    std::cerr << "\nstrsize\tmsecs\tfind: hset<cstr>, size=" << BENCHMARK_SIZE/2 << "\n";
    for (int strsize = 1; strsize <= MAX_STRING_SIZE; strsize += 2) {
        StcSet set = {0};
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
