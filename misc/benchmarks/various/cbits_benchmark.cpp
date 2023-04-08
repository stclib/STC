#include <iostream>
#include <bitset>
#include <cstdlib> // rand
#include <ctime> // timer

enum{ N=1<<22 }; // 4.2 mill.
#define i_static
#include <stc/crand.h>
#define i_type cbits
#define i_len N
#include <stc/cbits.h>

inline unsigned long get_time_in_ms()
{
    return (unsigned long)(1000 * clock() / CLOCKS_PER_SEC);
}


void one_sec_delay()
{
    unsigned long end_time = get_time_in_ms() + 1000;

    while(get_time_in_ms() < end_time)
    {
    }
}


int main(int argc, char **argv)
{
    size_t seed = time(NULL);

    using namespace std;
    bool *bools = new bool[N];

    unsigned long current_time, difference1, difference2;
    uint64_t total;

    one_sec_delay();

    total = 0;
    csrand(seed);
    current_time = get_time_in_ms();

    c_forrange (40 * N)
    {
        uint64_t r = crand();
        bools[r & (N-1)] = r & 1<<29;
    }

    difference1 = get_time_in_ms() - current_time;
    current_time = get_time_in_ms();

    c_forrange (100) c_forrange (num, N) 
    {
        total += bools[num];
    }

    delete [] bools;

    difference2 = get_time_in_ms() - current_time;

    cout << "Bool:" << endl << "sum total = " << total << ", random access time = " << difference1 
                            << ", sequential access time = " << difference2 << endl << endl;

    one_sec_delay();

    total = 0;
    csrand(seed);
    current_time = get_time_in_ms();
    bitset<N> bits;

    c_forrange (40 * N)
    {
        uint64_t r = crand();
        bits[r & (N-1)] = r & 1<<29;
    }

    difference1 = get_time_in_ms() - current_time;
    current_time = get_time_in_ms();

    c_forrange (100) c_forrange (num, N)
    {
        total += bits[num];
    }   

    difference2 = get_time_in_ms() - current_time;

    cout << "Bitset:" << endl << "sum total = " << total << ", random access time = " << difference1
                              << ", sequential access time = " << difference2 << endl << endl;

    one_sec_delay();

    total = 0;
    csrand(seed);
    current_time = get_time_in_ms();
    cbits bits2 = cbits_with_size(N, false);

    c_forrange (40 * N)
    {
        uint64_t r = crand();
        cbits_set_value(&bits2, r & (N-1), r & 1<<29);
    }

    difference1 = get_time_in_ms() - current_time;
    current_time = get_time_in_ms();

    c_forrange (100) c_forrange (num, N)
    {
        total += cbits_at(&bits2, num);
    }

    cbits_drop(&bits2);

    difference2 = get_time_in_ms() - current_time;

    cout << "cbits:" << endl << "sum total = " << total << ", random access time = " << difference1
                              << ", sequential access time = " << difference2 << endl << endl;

    //cin.get();

    return 0;
}
