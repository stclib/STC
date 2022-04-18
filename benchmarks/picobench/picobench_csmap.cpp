#include <iostream>
#include <stc/crandom.h>
#include <stc/cstr.h>
#include <cmath>
#include <string>
#include <map>

#define PICOBENCH_IMPLEMENT_WITH_MAIN
#include "picobench.hpp"

enum {N1 = 1000000, S1 = 1};
uint64_t seed = time(NULL); // 18237129837891;

using omap_i = std::map<int, int>;
using omap_x = std::map<uint64_t, uint64_t>;
using omap_s = std::map<std::string, std::string>;

#define i_key int
#define i_val int
#define i_tag i
#include <stc/csmap.h>

#define i_key size_t
#define i_val size_t
#define i_tag x
#include <stc/csmap.h>

#define i_key_str
#define i_val_str
#include <stc/csmap.h>

PICOBENCH_SUITE("Map1");

template <class MapInt>
static void ctor_and_ins_one_i(picobench::state& s)
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

static void ctor_and_ins_one_csmap_i(picobench::state& s)
{
    size_t result = 0;
    picobench::scope scope(s);
    c_forrange (n, s.iterations()) {
        csmap_i map = csmap_i_init();
        csmap_i_insert(&map, n, 0);
        result += csmap_i_size(map);
        csmap_i_drop(&map);
    }
    s.set_result(result);
}

#define P samples(S1).iterations({N1})
//PICOBENCH(ctor_and_ins_one_i<omap_i>).P;
//PICOBENCH(ctor_and_ins_one_csmap_i).P;
#undef P


PICOBENCH_SUITE("Map_insert_only");

template <class MapInt>
static void insert_i(picobench::state& s)
{
    size_t result = 0;
    MapInt map;
    csrandom(seed);
    picobench::scope scope(s);
    c_forrange (n, s.iterations())
        map.emplace(crandom() & 0xfffffff, n);
    s.set_result(map.size());
}

static void insert_csmap_i(picobench::state& s)
{
    size_t result = 0;
    csmap_i map = csmap_i_init();
    csrandom(seed);
    picobench::scope scope(s);
    c_forrange (n, s.iterations())
        csmap_i_insert(&map, crandom() & 0xfffffff, n);
    s.set_result(csmap_i_size(map));
    csmap_i_drop(&map);
}

#define P samples(S1).iterations({N1})
PICOBENCH(insert_i<omap_i>).P;
PICOBENCH(insert_csmap_i).P;
#undef P


PICOBENCH_SUITE("Map2");

template <class MapInt>
static void ins_and_erase_i(picobench::state& s)
{
    size_t result = 0;
    uint64_t mask = (1ull << s.arg()) - 1;
    MapInt map;
    csrandom(seed);

    picobench::scope scope(s);
    c_forrange (i, s.iterations())
        map.emplace(crandom() & mask, i);
    result = map.size();

    map.clear();
    csrandom(seed);
    c_forrange (i, s.iterations())
        map[crandom() & mask] = i;

    csrandom(seed);
    c_forrange (s.iterations())
        map.erase(crandom() & mask);
    s.set_result(result);
}

static void ins_and_erase_csmap_i(picobench::state& s)
{
    size_t result = 0;
    uint64_t mask = (1ull << s.arg()) - 1;
    csmap_i map = csmap_i_init();
    csrandom(seed);

    picobench::scope scope(s);
    c_forrange (i, s.iterations())
        csmap_i_insert(&map, crandom() & mask, i);
    result = csmap_i_size(map);

    csmap_i_clear(&map);
    csrandom(seed);
    c_forrange (i, s.iterations())
        csmap_i_insert_or_assign(&map, crandom() & mask, i);

    csrandom(seed);
    c_forrange (s.iterations())
        csmap_i_erase(&map, crandom() & mask);
    s.set_result(result);
    csmap_i_drop(&map);
}

#define P samples(S1).iterations({N1/2, N1/2, N1/2, N1/2}).args({18, 23, 25, 31})
PICOBENCH(ins_and_erase_i<omap_i>).P;
PICOBENCH(ins_and_erase_csmap_i).P;
#undef P

PICOBENCH_SUITE("Map3");

template <class MapInt>
static void ins_and_access_i(picobench::state& s)
{
    uint64_t mask = (1ull << s.arg()) - 1;
    size_t result = 0;
    MapInt map;
    csrandom(seed);

    picobench::scope scope(s);
    c_forrange (s.iterations()) {
        result += ++map[crandom() & mask];
        auto it = map.find(crandom() & mask);
        if (it != map.end()) map.erase(it->first);
    }
    s.set_result(result + map.size());
}

