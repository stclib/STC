#include <stc/crand.h>
#include <stc/cmap.h>
#include <stc/cstr.h>
#include <unordered_map>
#include <stdio.h>

#define PICOBENCH_IMPLEMENT_WITH_MAIN
#include "picobench.hpp"


PICOBENCH_SUITE("Map");
enum {N1 = 50000000, N2=20000000, N3=12000000, N4=6000000};
uint64_t seed = 123456;

static inline uint32_t fibonacci_hash(const void* data, size_t len) {
    return (uint32_t) (((*(const uint32_t *) data) * 11400714819323198485llu) >> 24);
}
using_cmap(ii, int, int, c_default_del, c_default_clone, c_default_equals, fibonacci_hash);
using_cmap_strkey(ss, cstr, cstr_del, cstr_clone);

using Map = std::unordered_map<int, int>;
using MapStr = std::unordered_map<std::string, std::string>;

static void stdmap_ctor_and_insert_one(picobench::state& s)
{
    printf("std::unordered_map: ctor_and_insert_one: %zu: %zd\n", s.iterations(), s.user_data());
    picobench::scope scope(s);
    size_t result = 0;
    c_forrange (n, s.iterations()) {
        Map map;
        map[n];
        result += map.size();
    }
    s.set_result(result);
}
PICOBENCH(stdmap_ctor_and_insert_one).samples(1).iterations({N1}).baseline();

static void cmap__ctor_and_insert_one(picobench::state& s)
{
    printf("cmap: ctor_and_insert_one: %zu: %zd\n", s.iterations(), s.user_data());
    picobench::scope scope(s);
    size_t result = 0;
    c_forrange (n, s.iterations()) {
        cmap_ii map = cmap_inits;
        cmap_ii_emplace(&map, n, 0);
        result += cmap_ii_size(map);
        cmap_ii_del(&map);
    }
    s.set_result(result);
}
PICOBENCH(cmap__ctor_and_insert_one).samples(1).iterations({N1});


static void stdmap__insert_and_erase(picobench::state& s)
{
    stc64_srandom(seed);
    printf("std::unordered_map: insert_and_erase: %zu: %zd\n", s.iterations(), s.user_data());
    picobench::scope scope(s);
    size_t result = 0;
    Map map;
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
PICOBENCH(stdmap__insert_and_erase).samples(1).iterations({N1}).baseline();


static void cmap__insert_and_erase(picobench::state& s)
{
    stc64_srandom(seed);
    printf("cmap: insert_and_erase: %zu: %zd\n", s.iterations(), s.user_data());
    picobench::scope scope(s);
    cmap_ii map = cmap_inits;
    cmap_ii_set_load_factors(&map, 0.80, 0);
    c_forrange (s.iterations())
        cmap_ii_emplace(&map, stc64_random(), 0);
    cmap_ii_clear(&map);
    stc64_srandom(seed);
    c_forrange (s.iterations())
        cmap_ii_emplace(&map, stc64_random(), 0);
    stc64_srandom(seed);
    c_forrange (s.iterations())
        cmap_ii_erase(&map, stc64_random());
    cmap_ii_del(&map);
    s.set_result(cmap_ii_size(map));
}
PICOBENCH(cmap__insert_and_erase).samples(1).iterations({N1});


static void stdmap__insert_and_access(picobench::state& s)
{
    stc64_srandom(seed);
    uint64_t filter = (1ull << s.user_data()) - 1;
    printf("std::unordered_map: insert_and_access: %zu: %zd\n", s.iterations(), s.user_data());    
    picobench::scope scope(s);
    size_t result = 0;
    Map map;
    c_forrange (N1)
        result += ++map[stc64_random() & filter];
    s.set_result(result);
}
PICOBENCH(stdmap__insert_and_access).samples(1).iterations({N1, N1, N1, N1}).user_data({18, 23, 25, 31}).baseline();


static void cmap__insert_and_access(picobench::state& s)
{
    stc64_srandom(seed);
    uint64_t filter = (1ull << s.user_data()) - 1;
    printf("cmap: insert_and_access: %zu: %zd\n", s.iterations(), s.user_data());
    picobench::scope scope(s);
    size_t result = 0;
    cmap_ii map = cmap_inits;
    cmap_ii_set_load_factors(&map, 0.80, 0);
    c_forrange (N1)
        result += ++cmap_ii_emplace(&map, stc64_random() & filter, 0).first->second;
    s.set_result(result);
    cmap_ii_del(&map);
}
PICOBENCH(cmap__insert_and_access).samples(1).iterations({N1, N1, N1, N1}).user_data({18, 23, 25, 31});

static void randomize(char* str, size_t len) {
    union {uint64_t i; char c[8];} r = {.i = stc64_random()};
    for (int i = len - 7, j = 0; i < len; ++j, ++i)
        str[i] = (r.c[j] & 63) + 48;
}

static void stdmap__ins_and_access_str(picobench::state& s)
{
    stc64_srandom(seed);
    std::string str(s.user_data(), 'x');
    randomize(&str[0], str.size());
    printf("std::unordered_map: insert_and_access string: %zu: %zd\n", s.iterations(), s.user_data());
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
PICOBENCH(stdmap__ins_and_access_str).samples(1).iterations({N2, N2, N2, N3, N4})
                                                .user_data({7, 8, 13, 100, 1000}).baseline();


static void cmap__ins_and_access_str(picobench::state& s)
{
    stc64_srandom(seed);
    cstr str = cstr_with_size(s.user_data(), 'x');
    randomize(str.str, cstr_size(str));
    printf("cmap: insert_and_access string: %zu: %zd\n", s.iterations(), s.user_data());
    picobench::scope scope(s);
    size_t result = 0;
    cmap_ss map = cmap_inits;
    cmap_ss_set_load_factors(&map, 0.80, 0);
    c_forrange (s.iterations()) {
        randomize(str.str, cstr_size(str));
        cmap_ss_put(&map, str.str, cstr_clone(str));
        randomize(str.str, cstr_size(str));
        cmap_ss_value_t* val = cmap_ss_find(&map, str.str);
        if (val) {
            ++result;
            cmap_ss_erase_entry(&map, val);
        }
    }
    s.set_result(result);
    cstr_del(&str);
    cmap_ss_del(&map);
}
PICOBENCH(cmap__ins_and_access_str).samples(1).iterations({N2, N2, N2, N3, N4})
                                              .user_data({7, 8, 13, 100, 1000});
