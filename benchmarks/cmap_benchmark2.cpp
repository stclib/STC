#include <stc/crand.h>
#include <stc/cstr.h>
#include <stc/cmap.h>

#include <string>
#include <unordered_map>
#include "others/bytell_hash_map.hpp"
#include "others/robin_hood.hpp"
#include "others/hopscotch_map.h"
#include "others/sparsepp/spp.h"

#define PICOBENCH_IMPLEMENT_WITH_MAIN
#include "picobench.hpp"

PICOBENCH_SUITE("Map");
enum {N1 = 5000000};
uint64_t seed = 123456;

static inline uint32_t fibonacci_hash(const void* data, size_t len) {
    return (uint32_t) (((*(const uint32_t *) data) * 11400714819323198485llu) >> 24);
}
template <class K, class V> using umap = std::unordered_map<K, V>;
template <class K, class V> using bmap = ska::bytell_hash_map<K, V>;
template <class K, class V> using fmap = ska::flat_hash_map<K, V>;
template <class K, class V> using hmap = tsl::hopscotch_map<K, V>;
template <class K, class V> using smap = spp::sparse_hash_map<K, V>;
template <class K, class V> using rmap = robin_hood::unordered_flat_map<K, V>;

using_cmap(i, int, int, c_default_del, c_default_clone, c_default_equals, fibonacci_hash);
using_cmap_strkey(s, cstr, cstr_del, cstr_clone);
using umap_i = umap<int, int>;
using umap_s = umap<std::string, std::string>;
using bmap_i = bmap<int, int>;
using bmap_s = bmap<std::string, std::string>;
using fmap_i = fmap<int, int>;
using fmap_s = fmap<std::string, std::string>;
using hmap_i = hmap<int, int>;
using hmap_s = hmap<std::string, std::string>;
using smap_i = smap<int, int>;
using smap_s = smap<std::string, std::string>;
using rmap_i = rmap<int, int>;
using rmap_s = rmap<std::string, std::string>;

template <class MapInt>
static void ctor_and_ins_one(picobench::state& s)
{
    picobench::scope scope(s);
    size_t result = 0;
    c_forrange (n, s.iterations()) {
        MapInt map;
        map[n];
        result += map.size();
    }
    s.set_result(result);
}

static void ctor_and_ins_one_cmap_i_(picobench::state& s)
{
    picobench::scope scope(s);
    size_t result = 0;
    c_forrange (n, s.iterations()) {
        cmap_i map = cmap_inits;
        cmap_i_emplace(&map, n, 0);
        result += cmap_i_size(map);
        cmap_i_del(&map);
    }
    s.set_result(result);
}

#define P samples(2).iterations({N1})
PICOBENCH(ctor_and_ins_one<umap_i>).P.baseline();
PICOBENCH(ctor_and_ins_one<bmap_i>).P;
PICOBENCH(ctor_and_ins_one<fmap_i>).P;
PICOBENCH(ctor_and_ins_one<hmap_i>).P;
PICOBENCH(ctor_and_ins_one<smap_i>).P;
PICOBENCH(ctor_and_ins_one<rmap_i>).P;
PICOBENCH(ctor_and_ins_one_cmap_i_).P;
#undef P


template <class MapInt>
static void ins_and_erase(picobench::state& s)
{
    stc64_srandom(seed);
    picobench::scope scope(s);
    size_t result = 0;
    MapInt map;
    c_forrange (s.iterations())
        map[stc64_random()];
    map.clear();
    stc64_srandom(seed);
    c_forrange (s.iterations())
        map[stc64_random()];
    stc64_srandom(seed);
    c_forrange (s.iterations())
        map.erase(stc64_random());
    s.set_result(map.size());
}

static void ins_and_erase_cmap_i_(picobench::state& s)
{
    stc64_srandom(seed);
    picobench::scope scope(s);
    cmap_i map = cmap_inits;
    cmap_i_set_load_factors(&map, 0.0, 0.80);
    c_forrange (s.iterations())
        cmap_i_emplace(&map, stc64_random(), 0);
    cmap_i_clear(&map);
    stc64_srandom(seed);
    c_forrange (s.iterations())
        cmap_i_emplace(&map, stc64_random(), 0);
    stc64_srandom(seed);
    c_forrange (s.iterations())
        cmap_i_erase(&map, stc64_random());
    cmap_i_del(&map);
    s.set_result(cmap_i_size(map));
}