static void ins_and_access_csmap_i(picobench::state& s)
{
    uint64_t mask = (1ull << s.arg()) - 1;
    size_t result = 0;
    csmap_i map = csmap_i_init();
    csrandom(seed);

    picobench::scope scope(s);
    c_forrange (s.iterations()) {
        result += ++csmap_i_insert(&map, crandom() & mask, 0).ref->second;
        const csmap_i_value* val = csmap_i_get(&map, crandom() & mask);
        if (val) csmap_i_erase(&map, val->first);
    }
    s.set_result(result + csmap_i_size(map));
    csmap_i_drop(&map);
}

#define P samples(S1).iterations({N1, N1, N1, N1}).args({18, 23, 25, 31})
PICOBENCH(ins_and_access_i<omap_i>).P;
PICOBENCH(ins_and_access_csmap_i).P;
#undef P

PICOBENCH_SUITE("Map4");

static void randomize(char* str, size_t len) {
    union {uint64_t i; char c[8];} r = {.i = crandom()};
    for (int i = len - 7, j = 0; i < len; ++j, ++i)
        str[i] = (r.c[j] & 63) + 48;
}

template <class MapStr>
static void ins_and_access_s(picobench::state& s)
{
    std::string str(s.arg(), 'x');
    size_t result = 0;
    MapStr map;

    picobench::scope scope(s);
    csrandom(seed);
    c_forrange (s.iterations()) {
        randomize(&str[0], str.size());
        map.emplace(str, str);
    }
    csrandom(seed);
    c_forrange (s.iterations()) {
        randomize(&str[0], str.size());
        result += map.erase(str);
    }
    s.set_result(result + map.size());
}

static void ins_and_access_csmap_s(picobench::state& s)
{
    cstr str = cstr_with_size(s.arg(), 'x');
    char* buf = cstr_data(&str);
    size_t result = 0;
    csmap_str map = csmap_str_init();

    picobench::scope scope(s);
    csrandom(seed);
    c_forrange (s.iterations()) {
        randomize(buf, s.arg());
        csmap_str_emplace(&map, buf, buf);
    }
    csrandom(seed);
    c_forrange (s.iterations()) {
        randomize(buf, s.arg());
        result += csmap_str_erase(&map, buf);
        /*csmap_str_iter it = csmap_str_find(&map, buf);
        if (it.ref) {
            ++result;
            csmap_str_erase(&map, cstr_str(&it.ref->first));
        }*/
    }
    s.set_result(result + csmap_str_size(map));
    cstr_drop(&str);
    csmap_str_drop(&map);
}

#define P samples(S1).iterations({N1/5, N1/5, N1/5, N1/10, N1/40}).args({13, 7, 8, 100, 1000})
PICOBENCH(ins_and_access_s<omap_s>).P;
PICOBENCH(ins_and_access_csmap_s).P;
#undef P

PICOBENCH_SUITE("Map5");

template <class MapX>
static void iterate_x(picobench::state& s)
{
    MapX map;
    uint64_t K = (1ull << s.arg()) - 1;

    picobench::scope scope(s);
    csrandom(seed);
    size_t result = 0;

    // measure insert then iterate whole map
    c_forrange (n, s.iterations()) {
        map[crandom()] = n;
        if (!(n & K)) for (auto const& keyVal : map)
            result += keyVal.second;
    }

    // reset rng back to inital state
    csrandom(seed);

    // measure erase then iterate whole map
    c_forrange (n, s.iterations()) {
        map.erase(crandom());
        if (!(n & K)) for (auto const& keyVal : map)
            result += keyVal.second;
    }
    s.set_result(result);
}

static void iterate_csmap_x(picobench::state& s)
{
    csmap_x map = csmap_x_init();
    uint64_t K = (1ull << s.arg()) - 1;

    picobench::scope scope(s);
    csrandom(seed);
    size_t result = 0;

    // measure insert then iterate whole map
    c_forrange (n, s.iterations()) {
        csmap_x_insert_or_assign(&map, crandom(), n);
        if (!(n & K)) c_foreach (i, csmap_x, map)
            result += i.ref->second;
    }

    // reset rng back to inital state
    csrandom(seed);

    // measure erase then iterate whole map
    c_forrange (n, s.iterations()) {
        csmap_x_erase(&map, crandom());
        if (!(n & K)) c_foreach (i, csmap_x, map)
            result += i.ref->second;
    }
    s.set_result(result);
    csmap_x_drop(&map);
}


#define P samples(S1).iterations({N1/20}).args({12})
//PICOBENCH(iterate_x<omap_x>).P;
//PICOBENCH(iterate_csmap_x).P;
#undef P
