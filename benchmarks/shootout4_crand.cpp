#include <cstdint>
#include <iostream>
#include <ctime>
#include <random>
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

/* romu_trio */

uint64_t romu_trio(uint64_t s[3]) {
   uint64_t xp = s[0], yp = s[1], zp = s[2];
   s[0] = 15241094284759029579u * zp;
   s[1] = yp - xp; s[1] = rotl64(s[1], 12);
   s[2] = zp - yp; s[2] = rotl64(s[2], 44);
   return xp;
}

/* sfc64 */

static inline uint64_t sfc64(uint64_t s[4]) {
  uint64_t result = s[0] + s[1] + s[3]++;
  s[0] = s[1] ^ (s[1] >> 11);
  s[1] = s[2] + (s[2] << 3);
  s[2] = rotl64(s[2], 24) + result;
  return result;
}

/* xoshiro128+  */

uint64_t xoroshiro128plus(uint64_t s[2]) {
	const uint64_t s0 = s[0];
	uint64_t s1 = s[1];
	const uint64_t result = s0 + s1;

	s1 ^= s0;
	s[0] = rotl64(s0, 24) ^ s1 ^ (s1 << 16); // a, b
	s[1] = rotl64(s1, 37); // c

	return result;
}


/* xoshiro256**  */

static inline uint64_t xoshiro256starstar(uint64_t s[4]) {
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


using namespace std;

int main(void)
{
  enum {N = 2000000000};
  uint16_t* recipient = new uint16_t[N];
  static stc64_t rng;
  init_state(rng.state, 12345123);
  std::mt19937 mt(12345123);

  cout << "WARMUP"  << endl;
  for (size_t i = 0; i < N; i++)
    recipient[i] = wyrand64(rng.state);

  clock_t beg, end;
  for (size_t ti = 0; ti < 2; ti++) {
    init_state(rng.state, 12345123);
    cout << endl << "ROUND " << ti+1 << " ---------" << endl;

    beg = clock();
    for (size_t i = 0; i < N; i++)
      recipient[i] = romu_trio(rng.state);
    end = clock();
    cout << "romu_trio:\t"
         << (float(end - beg) / CLOCKS_PER_SEC)
         << "s: " << recipient[312] << endl;

    beg = clock();
    for (size_t i = 0; i < N; i++)
      recipient[i] = wyrand64(rng.state);
    end = clock();
    cout << "wyrand64:\t"
         << (float(end - beg) / CLOCKS_PER_SEC)
         << "s: " << recipient[312] << endl;

    beg = clock();
    for (size_t i = 0; i < N; i++)
      recipient[i] = sfc64(rng.state);
    end = clock();
    cout << "sfc64:\t\t"
         << (float(end - beg) / CLOCKS_PER_SEC)
         << "s: " << recipient[312] << endl;

    beg = clock();
    for (size_t i = 0; i < N; i++)
      recipient[i] = stc64_rand(&rng);
    end = clock();
    cout << "stc64:\t\t"
         << (float(end - beg) / CLOCKS_PER_SEC)
         << "s: " << recipient[312] << endl;

    beg = clock();
    for (size_t i = 0; i < N; i++)
      recipient[i] = xoroshiro128plus(rng.state);
    end = clock();
    cout << "xoroshiro128+:\t"
         << (float(end - beg) / CLOCKS_PER_SEC)
         << "s: " << recipient[312] << endl;

    beg = clock();
    for (size_t i = 0; i < N; i++)
      recipient[i] = xoshiro256starstar(rng.state);
    end = clock();
    cout << "xoshiro256**:\t"
         << (float(end - beg) / CLOCKS_PER_SEC)
         << "s: " << recipient[312] << endl;

    beg = clock();
    for (size_t i = 0; i < N; i++)
      recipient[i] = mt();
    end = clock();
    cout << "std::mt19937:\t"
         << (float(end - beg) / CLOCKS_PER_SEC)
         << "s: " << recipient[312] << endl;
  }
  delete[] recipient;
  return 0;
}
