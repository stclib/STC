#include <iostream>
#define i_static
#include "stc/crand.h"
#define i_static
#include "stc/cstr.h"
#include <cmath>
#include <string>
#include <map>

#define PICOBENCH_IMPLEMENT_WITH_MAIN
#include "picobench.hpp"

enum {N1 = 1000000, S1 = 1};
uint64_t seed = time(NULL); // 18237129837891;

using omap_i32 = std::map<int, int>;
using omap_u64 = std::map<uint64_t, uint64_t>;
using omap_str = std::map<std::string, std::string>;

#define i_TYPE smap_i32, int32_t, int32_t
#include "stc/smap.h"

#define i_TYPE smap_u64, uint64_t, uint64_t
#include "stc/smap.h"

#define i_key_cstr
#define i_val_cstr
#include "stc/smap.h"

PICOBENCH_SUITE("Map1");

template <class MapInt>
static void ctor_and_insert_one_i32(picobench::state& s)
{
    size_t result = 0;
    picobench::scope scope(s);
    c_forrange (n, s.iterations()) {
        MapInt map;
        map[n];
        result += map.size();
    }
    s.set_result(result);
}
/*
static void ctor_and_insert_one_smap_i32(picobench::state& s)
{
    size_t result = 0;
    picobench::scope scope(s);
    c_forrange (n, s.iterations()) {
        smap_i32 map = {0};
        smap_i32_insert(&map, n, 0);
        result += smap_i32_size(&map);
        smap_i32_drop(&map);
    }
    s.set_result(result);
}

#define P samples(S1).iterations({N1})
PICOBENCH(ctor_and_insert_one_i32<omap_i32>).P;
PICOBENCH(ctor_and_insert_one_smap_i32).P;
#undef P
*/

PICOBENCH_SUITE("Map_insert_only");

template <class MapInt>
static void insert_i32(picobench::state& s)
{
    MapInt map;
    csrand(seed);
    picobench::scope scope(s);
    c_forrange (n, s.iterations())
        map.emplace(crand() & 0xfffffff, n);
    s.set_result(map.size());
}

static void insert_smap_i32(picobench::state& s)
{
    smap_i32 map = {0};
    csrand(seed);
    picobench::scope scope(s);
    c_forrange (n, s.iterations())
        smap_i32_insert(&map, crand() & 0xfffffff, n);
    s.set_result(smap_i32_size(&map));
    smap_i32_drop(&map);
}

#define P samples(S1).iterations({N1})
PICOBENCH(insert_i32<omap_i32>).P;
PICOBENCH(insert_smap_i32).P;
#undef P


PICOBENCH_SUITE("Map2");

template <class MapInt>
static void insert_and_erase_i32(picobench::state& s)
{
    size_t result = 0;
    uint64_t mask = (1ull << s.arg()) - 1;
    MapInt map;
    csrand(seed);

    picobench::scope scope(s);
    c_forrange (i, s.iterations())
        map.emplace(crand() & mask, i);
    result = map.size();

    map.clear();
    csrand(seed);
    c_forrange (i, s.iterations())
        map[crand() & mask] = i;

    csrand(seed);
    c_forrange (s.iterations())
        map.erase(crand() & mask);
    s.set_result(result);
}

static void insert_and_erase_smap_i32(picobench::state& s)
{
    size_t result = 0;
    uint64_t mask = (1ull << s.arg()) - 1;
    smap_i32 map = {0};
    csrand(seed);

    picobench::scope scope(s);
    c_forrange (i, s.iterations())
        smap_i32_insert(&map, crand() & mask, i);
    result = smap_i32_size(&map);

    smap_i32_clear(&map);
    csrand(seed);
    c_forrange (i, s.iterations())
        smap_i32_insert_or_assign(&map, crand() & mask, i);

    csrand(seed);
    c_forrange (s.iterations())
        smap_i32_erase(&map, crand() & mask);
    s.set_result(result);
    smap_i32_drop(&map);
}

#define P samples(S1).iterations({N1/2, N1/2, N1/2, N1/2}).args({18, 23, 25, 31})
PICOBENCH(insert_and_erase_i32<omap_i32>).P;
PICOBENCH(insert_and_erase_smap_i32).P;
#undef P

PICOBENCH_SUITE("Map3");

template <class MapInt>
static void insert_and_access_i32(picobench::state& s)
{
    uint64_t mask = (1ull << s.arg()) - 1;
    size_t result = 0;
    MapInt map;
    csrand(seed);

    picobench::scope scope(s);
    c_forrange (s.iterations()) {
        result += ++map[crand() & mask];
        auto it = map.find(crand() & mask);
        if (it != map.end()) map.erase(it->first);
    }
    s.set_result(result + map.size());
}

