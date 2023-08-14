// https://www.codeproject.com/Tips/5255442/Cplusplus14-20-Heterogeneous-Lookup-Benchmark
// https://github.com/shaovoon/cpp_hetero_lookup_bench

#include <iostream>
#include <iomanip>
#include <chrono>
#define i_implement
#include <stc/cstr.h>   // string
#define i_implement
#include <stc/csview.h> // string_view
#include <stc/algo/raii.h>

#define i_key_str
#include <stc/cvec.h>   // vec of cstr with const char* lookup

#define i_type cvec_sv  // override default type name (cvec_csview)
#define i_key csview
#define i_cmp csview_cmp
#include <stc/cvec.h>   // cvec_vs: vec of csview

#define i_key_str
#define i_val size_t
#include <stc/csmap.h>  // sorted map of cstr, const char* lookup

#define i_key_ssv
#define i_val size_t
#include <stc/csmap.h>  // sorted map of cstr, csview lookup

#define i_key_str
#define i_val size_t
#include <stc/cmap.h>   // unordered map of cstr, const char* lookup

#define i_key_ssv
#define i_val size_t
#include <stc/cmap.h>   // unordered map of cstr, csview lookup


cvec_str read_file(const char* name)
{
    cvec_str data = cvec_str_init();
    c_auto (cstr, line)
    c_with (FILE* f = fopen(name, "r"), fclose(f))
        while (cstr_getline(&line, f))
            cvec_str_emplace_back(&data, cstr_str(&line));
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

void initShortStringVec(cvec_str* vs, cvec_sv* vsv)
{
    cvec_str_drop(vs);
    cvec_sv_clear(vsv);
    
    *vs = read_file("names.txt");
/*
    cvec_str_emplace_back(vs, "Susan");
    cvec_str_emplace_back(vs, "Jason");
    cvec_str_emplace_back(vs, "Lily");
    cvec_str_emplace_back(vs, "Michael");
    cvec_str_emplace_back(vs, "Mary");

    cvec_str_emplace_back(vs, "Jerry");
    cvec_str_emplace_back(vs, "Jenny");
    cvec_str_emplace_back(vs, "Klaus");
    cvec_str_emplace_back(vs, "Celine");
    cvec_str_emplace_back(vs, "Kenny");

    cvec_str_emplace_back(vs, "Kelly");
    cvec_str_emplace_back(vs, "Jackson");
    cvec_str_emplace_back(vs, "Mandy");
    cvec_str_emplace_back(vs, "Terry");
    cvec_str_emplace_back(vs, "Sandy");

    cvec_str_emplace_back(vs, "Billy");
    cvec_str_emplace_back(vs, "Cindy");
    cvec_str_emplace_back(vs, "Phil");
    cvec_str_emplace_back(vs, "Lindy");
    cvec_str_emplace_back(vs, "David");
*/
    size_t num = 0;
    c_foreach (i, cvec_str, *vs)
    {
        cvec_sv_push_back(vsv, cstr_sv(i.ref));
        num += cstr_size(i.ref);
    }
    std::cout << "num strings: " << cvec_sv_size(vsv) << std::endl;
    std::cout << "avg str len: " << num / (float)cvec_sv_size(vsv) << std::endl;
}

void initLongStringVec(cvec_str* vs, cvec_sv* vsv)
{
    cvec_str_drop(vs);
    cvec_sv_clear(vsv);
    
    *vs = read_file("names.txt");
    c_foreach (i, cvec_str, *vs) {
        cstr_append_s(i.ref, *i.ref);
        cstr_append_s(i.ref, *i.ref);
        cstr_append_s(i.ref, *i.ref);
    }    
/*
    cvec_str_emplace_back(vs, "Susan Susan Susan Susan Susan Susan");
    cvec_str_emplace_back(vs, "Jason Jason Jason Jason Jason Jason");
    cvec_str_emplace_back(vs, "Lily Lily Lily Lily Lily Lily");
    cvec_str_emplace_back(vs, "Michael Michael Michael Michael Michael Michael");
    cvec_str_emplace_back(vs, "Mary Mary Mary Mary Mary Mary");

    cvec_str_emplace_back(vs, "Jerry Jerry Jerry Jerry Jerry Jerry");
    cvec_str_emplace_back(vs, "Jenny Jenny Jenny Jenny Jenny Jenny");
    cvec_str_emplace_back(vs, "Klaus Klaus Klaus Klaus Klaus Klaus");
    cvec_str_emplace_back(vs, "Celine Celine Celine Celine Celine Celine");
    cvec_str_emplace_back(vs, "Kenny Kenny Kenny Kenny Kenny Kenny");

    cvec_str_emplace_back(vs, "Kelly Kelly Kelly Kelly Kelly Kelly");
    cvec_str_emplace_back(vs, "Jackson Jackson Jackson Jackson Jackson Jackson");
    cvec_str_emplace_back(vs, "Mandy Mandy Mandy Mandy Mandy Mandy");
    cvec_str_emplace_back(vs, "Terry Terry Terry Terry Terry Terry");
    cvec_str_emplace_back(vs, "Sandy Sandy Sandy Sandy Sandy Sandy");

    cvec_str_emplace_back(vs, "Billy Billy Billy Billy Billy Billy");
    cvec_str_emplace_back(vs, "Cindy Cindy Cindy Cindy Cindy Cindy");
    cvec_str_emplace_back(vs, "Phil Phil Phil Phil Phil Phil");
    cvec_str_emplace_back(vs, "Lindy Lindy Lindy Lindy Lindy Lindy");
    cvec_str_emplace_back(vs, "David David David David David David");
*/
    size_t num = 0;
    c_foreach (i, cvec_str, *vs)
    {
        cvec_sv_push_back(vsv, cstr_sv(i.ref));
        num += cstr_size(i.ref);
    }
    std::cout << "num strings: " << cvec_sv_size(vsv) << std::endl;
    std::cout << "avg str len: " << num / (float)cvec_sv_size(vsv) << std::endl;
}

void initMaps(const cvec_str* vs, csmap_str* mapTrans, csmap_ssv* mapSview,
                                  cmap_str* unordmapTrans, cmap_ssv* unordmapSview)
{
    csmap_str_clear(mapTrans);
    csmap_ssv_clear(mapSview);
    cmap_str_clear(unordmapTrans);
    cmap_ssv_clear(unordmapSview);

    size_t n = 0;
    c_foreach (i, cvec_str, *vs)
    {
        csmap_str_insert(mapTrans, cstr_clone(*i.ref), n);
        csmap_ssv_insert(mapSview, cstr_clone(*i.ref), n);
        cmap_str_insert(unordmapTrans, cstr_clone(*i.ref), n);
        cmap_ssv_insert(unordmapSview, cstr_clone(*i.ref), n);
        ++n;
    }
}

void benchmark(
    const cvec_str* vec_string,
    const cvec_sv* vec_stringview,
    const csmap_str* mapTrans,
    const csmap_ssv* mapSview,
    const cmap_str* unordmapTrans,
    const cmap_ssv* unordmapSview);

//const size_t MAX_LOOP = 1000000;
const size_t MAX_LOOP = 2000;

int main(void)
{
    c_auto (cvec_str, vec_string)
    c_auto (cvec_sv, vec_stringview)
    c_auto (csmap_str, mapTrans)
    c_auto (csmap_ssv, mapSview)
    c_auto (cmap_str, unordmapTrans)
    c_auto (cmap_ssv, unordmapSview)
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
    const cvec_str* vec_string,
    const cvec_sv* vec_stringview,
    const csmap_str* mapTrans,
    const csmap_ssv*  mapSview,
    const cmap_str* unordmapTrans,
    const cmap_ssv* unordmapSview)
{
    size_t grandtotal = 0;

    size_t total = 0;

    timer stopwatch;
    total = 0;
    stopwatch.start("Trans Map with char*");
    for (size_t i = 0; i < MAX_LOOP; ++i)
    {
        c_foreach (j, cvec_str, *vec_string)
        {
            const csmap_str_value* v = csmap_str_get(mapTrans, cstr_str(j.ref));
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
        c_foreach (j, cvec_sv, *vec_stringview)
        {
            const csmap_ssv_value* v = csmap_ssv_get(mapSview, *j.ref);
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
        c_foreach (j, cvec_str, *vec_string)
        {
            const cmap_str_value* v = cmap_str_get(unordmapTrans, cstr_str(j.ref));
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
        c_foreach (j, cvec_sv, *vec_stringview)
        {
            const cmap_ssv_value* v = cmap_ssv_get(unordmapSview, *j.ref);
            if (v)
                total += v->second;
        }
    }
    grandtotal += total;
    stopwatch.stop();

    std::cout << "grandtotal:" << grandtotal << " <--- Ignore this\n" << std::endl;

}
