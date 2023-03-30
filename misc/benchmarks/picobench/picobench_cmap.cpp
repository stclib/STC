#define i_static
#include <stc/crand.h>
#define i_static
#include <stc/cstr.h>
#include <cmath>
#include <string>
#include <unordered_map>
#include <stdexcept>
#include "../external/ankerl/unordered_dense.h"
#include "../external/skarupke/flat_hash_map.hpp"
#include "../external/tsl/robin_map.h"

#define PICOBENCH_IMPLEMENT_WITH_MAIN
#include "picobench.hpp"

enum {N1 = 4000000, S1 = 1, MaxLoadFactor100 = 80};
uint64_t seed = time(NULL);

template <class K, class V> using umap = std::unordered_map<K, V>;
template <class K, class V> using fmap = ska::flat_hash_map<K, V>;
template <class K, class V> using tmap = tsl::robin_map<K, V>;
template <class K, class V> using dmap = ankerl::unordered_dense::map<K, V>;
#define DEFMAP(map, ...) \
    using u##map = umap __VA_ARGS__; \
    using f##map = fmap __VA_ARGS__; \
    using t##map = tmap __VA_ARGS__; \
    using d##map = dmap __VA_ARGS__

DEFMAP(map_i, <int32_t, int32_t>);
DEFMAP(map_x, <uint64_t, uint64_t>);
DEFMAP(map_s, <std::string, std::string>);

#define i_key int32_t
#define i_val int32_t
#define i_tag i
#define i_max_load_factor float(MaxLoadFactor100) / 100.0f
#include <stc/cmap.h>

#define i_key uint64_t
#define i_val uint64_t
#define i_tag x
#define i_max_load_factor float(MaxLoadFactor100) / 100.0f
#include <stc/cmap.h>

#define i_key_str
#define i_val_str
#define i_max_load_factor float(MaxLoadFactor100) / 100.0f
#include <stc/cmap.h>

PICOBENCH_SUITE("Map1");

template <class MapInt>
static void ins_and_erase_i(picobench::state& s)
{
    MapInt map;
    map.max_load_factor((int)MaxLoadFactor100 / 100.0);
    csrand(seed);

    picobench::scope scope(s);
    c_forrange (s.iterations())
        map[crand()];
    map.clear();
    csrand(seed);
    c_forrange (s.iterations())
        map[crand()];
    csrand(seed);
    c_forrange (s.iterations())
        map.erase(crand());
    s.set_result(map.size());
}
/*
static void ins_and_erase_cmap_i(picobench::state& s)
{
    cmap_i map = cmap_i_init();
    csrand(seed);

    picobench::scope scope(s);
    c_forrange (s.iterations())
        cmap_i_insert(&map, crand(), 0);
    cmap_i_clear(&map);
    csrand(seed);
    c_forrange (s.iterations())
        cmap_i_insert(&map, crand(), 0);
    csrand(seed);
    c_forrange (s.iterations())
        cmap_i_erase(&map, crand());
    s.set_result(cmap_i_size(&map));
    cmap_i_drop(&map);
}
*/
static void ins_and_erase_cmap_x(picobench::state& s)
{
    cmap_x map = cmap_x_init();
    csrand(seed);

    picobench::scope scope(s);
    c_forrange (s.iterations())
        cmap_x_insert(&map, crand(), 0);
    cmap_x_clear(&map);
    csrand(seed);
    c_forrange (s.iterations())
        cmap_x_insert(&map, crand(), 0);
    csrand(seed);
    c_forrange (s.iterations())
        cmap_x_erase(&map, crand());
    s.set_result(cmap_x_size(&map));
    cmap_x_drop(&map);
}

#define P samples(S1).iterations({N1/4})
PICOBENCH(ins_and_erase_i<umap_x>).P;
PICOBENCH(ins_and_erase_i<dmap_x>).P;
PICOBENCH(ins_and_erase_i<fmap_x>).P;
PICOBENCH(ins_and_erase_i<tmap_x>).P;
PICOBENCH(ins_and_erase_cmap_x).P;
#undef P

PICOBENCH_SUITE("Map2");

template <class MapInt>
static void ins_and_access_i(picobench::state& s)
{
    uint64_t mask = (1ull << s.arg()) - 1;
    size_t result = 0;
    MapInt map;
    map.max_load_factor((int)MaxLoadFactor100 / 100.0);
    csrand(seed);

    picobench::scope scope(s);
    c_forrange (N1)
        result += ++map[crand() & mask];
    s.set_result(result);
}

