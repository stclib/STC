#include <iostream>
#include <stc/crandom.h>
#include <stc/cstr.h>
#include <stc/csmap.h>
#include <cmath>
#include <string>
#include <map>

#define PICOBENCH_IMPLEMENT_WITH_MAIN
#include "picobench.hpp"

enum {N1 = 3000000, S1 = 1};
uint64_t seed = time(NULL); // 18237129837891;

using omap_i = std::map<int, int>;
using omap_x = std::map<uint64_t, uint64_t>;
using omap_s = std::map<std::string, std::string>;

using_csmap(i, int, int);
using_csmap(x, uint64_t, uint64_t);
using_csmap_strkey(s, cstr, cstr_del, cstr_clone);

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
        csmap_i_emplace(&map, n, 0);
        result += csmap_i_size(map);
        csmap_i_del(&map);
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
    stc64_srandom(seed);
    s.start_timer();
    c_forrange (n, s.iterations())
        map.emplace(stc64_random() & 0xfffffff, n);
    s.set_result(map.size());
    s.stop_timer();
}

static void insert_csmap_i(picobench::state& s)
{
    size_t result = 0;
    csmap_i map = csmap_i_init();
    stc64_srandom(seed);
    s.start_timer();
    c_forrange (n, s.iterations())
        csmap_i_emplace(&map, stc64_random() & 0xfffffff, n);
    s.set_result(csmap_i_size(map));
    s.stop_timer();
    csmap_i_del(&map);
}

#define P samples(S1).iterations({N1})
//PICOBENCH(insert_i<omap_i>).P;
//PICOBENCH(insert_csmap_i).P;
#undef P


PICOBENCH_SUITE("Map2");

template <class MapInt>
static void ins_and_erase_i(picobench::state& s)
{
    size_t result = 0;
    uint64_t mask = (1ull << s.arg()) - 1;
    MapInt map;
    stc64_srandom(seed);

    picobench::scope scope(s);
    c_forrange (i, s.iterations())
        map.emplace(stc64_random() & mask, i);
    result = map.size();

    map.clear();
    stc64_srandom(seed);
    c_forrange (i, s.iterations())
        map[stc64_random() & mask] = i;

    stc64_srandom(seed);
    c_forrange (s.iterations())
        map.erase(stc64_random() & mask);
    s.set_result(result);
}

static void ins_and_erase_csmap_i(picobench::state& s)
{
    size_t result = 0;
    uint64_t mask = (1ull << s.arg()) - 1;
    csmap_i map = csmap_i_init();
    stc64_srandom(seed);

    picobench::scope scope(s);
    c_forrange (i, s.iterations())
        csmap_i_emplace(&map, stc64_random() & mask, i);
    result = csmap_i_size(map);

    csmap_i_clear(&map);
    stc64_srandom(seed);
    c_forrange (i, s.iterations())
        csmap_i_put(&map, stc64_random() & mask, i);

    stc64_srandom(seed);
    c_forrange (s.iterations())
        csmap_i_erase(&map, stc64_random() & mask);
    s.set_result(result);
    csmap_i_del(&map);
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
    stc64_srandom(seed);

    picobench::scope scope(s);
    c_forrange (N1) {
        result += ++map[stc64_random() & mask];
        auto it = map.find(stc64_random() & mask);
        if (it != map.end()) map.erase(it->first);
    }
    s.set_result(result + map.size());
}

static void ins_and_access_csmap_i(picobench::state& s)
{
    uint64_t mask = (1ull << s.arg()) - 1;
    size_t result = 0;
    csmap_i map = csmap_i_init();
    stc64_srandom(seed);

    picobench::scope scope(s);
    c_forrange (N1) {
        result += ++csmap_i_emplace(&map, stc64_random() & mask, 0).first->second;
        csmap_i_value_t* val = csmap_i_find(&map, stc64_random() & mask);
        if (val) csmap_i_erase(&map, val->first);
    }
    s.set_result(result + csmap_i_size(map));
    csmap_i_del(&map);
}

#define P samples(S1).iterations({N1, N1, N1, N1}).args({18, 23, 25, 31})
PICOBENCH(ins_and_access_i<omap_i>).P;
PICOBENCH(ins_and_access_csmap_i).P;
#undef P

PICOBENCH_SUITE("Map4");

static void randomize(char* str, size_t len) {
    union {uint64_t i; char c[8];} r = {.i = stc64_random()};
    for (int i = len - 7, j = 0; i < len; ++j, ++i)
        str[i] = (r.c[j] & 63) + 48;
}

template <class MapStr>
static void ins_and_access_s(picobench::state& s)
{
    std::string str(s.arg(), 'x');
    size_t result = 0;
    MapStr map;
    stc64_srandom(seed);

    picobench::scope scope(s);
    c_forrange (s.iterations()) {
        randomize(&str[0], str.size());
        map[str] = str;
        randomize(&str[0], str.size());
        auto it = map.find(str);
        if (it != map.end()) {
            ++result;
            map.erase(it);
        }
    }
    s.set_result(result + map.size());
}

static void ins_and_access_csmap_s(picobench::state& s)
{
    cstr str = cstr_with_size(s.arg(), 'x');
    size_t result = 0;
    csmap_s map = csmap_s_init();
    stc64_srandom(seed);

    picobench::scope scope(s);
    c_forrange (s.iterations()) {
        randomize(str.str, cstr_size(str));
        csmap_s_put(&map, str.str, cstr_clone(str));
        randomize(str.str, cstr_size(str));
        csmap_s_value_t* val = csmap_s_find(&map, str.str);
        if (val) {
            ++result;
            csmap_s_erase(&map, val->first.str);
        }
    }
    s.set_result(result + csmap_s_size(map));
    cstr_del(&str);
    csmap_s_del(&map);
}

#define P samples(S1).iterations({N1/5, N1/5, N1/5, N1/10, N1/40}).args({13, 7, 8, 100, 1000})
//PICOBENCH(ins_and_access_s<omap_s>).P;
//PICOBENCH(ins_and_access_csmap_s).P;
#undef P

PICOBENCH_SUITE("Map5");

template <class MapX>
static void iterate_x(picobench::state& s)
{
    MapX map;
    uint64_t K = (1ull << s.arg()) - 1;

    picobench::scope scope(s);
    stc64_srandom(seed);
    size_t result = 0;

    // measure insert then iterate whole map
    c_forrange (n, s.iterations()) {
        map[stc64_random()] = n;
        if (!(n & K)) for (auto const& keyVal : map)
            result += keyVal.second;
    }

    // reset rng back to inital state
    stc64_srandom(seed);

    // measure erase then iterate whole map
    c_forrange (n, s.iterations()) {
        map.erase(stc64_random());
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
    stc64_srandom(seed);
    size_t result = 0;

    // measure insert then iterate whole map
    c_forrange (n, s.iterations()) {
        csmap_x_put(&map, stc64_random(), n);
        if (!(n & K)) c_foreach (i, csmap_x, map)
            result += i.ref->second;
    }

    // reset rng back to inital state
    stc64_srandom(seed);

    // measure erase then iterate whole map
    c_forrange (n, s.iterations()) {
        csmap_x_erase(&map, stc64_random());
        if (!(n & K)) c_foreach (i, csmap_x, map)
            result += i.ref->second;
    }
    s.set_result(result);
    csmap_x_del(&map);
}


#define P samples(S1).iterations({N1/20}).args({12})
//PICOBENCH(iterate_x<omap_x>).P;
//PICOBENCH(iterate_csmap_x).P;
#undef P