static void insert_and_access_smap_i32(picobench::state& s)
{
    uint64_t mask = (1ull << s.arg()) - 1;
    size_t result = 0;
    smap_i32 map = {0};
    csrand(seed);

    picobench::scope scope(s);
    c_forrange (s.iterations()) {
        result += ++smap_i32_insert(&map, crand() & mask, 0).ref->second;
        const smap_i32_value* val = smap_i32_get(&map, crand() & mask);
        if (val) smap_i32_erase(&map, val->first);
    }
    s.set_result(result + smap_i32_size(&map));
    smap_i32_drop(&map);
}

#define P samples(S1).iterations({N1, N1, N1, N1}).args({18, 23, 25, 31})
PICOBENCH(insert_and_access_i32<omap_i32>).P;
PICOBENCH(insert_and_access_smap_i32).P;
#undef P

PICOBENCH_SUITE("Map4");

static void randomize(char* str, int len) {
    union {uint64_t i; char c[8];} r = {.i = crand()};
    for (int i = len - 7, j = 0; i < len; ++j, ++i)
        str[i] = (r.c[j] & 63) + 48;
}

template <class MapStr>
static void insert_and_access_str(picobench::state& s)
{
    std::string str(s.arg(), 'x');
    size_t result = 0;
    MapStr map;

    picobench::scope scope(s);
    csrand(seed);
    c_forrange (s.iterations()) {
        randomize(&str[0], str.size());
        map.emplace(str, str);
    }
    csrand(seed);
    c_forrange (s.iterations()) {
        randomize(&str[0], str.size());
        result += map.erase(str);
    }
    s.set_result(result + map.size());
}

static void insert_and_access_smap_cstr(picobench::state& s)
{
    cstr str = cstr_with_size(s.arg(), 'x');
    char* buf = cstr_data(&str);
    size_t result = 0;
    smap_cstr map = {0};

    picobench::scope scope(s);
    csrand(seed);
    c_forrange (s.iterations()) {
        randomize(buf, s.arg());
        smap_cstr_emplace(&map, buf, buf);
    }
    csrand(seed);
    c_forrange (s.iterations()) {
        randomize(buf, s.arg());
        result += smap_cstr_erase(&map, buf);
        /*smap_cstr_iter it = smap_cstr_find(&map, buf);
        if (it.ref) {
            ++result;
            smap_cstr_erase(&map, cstr_str(&it.ref->first));
        }*/
    }
    s.set_result(result + smap_cstr_size(&map));
    cstr_drop(&str);
    smap_cstr_drop(&map);
}

#define P samples(S1).iterations({N1/5, N1/5, N1/5, N1/10, N1/40}).args({13, 7, 8, 100, 1000})
PICOBENCH(insert_and_access_str<omap_str>).P;
PICOBENCH(insert_and_access_smap_cstr).P;
#undef P

PICOBENCH_SUITE("Map5");

template <class MapX>
static void iterate_u64(picobench::state& s)
{
    MapX map;
    uint64_t K = (1ull << s.arg()) - 1;

    picobench::scope scope(s);
    csrand(seed);
    size_t result = 0;

    // measure insert then iterate whole map
    c_forrange (n, s.iterations()) {
        map[crand()] = n;
        if (!(n & K)) for (auto const& keyVal : map)
            result += keyVal.second;
    }

    // reset rng back to inital state
    csrand(seed);

    // measure erase then iterate whole map
    c_forrange (n, s.iterations()) {
        map.erase(crand());
        if (!(n & K)) for (auto const& keyVal : map)
            result += keyVal.second;
    }
    s.set_result(result);
}
/*
static void iterate_smap_u64(picobench::state& s)
{
    smap_u64 map = {0};
    uint64_t K = (1ull << s.arg()) - 1;

    picobench::scope scope(s);
    csrand(seed);
    size_t result = 0;

    // measure insert then iterate whole map
    c_forrange (n, s.iterations()) {
        smap_u64_insert_or_assign(&map, crand(), n);
        if (!(n & K)) c_foreach (i, smap_u64, map)
            result += i.ref->second;
    }

    // reset rng back to inital state
    csrand(seed);

    // measure erase then iterate whole map
    c_forrange (n, s.iterations()) {
        smap_u64_erase(&map, crand());
        if (!(n & K)) c_foreach (i, smap_u64, map)
            result += i.ref->second;
    }
    s.set_result(result);
    smap_u64_drop(&map);
}

#define P samples(S1).iterations({N1/20}).args({12})
PICOBENCH(iterate_u64<omap_u64>).P;
PICOBENCH(iterate_smap_u64).P;
#undef P
*/