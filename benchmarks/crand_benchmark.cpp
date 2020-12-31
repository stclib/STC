#include <cstdint>
#include <iostream>
#include <ctime>
#include <stc/crand.h>

static inline uint64_t rotl64(const uint64_t x, const int k)
  { return (x << k) | (x >> (64 - k)); }

static uint64_t splitmix64_x = 87213627321ull; /* The state can be seeded with any value. */

uint64_t splitmix64(void) {
  uint64_t z = (splitmix64_x += 0x9e3779b97f4a7c15);
  z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9;
  z = (z ^ (z >> 27)) * 0x94d049bb133111eb;
  return z ^ (z >> 31);
}

static void init_state(uint64_t *rng, uint64_t seed) {
    splitmix64_x = seed;
    for (int i=0; i<4; ++i) rng[i] = splitmix64();
}

/* jsf64 */

static inline uint64_t jsf64(uint64_t *s) {
  uint64_t e = s[0] - rotl64(s[1], 7);
  s[0] = s[1] ^ rotl64(s[2], 13);
  s[1] = s[2] + rotl64(s[3], 37);
  s[2] = s[3] + e;
  s[3] = e + s[0];
  return s[3];
}

/* sfc64 */

static inline uint64_t sfc64(uint64_t *s) {
  uint64_t result = s[0] + s[1] + s[3]++;
  s[0] = s[1] ^ (s[1] >> 11);
  s[1] = s[2] + (s[2] << 3);
  s[2] = rotl64(s[2], 24) + result;
  return result;
}

/* sfc64 with Weyl increment */
static uint64_t weyl = 1234566789123ull;
static inline uint64_t sfc64w(uint64_t *s) {
  uint64_t result = s[0] + s[1] + (s[3] += weyl|1);
  s[0] = s[1] ^ (s[1] >> 11);
  s[1] = s[2] + (s[2] << 3);
  s[2] = rotl64(s[2], 24) + result;
  return result;
}


/* xoshiro256**  */

static inline uint64_t xoshiro256starstar(uint64_t* s) {
  const uint64_t result = rotl64(s[1] * 5, 7) * 9;
  const uint64_t t = s[1] << 17;
  s[2] ^= s[0];
  s[3] ^= s[1];
  s[1] ^= s[2];
  s[0] ^= s[3];
  s[2] ^= t;
  s[3] = rotl64(s[3], 45);
  return result;
}


inline unsigned long long wyhash64(uint64_t* s) {
  *s += 0x60bee2bee120fc15ull;
  __uint128_t tmp = (__uint128_t) *s * 0xa3b195354a39b70dull;
  unsigned long long m1 = (tmp >> 64) ^ tmp;
  tmp = (__uint128_t)m1 * 0x1b03738712fad5c9ull;
  return (tmp >> 64) ^ tmp;
}

inline unsigned long long lehmer64(uint64_t* s) {
  *(__uint128_t *)s *= 0xda942042e4dd58b5ull;
  return *(__uint128_t *)s >> 64;
}

using namespace std;

int main(void)
{
  enum {N = 800000000};
  uint64_t* recipient = new uint64_t[N];
  static stc64_t rng;
  init_state(rng.state, 12345123);

  cout << "WARMUP"  << endl;
  for (size_t i = 0; i < N; i++)
    recipient[i] = wyhash64(rng.state);

  clock_t beg, end;
  for (size_t ti = 0; ti < 4; ti++) {
    cout << endl << "ROUND " << ti+1 << endl;
    beg = clock();
    for (size_t i = 0; i < N; i++)
      recipient[i] = wyhash64(rng.state);
    end = clock();
    cout << "wyhash64:\t"
         << (float(end - beg) / CLOCKS_PER_SEC)
         << " s: " << recipient[312] << endl;
    beg = clock();
    for (size_t i = 0; i < N; i++)
      recipient[i] = sfc64w(rng.state);
    end = clock();
    cout << "sfc64w:\t\t"
         << (float(end - beg) / CLOCKS_PER_SEC)
         << " s: " << recipient[312] << endl;

    beg = clock();
    for (size_t i = 0; i < N; i++)
      recipient[i] = stc64_rand(&rng);
    end = clock();
    cout << "stc64:\t\t"
         << (float(end - beg) / CLOCKS_PER_SEC)
         << " s: " << recipient[312] << endl;

    beg = clock();
    for (size_t i = 0; i < N; i++)
      recipient[i] = xoshiro256starstar(rng.state);
    end = clock();
    cout << "xoshiro256**:\t"
         << (float(end - beg) / CLOCKS_PER_SEC)
         << " s: " << recipient[312] << endl;

    beg = clock();
    for (size_t i = 0; i < N; i++)
      recipient[i] = lehmer64(rng.state);
    end = clock();
    cout << "lehmer64:\t"
         << ((float) end - beg) / CLOCKS_PER_SEC
         << " s: " << recipient[312] << endl;

    cout << "Next we do random number computations only, doing no work."
         << endl;
    uint64_t s = 0;
    beg = clock();
    for (size_t i = 0; i < N; i++)
      s += wyhash64(rng.state);
    end = clock();
    cout << "wyhash64:\t"
         << ((float) end - beg) / CLOCKS_PER_SEC
         << " s: " << s << endl;

    s = 0;
    beg = clock();
    for (size_t i = 0; i < N; i++)
      s += sfc64w(rng.state);
    end = clock();
    cout << "sfc64w:\t\t"
         << ((float) end - beg) / CLOCKS_PER_SEC
         << " s: " << s << endl;

    s = 0;
    beg = clock();
    for (size_t i = 0; i < N; i++)
      s += stc64_rand(&rng);
    end = clock();
    cout << "stc64:\t\t"
         << ((float) end - beg) / CLOCKS_PER_SEC
         << " s: " << s << endl;

    s = 0;
    beg = clock();
    for (size_t i = 0; i < N; i++)
      s += xoshiro256starstar(rng.state);
    end = clock();
    cout << "xoshiro256**:\t"
         << ((float) end - beg) / CLOCKS_PER_SEC
         << " s: " << s << endl;

    s = 0;
    beg = clock();
    for (size_t i = 0; i < N; i++)
      s += lehmer64(rng.state);
    end = clock();
    cout << "lehmer64:\t"
         << ((float) end - beg) / CLOCKS_PER_SEC
         << " s: " << s << endl;
  }
  delete[] recipient;
  return 0;
}
