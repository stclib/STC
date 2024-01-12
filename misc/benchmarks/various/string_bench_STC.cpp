// https://www.codeproject.com/Tips/5255442/Cplusplus14-20-Heterogeneous-Lookup-Benchmark
// https://github.com/shaovoon/cpp_hetero_lookup_bench

#include <iostream>
#include <iomanip>
#include <chrono>
#define i_implement
#include "stc/cstr.h"   // string
#define i_implement
#include "stc/csview.h" // string_view
#include "stc/algo/utility.h"

#define i_key_str
#include "stc/vec.h"    // vec of cstr with const char* lookup

#define i_type vec_sv  // override default type name (vec_csview)
#define i_key csview
#define i_cmp csview_cmp
#include "stc/vec.h"   // vec_vs: vec of csview

#define i_key_str
#define i_val size_t
#include "stc/smap.h"  // sorted map of cstr, const char* lookup

#define i_key_ssv
#define i_val size_t
#include "stc/smap.h"  // sorted map of cstr, csview lookup

#define i_key_str
#define i_val size_t
#include "stc/hmap.h"   // unordered map of cstr, const char* lookup

#define i_key_ssv
#define i_val size_t
#include "stc/hmap.h"   // unordered map of cstr, csview lookup


vec_str read_file(const char* name)
{
    vec_str data = {0};
    c_scoped (FILE* f = fopen(name, "r"), f != 0, fclose(f))
    c_scoped (cstr line = {0}, cstr_drop(&line)) {
        while (cstr_getline(&line, f))
            vec_str_push(&data, cstr_clone(line));
    }
    return data;
}

class timer
{
public:
    timer() = default;
    void start(const std::string& text_)
    {
        text = text_;
        begin = std::chrono::high_resolution_clock::now();
    }
    void stop()
    {
        auto end = std::chrono::high_resolution_clock::now();
        auto dur = end - begin;
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(dur).count();
        std::cout << std::setw(32) << text << " timing:" << std::setw(5) << ms << "ms" << std::endl;
    }

private:
    std::string text;
    std::chrono::high_resolution_clock::time_point begin;
};

void initShortStringVec(vec_str* vs, vec_sv* vsv)
{
    vec_str_drop(vs);
    vec_sv_clear(vsv);

    *vs = read_file("names.txt");
/*
    vec_str_emplace_back(vs, "Susan");
    vec_str_emplace_back(vs, "Jason");
    vec_str_emplace_back(vs, "Lily");
    vec_str_emplace_back(vs, "Michael");
    vec_str_emplace_back(vs, "Mary");

    vec_str_emplace_back(vs, "Jerry");
    vec_str_emplace_back(vs, "Jenny");
    vec_str_emplace_back(vs, "Klaus");
    vec_str_emplace_back(vs, "Celine");
    vec_str_emplace_back(vs, "Kenny");

    vec_str_emplace_back(vs, "Kelly");
    vec_str_emplace_back(vs, "Jackson");
    vec_str_emplace_back(vs, "Mandy");
    vec_str_emplace_back(vs, "Terry");
    vec_str_emplace_back(vs, "Sandy");

    vec_str_emplace_back(vs, "Billy");
    vec_str_emplace_back(vs, "Cindy");
    vec_str_emplace_back(vs, "Phil");
    vec_str_emplace_back(vs, "Lindy");
    vec_str_emplace_back(vs, "David");
*/
    size_t num = 0;
    c_foreach (i, vec_str, *vs)
    {
        vec_sv_push_back(vsv, cstr_sv(i.ref));
        num += cstr_size(i.ref);
    }
    std::cout << "num strings: " << vec_sv_size(vsv) << std::endl;
    std::cout << "avg str len: " << num / (float)vec_sv_size(vsv) << std::endl;
}

void initLongStringVec(vec_str* vs, vec_sv* vsv)
{
    vec_str_drop(vs);
    vec_sv_clear(vsv);

    *vs = read_file("names.txt");
    c_foreach (i, vec_str, *vs) {
        cstr_append_s(i.ref, *i.ref);
        cstr_append_s(i.ref, *i.ref);
        cstr_append_s(i.ref, *i.ref);
    }
/*
    vec_str_emplace_back(vs, "Susan Susan Susan Susan Susan Susan");
    vec_str_emplace_back(vs, "Jason Jason Jason Jason Jason Jason");
    vec_str_emplace_back(vs, "Lily Lily Lily Lily Lily Lily");
    vec_str_emplace_back(vs, "Michael Michael Michael Michael Michael Michael");
    vec_str_emplace_back(vs, "Mary Mary Mary Mary Mary Mary");

    vec_str_emplace_back(vs, "Jerry Jerry Jerry Jerry Jerry Jerry");
    vec_str_emplace_back(vs, "Jenny Jenny Jenny Jenny Jenny Jenny");
    vec_str_emplace_back(vs, "Klaus Klaus Klaus Klaus Klaus Klaus");
    vec_str_emplace_back(vs, "Celine Celine Celine Celine Celine Celine");
    vec_str_emplace_back(vs, "Kenny Kenny Kenny Kenny Kenny Kenny");

    vec_str_emplace_back(vs, "Kelly Kelly Kelly Kelly Kelly Kelly");
    vec_str_emplace_back(vs, "Jackson Jackson Jackson Jackson Jackson Jackson");
    vec_str_emplace_back(vs, "Mandy Mandy Mandy Mandy Mandy Mandy");
    vec_str_emplace_back(vs, "Terry Terry Terry Terry Terry Terry");
    vec_str_emplace_back(vs, "Sandy Sandy Sandy Sandy Sandy Sandy");

    vec_str_emplace_back(vs, "Billy Billy Billy Billy Billy Billy");
    vec_str_emplace_back(vs, "Cindy Cindy Cindy Cindy Cindy Cindy");
    vec_str_emplace_back(vs, "Phil Phil Phil Phil Phil Phil");
    vec_str_emplace_back(vs, "Lindy Lindy Lindy Lindy Lindy Lindy");
    vec_str_emplace_back(vs, "David David David David David David");
*/
    size_t num = 0;
    c_foreach (i, vec_str, *vs)
    {
        vec_sv_push_back(vsv, cstr_sv(i.ref));
        num += cstr_size(i.ref);
    }
    std::cout << "num strings: " << vec_sv_size(vsv) << std::endl;
    std::cout << "avg str len: " << num / (float)vec_sv_size(vsv) << std::endl;
}