#define P samples(2).iterations({N1/4})
PICOBENCH(ins_and_erase<umap_i>).P.baseline();
PICOBENCH(ins_and_erase<bmap_i>).P;
PICOBENCH(ins_and_erase<fmap_i>).P;
PICOBENCH(ins_and_erase<hmap_i>).P;
PICOBENCH(ins_and_erase<smap_i>).P;
PICOBENCH(ins_and_erase<rmap_i>).P;
PICOBENCH(ins_and_erase_cmap_i_).P;
#undef P


template <class MapInt>
static void ins_and_access(picobench::state& s)
{
    stc64_srandom(seed);
    uint64_t filter = (1ull << s.user_data()) - 1;
    picobench::scope scope(s);
    size_t result = 0;
    MapInt map;
    c_forrange (N1)
        result += ++map[stc64_random() & filter];
    s.set_result(result);
}

static void ins_and_access_cmap_i_(picobench::state& s)
{
    stc64_srandom(seed);
    uint64_t filter = (1ull << s.user_data()) - 1;
    picobench::scope scope(s);
    size_t result = 0;
    cmap_i map = cmap_inits;
    cmap_i_set_load_factors(&map, 0.0, 0.80);
    c_forrange (N1)
        result += ++cmap_i_emplace(&map, stc64_random() & filter, 0).first->second;
    s.set_result(result);
    cmap_i_del(&map);
}

#define P samples(2).iterations({N1, N1, N1, N1}).user_data({18, 23, 25, 31})
PICOBENCH(ins_and_access<umap_i>).P.baseline();
PICOBENCH(ins_and_access<bmap_i>).P;
PICOBENCH(ins_and_access<fmap_i>).P;
PICOBENCH(ins_and_access<hmap_i>).P;
PICOBENCH(ins_and_access<smap_i>).P;
PICOBENCH(ins_and_access<rmap_i>).P;
PICOBENCH(ins_and_access_cmap_i_).P;
#undef P


static void randomize(char* str, size_t len) {
    union {uint64_t i; char c[8];} r = {.i = stc64_random()};
    for (int i = len - 7, j = 0; i < len; ++j, ++i)
        str[i] = (r.c[j] & 63) + 48;
}

template <class MapStr>
static void ins_and_access_s(picobench::state& s)
{
    stc64_srandom(seed);
    std::string str(s.user_data(), 'x');
    randomize(&str[0], str.size());
    picobench::scope scope(s);
    size_t result = 0;
    MapStr map;
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
    s.set_result(result);
}

static void ins_and_access_s_cmap_s_(picobench::state& s)
{
    stc64_srandom(seed);
    cstr str = cstr_with_size(s.user_data(), 'x');
    randomize(str.str, cstr_size(str));
    picobench::scope scope(s);
    size_t result = 0;
    cmap_s map = cmap_inits;
    cmap_s_set_load_factors(&map, 0.0, 0.80);
    c_forrange (s.iterations()) {
        randomize(str.str, cstr_size(str));
        cmap_s_put(&map, str.str, cstr_clone(str));
        randomize(str.str, cstr_size(str));
        cmap_s_value_t* val = cmap_s_find(&map, str.str);
        if (val) {
            ++result;
            cmap_s_erase_entry(&map, val);
        }
    }
    s.set_result(result);
    cstr_del(&str);
    cmap_s_del(&map);
}

#define P samples(2).iterations({N1/5, N1/5, N1/5, N1/10, N1/40}).user_data({7, 8, 13, 100, 1000})
PICOBENCH(ins_and_access_s<umap_s>).P.baseline();
PICOBENCH(ins_and_access_s<bmap_s>).P;
PICOBENCH(ins_and_access_s<fmap_s>).P;
PICOBENCH(ins_and_access_s<hmap_s>).P;
PICOBENCH(ins_and_access_s<smap_s>).P;
PICOBENCH(ins_and_access_s<rmap_s>).P;
PICOBENCH(ins_and_access_s_cmap_s_).P;
#undef P