static void ins_and_access_cmap_i(picobench::state& s)
{
    uint64_t mask = (1ull << s.arg()) - 1;
    size_t result = 0;
    cmap_i map = cmap_i_init();
    csrand(seed);

    picobench::scope scope(s);
    c_forrange (N1)
        result += ++cmap_i_insert(&map, crand() & mask, 0).ref->second;
    s.set_result(result);
    cmap_i_drop(&map);
}

#define P samples(S1).iterations({N1, N1, N1, N1}).args({18, 23, 25, 31})
PICOBENCH(ins_and_access_i<umap_i>).P;
PICOBENCH(ins_and_access_i<dmap_i>).P;
PICOBENCH(ins_and_access_i<fmap_i>).P;
PICOBENCH(ins_and_access_i<tmap_i>).P;
PICOBENCH(ins_and_access_cmap_i).P;
#undef P

PICOBENCH_SUITE("Map3");

static void randomize(char* str, size_t len) {
    for (size_t k=0; k < len; ++k) {
        union {uint64_t i; char c[8];} r = {.i = crand()};
        for (unsigned i=0; i<8 && k<len; ++k, ++i)
            str[k] = (r.c[i] & 63) + 48;
    }
}

template <class MapStr>
static void ins_and_access_s(picobench::state& s)
{
    std::string str(s.arg(), 'x');
    size_t result = 0;
    MapStr map;
    map.max_load_factor((int)MaxLoadFactor100 / 100.0);
    csrand(seed);

    picobench::scope scope(s);
    c_forrange (s.iterations()) {
        randomize(&str[0], str.size());
        map.emplace(str, str);
        randomize(&str[0], str.size());
        result += map.erase(str);
    }
    s.set_result(result + map.size());
}

static void ins_and_access_cmap_s(picobench::state& s)
{
    cstr str = cstr_with_size(s.arg(), 'x');
    char* buf = cstr_data(&str);
    size_t result = 0;
    cmap_str map = cmap_str_init();
    csrand(seed);

    picobench::scope scope(s);
    c_forrange (s.iterations()) {
        randomize(buf, s.arg());
        //if (s.arg() > 30) { printf("%s\n", buf); exit(0); }
        cmap_str_emplace(&map, buf, buf);

        randomize(buf, s.arg());
        result += cmap_str_erase(&map, buf);
    }
    s.set_result(result + cmap_str_size(&map));
    cstr_drop(&str);
    cmap_str_drop(&map);
}

#define P samples(S1).iterations({N1/5, N1/5, N1/5, N1/10, N1/40}).args({13, 7, 8, 100, 1000})
PICOBENCH(ins_and_access_s<umap_s>).P;
PICOBENCH(ins_and_access_s<dmap_s>).P;
PICOBENCH(ins_and_access_s<fmap_s>).P;
PICOBENCH(ins_and_access_s<tmap_s>).P;
PICOBENCH(ins_and_access_cmap_s).P;
#undef P

PICOBENCH_SUITE("Map4");

template <class MapX>
static void iterate_x(picobench::state& s)
{
    MapX map;
    map.max_load_factor((int)MaxLoadFactor100 / 100.0);
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

static void iterate_cmap_x(picobench::state& s)
{
    cmap_x map = cmap_x_init();
    uint64_t K = (1ull << s.arg()) - 1;

    picobench::scope scope(s);
    csrand(seed);
    size_t result = 0;

    // measure insert then iterate whole map
    c_forrange (n, s.iterations()) {
        cmap_x_insert_or_assign(&map, crand(), n);
        if (!(n & K)) c_foreach (i, cmap_x, map)
            result += i.ref->second;
    }

    // reset rng back to inital state
    csrand(seed);

    // measure erase then iterate whole map
    c_forrange (n, s.iterations()) {
        cmap_x_erase(&map, crand());
        if (!(n & K)) c_foreach (i, cmap_x, map)
            result += i.ref->second;
    }
    s.set_result(result);
    cmap_x_drop(&map);
}


#define P samples(S1).iterations({N1/20}).args({12})
PICOBENCH(iterate_x<umap_x>).P;
PICOBENCH(iterate_x<dmap_x>).P;
PICOBENCH(iterate_x<fmap_x>).P;
PICOBENCH(iterate_x<tmap_x>).P;
PICOBENCH(iterate_cmap_x).P;
#undef P
