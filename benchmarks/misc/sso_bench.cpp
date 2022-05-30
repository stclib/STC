#include <string>
#include <iostream>
#include <vector>
#include <unordered_set>
#include <chrono>
#define i_static
#include <stc/crandom.h>
#define i_static
#include <stc/cstr.h>

#define i_type stccon
#define i_val_str
#include <stc/cvec.h>

using stdcon = std::vector<std::string>;

static const int BENCHMARK_SIZE = 5000000;
static const int MAX_STRING_LENGTH = 40;
static const char CHARS[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz=+-";
using time_point = std::chrono::high_resolution_clock::time_point;


void addRandomString_STD(stdcon& con, int length) {
    std::string s(length, 0);
    char* p = &s[0];
    union { uint64_t u8; uint8_t b[8]; } r;
    for (int i = 0; i < length; ++i) {
        if ((i & 7) == 0) r.u8 = crandom() & 0x3f3f3f3f3f3f3f3f;
        p[i] = CHARS[r.b[i & 7]];
    }
    con.push_back(std::move(s));
}

void addRandomString_STC(stccon& con, int length) {
    cstr s = cstr_with_size(length, 0);
    char* p = cstr_data(&s);
    union { uint64_t u8; uint8_t b[8]; } r;
    for (int i = 0; i < length; ++i) {
        if ((i & 7) == 0) r.u8 = crandom() & 0x3f3f3f3f3f3f3f3f;
        p[i] = CHARS[r.b[i & 7]];
    }
    stccon_push_back(&con, s);
}

template <class L, typename R>
int benchmark(L& con, const int length, R addRandomString) {
    time_point t1 = std::chrono::high_resolution_clock::now();

    if (length == 0)
        for (int i = 0; i < BENCHMARK_SIZE; i++)
            addRandomString(con, (crandom() & 63) + 1);
    else
        for (int i = 0; i < BENCHMARK_SIZE; i++)
            addRandomString(con, length);

    time_point t2 = std::chrono::high_resolution_clock::now();
    const auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
    std::cerr << (length ? length : 32) << "\t" << duration;
    return (int)duration;
}


int main() {
    uint64_t seed = 4321;
    csrandom(seed);
    int sum, n;
    std::cerr << "length\ttime\tstd::string\n";
    for (int k = 0; k < 4; k++) {
        stdcon con; con.reserve(BENCHMARK_SIZE);
        benchmark(con, 0, addRandomString_STD);
        std::cout << '\t' << *con.begin() << '\n';
    }

    csrandom(seed);
    std::cerr << "\nlength\ttime\tSTC string\n";
    for (int k = 0; k < 4; k++) {
        stccon con = stccon_with_capacity(BENCHMARK_SIZE);
        benchmark(con, 0, addRandomString_STC);
        std::cout << '\t' << cstr_str(stccon_begin(&con).ref) << '\n';
        stccon_drop(&con);
    }

    csrandom(seed);
    sum = 0, n = 0;
    std::cerr << "\nlength\ttime\tstd::string\n";
    for (int length = 1; length <= MAX_STRING_LENGTH; length += 2) {
        stdcon con; con.reserve(BENCHMARK_SIZE);
        sum += benchmark(con, length, addRandomString_STD), ++n;
        std::cout << '\t' << *con.begin() << '\n';
    }
    std::cout << "Avg:\t" << sum/n << '\n';

    csrandom(seed);
    sum = 0, n = 0;
    std::cerr << "\nlength\ttime\tSTC string\n";
    for (int length = 1; length <= MAX_STRING_LENGTH; length += 2) {
        stccon con = stccon_with_capacity(BENCHMARK_SIZE);
        sum += benchmark(con, length, addRandomString_STC), ++n;
        std::cout << '\t' << cstr_str(stccon_begin(&con).ref) << '\n';
        stccon_drop(&con);
    }
    std::cout << "Avg:\t" << sum/n << '\n';

    std::cerr << "sizeof std::string : " << sizeof(std::string) << std::endl
              << "sizeof STC string  : " << sizeof(cstr) << std::endl;
    return 0;
}