void initMaps(const vec_str* vs, smap_str* mapTrans, smap_ssv* mapSview,
                                  hmap_str* unordmapTrans, hmap_ssv* unordmapSview)
{
    smap_str_clear(mapTrans);
    smap_ssv_clear(mapSview);
    hmap_str_clear(unordmapTrans);
    hmap_ssv_clear(unordmapSview);

    size_t n = 0;
    c_foreach (i, vec_str, *vs)
    {
        smap_str_insert(mapTrans, cstr_clone(*i.ref), n);
        smap_ssv_insert(mapSview, cstr_clone(*i.ref), n);
        hmap_str_insert(unordmapTrans, cstr_clone(*i.ref), n);
        hmap_ssv_insert(unordmapSview, cstr_clone(*i.ref), n);
        ++n;
    }
}

void benchmark(
    const vec_str* vec_string,
    const vec_sv* vec_stringview,
    const smap_str* mapTrans,
    const smap_ssv* mapSview,
    const hmap_str* unordmapTrans,
    const hmap_ssv* unordmapSview);

//const size_t MAX_LOOP = 1000000;
const size_t MAX_LOOP = 2000;

int main(void)
{
    c_scoped (vec_str vec_string = {0}, vec_str_drop(&vec_string))
    c_scoped (vec_sv vec_stringview = {0}, vec_sv_drop(&vec_stringview))
    c_scoped (smap_str mapTrans = {0}, smap_str_drop(&mapTrans))
    c_scoped (smap_ssv mapSview = {0}, smap_ssv_drop(&mapSview))
    c_scoped (hmap_str unordmapTrans = {0}, hmap_str_drop(&unordmapTrans))
    c_scoped (hmap_ssv unordmapSview = {0}, hmap_ssv_drop(&unordmapSview))
    {
        std::cout << "Short String Benchmark" << std::endl;
        std::cout << "======================" << std::endl;

        initShortStringVec(&vec_string, &vec_stringview);
        initMaps(&vec_string, &mapTrans, &mapSview,
                              &unordmapTrans, &unordmapSview);

        for (int i=0; i<3; ++i)
        benchmark(
            &vec_string,
            &vec_stringview,
            &mapTrans,
            &mapSview,
            &unordmapTrans,
            &unordmapSview);

        std::cout << "Long String Benchmark" << std::endl;
        std::cout << "=====================" << std::endl;

        initLongStringVec(&vec_string, &vec_stringview);
        initMaps(&vec_string, &mapTrans, &mapSview,
                              &unordmapTrans, &unordmapSview);
        for (int i=0; i<3; ++i)
        benchmark(
            &vec_string,
            &vec_stringview,
            &mapTrans,
            &mapSview,
            &unordmapTrans,
            &unordmapSview);
    }
    return 0;
}

void benchmark(
    const vec_str* vec_string,
    const vec_sv* vec_stringview,
    const smap_str* mapTrans,
    const smap_ssv* mapSview,
    const hmap_str* unordmapTrans,
    const hmap_ssv* unordmapSview)
{
    size_t grandtotal = 0;

    size_t total = 0;

    timer stopwatch;
    total = 0;
    stopwatch.start("Trans Map with char*");
    for (size_t i = 0; i < MAX_LOOP; ++i)
    {
        c_foreach (j, vec_str, *vec_string)
        {
            const smap_str_value* v = smap_str_get(mapTrans, cstr_str(j.ref));
            if (v)
                total += v->second;
        }
    }
    grandtotal += total;
    stopwatch.stop();

    total = 0;
    stopwatch.start("Trans Map with string_view");
    for (size_t i = 0; i < MAX_LOOP; ++i)
    {
        c_foreach (j, vec_sv, *vec_stringview)
        {
            const smap_ssv_value* v = smap_ssv_get(mapSview, *j.ref);
            if (v)
                total += v->second;
        }
    }
    grandtotal += total;
    stopwatch.stop();

    total = 0;
    stopwatch.start("Trans Unord Map with char*");
    for (size_t i = 0; i < MAX_LOOP; ++i)
    {
        c_foreach (j, vec_str, *vec_string)
        {
            const hmap_str_value* v = hmap_str_get(unordmapTrans, cstr_str(j.ref));
            if (v)
                total += v->second;
        }
    }
    grandtotal += total;
    stopwatch.stop();

    total = 0;
    stopwatch.start("Trans Unord Map with string_view");
    for (size_t i = 0; i < MAX_LOOP; ++i)
    {
        c_foreach (j, vec_sv, *vec_stringview)
        {
            const hmap_ssv_value* v = hmap_ssv_get(unordmapSview, *j.ref);
            if (v)
                total += v->second;
        }
    }
    grandtotal += total;
    stopwatch.stop();

    std::cout << "grandtotal:" << grandtotal << " <--- Ignore this\n" << std::endl;

}
