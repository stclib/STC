#include <string>
#include <iostream>
#include <vector>
#include <chrono>
#define STC_USE_SSO 1
#define i_type svec
#define i_val_str
#include <stc/cstack.h>

#define ROTL_(x, k) (x << (k) | x >> (8*sizeof(x) - (k)))

static uint64_t g_romutrio[3] = {
    0x26aa069ea2fb1a4dULL, 0x70c72c95cd592d04ULL,
    0x504f333d3aa0b359ULL,
};

static inline uint64_t romutrio(void) {
   uint64_t *s = g_romutrio, xp = s[0], yp = s[1], zp = s[2];
   s[0] = 15241094284759029579u * zp;
   s[1] = yp - xp; s[1] = ROTL_(s[1], 12);
   s[2] = zp - yp; s[2] = ROTL_(s[2], 44);
   return xp;
}

static void sromutrio(uint64_t seed) {
   uint64_t *s = g_romutrio;
   s[0] = 0x26aa069ea2fb1a4dULL + seed;
   s[1] = 0x70c72c95cd592d04ULL + seed;
   s[2] = 0x504f333d3aa0b359ULL + seed;
}


static const char CHARS[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz=+-";

static const int BENCHMARK_SIZE = 5000000;
static const int MAX_STRING_LENGTH = 20;

using time_point = std::chrono::high_resolution_clock::time_point;

void addRandomString_STD(std::vector<std::string>& vec, const int length) {
    std::string s(length, 0);
    char* p = &s[0];
    for (int i = 0; i < length; ++i) {
        p[i] = CHARS[romutrio() & 63];
    }
    s.append(s);
    vec.push_back(s);
}

void addRandomString_STC(svec& vec, const int length) {
    cstr s = cstr_with_size(length, 0);
    char* p = cstr_data(&s);
    for (int i = 0; i < length; ++i) {
        p[i] = CHARS[romutrio() & 63];
    }
    cstr_append_s(&s, s);
    svec_push(&vec, s);
}

template <class L, typename R>
void benchmark(L& vec, const int length, R addRandomString) {
    time_point t1 = std::chrono::high_resolution_clock::now();

    if (length == 0)
        for (int i = 0; i < BENCHMARK_SIZE; i++)
            addRandomString(vec, (i*13 & 31) + 1);
    else
        for (int i = 0; i < BENCHMARK_SIZE; i++)
            addRandomString(vec, length);

    time_point t2 = std::chrono::high_resolution_clock::now();
    const auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
    std::cerr << length*2 << "\t" << duration;
}


int main() {
    sromutrio(1234);
    std::cerr << "length\ttime\tstd::string\n";
    for (int k = 0; k < 4; k++) {
        std::vector<std::string> vec; vec.reserve(BENCHMARK_SIZE);
        benchmark(vec, 0, addRandomString_STD);
        std::cout << '\t' << vec[0] << '\n';
    }

    sromutrio(1234);
    std::cerr << "\nlength\ttime\tSTC string\n";
    for (int k = 0; k < 4; k++) {
        svec vec = svec_with_capacity(BENCHMARK_SIZE);
        benchmark(vec, 0, addRandomString_STC);
        std::cout << '\t' << cstr_str(&vec.data[0]) << '\n';
        svec_drop(&vec);
    }

    sromutrio(1234);
    std::cerr << "length\ttime\tstd::string\n";
    for (int length = 1; length <= MAX_STRING_LENGTH; length++) {
        std::vector<std::string> vec; vec.reserve(BENCHMARK_SIZE);
        benchmark(vec, length, addRandomString_STD);
        std::cout << '\t' << vec[0] << '\n';
    }

    sromutrio(1234);
    std::cerr << "\nlength\ttime\tSTC string\n";
    for (int length = 1; length <= MAX_STRING_LENGTH; length++) {
        svec vec = svec_with_capacity(BENCHMARK_SIZE);
        benchmark(vec, length, addRandomString_STC);
        std::cout << '\t' << cstr_str(&vec.data[0]) << '\n';
        svec_drop(&vec);
    }

    std::cerr << "size std::string : " << sizeof(std::string) << std::endl
              << "size STC string  : " << sizeof(cstr) << std::endl;
    return 0;
}
