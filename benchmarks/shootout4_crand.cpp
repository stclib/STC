#include <cstdint>
#include <iostream>
#include <ctime>
#include <stc/crandom.h>

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

// wyrand - 2020-12-07
static inline void _wymum(uint64_t *A, uint64_t *B){
#if defined(__SIZEOF_INT128__)
  __uint128_t r = *A; r *= *B;
  *A = (uint64_t) r; *B = (uint64_t ) (r >> 64);
#elif defined(_MSC_VER) && defined(_M_X64)
  *A = _umul128(*A, *B, B);
#else
  uint64_t ha=*A>>32, hb=*B>>32, la=(uint32_t)*A, lb=(uint32_t)*B, hi, lo;
  uint64_t rh=ha*hb, rm0=ha*lb, rm1=hb*la, rl=la*lb, t=rl+(rm0<<32), c=t<rl;
  lo=t+(rm1<<32); c+=lo<t; hi=rh+(rm0>>32)+(rm1>>32)+c;
  *A=lo;  *B=hi;
#endif
}
static inline uint64_t _wymix(uint64_t A, uint64_t B){
  _wymum(&A,&B); return A^B;
}
static inline uint64_t wyrand64(uint64_t *seed){
  static const uint64_t _wyp[] = {0xa0761d6478bd642full, 0xe7037ed1a0b428dbull};
  *seed+=_wyp[0]; return _wymix(*seed,*seed^_wyp[1]);
}


inline unsigned long long lehmer64(uint64_t* s) {
  *(__uint128_t *)s *= 0xda942042e4dd58b5ull;
  return *(__uint128_t *)s >> 64;
}

using namespace std;

int main(void)
{
  enum {N = 2000000000};
  uint64_t* recipient = new uint64_t[N];
  static stc64_t rng;
  init_state(rng.state, 12345123);

  cout << "WARMUP"  << endl;
  for (size_t i = 0; i < N; i++)
    recipient[i] = wyrand64(rng.state);

  clock_t beg, end;
  for (size_t ti = 0; ti < 4; ti++) {
    init_state(rng.state, 12345123);
    cout << endl << "ROUND " << ti+1 << endl;
    beg = clock();
    for (size_t i = 0; i < N; i++)
      recipient[i] = wyrand64(rng.state);
    end = clock();
    cout << "wyrand64:\t"
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
    init_state(rng.state, 12345123);
    uint64_t s = 0;
    beg = clock();
    for (size_t i = 0; i < N; i++)
      s += wyrand64(rng.state);
    end = clock();
    cout << "wyrand64:\t"
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
