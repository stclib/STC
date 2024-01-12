// https://www.codeproject.com/Tips/5255442/Cplusplus14-20-Heterogeneous-Lookup-Benchmark
// https://github.com/shaovoon/cpp_hetero_lookup_bench
// Requires c++20, e.g. g++ -std=c++20

#include <iostream>
#include <iomanip>
#include <chrono>
#include <string>
#include <string_view>
#include <vector>
#include <map>
#include <unordered_map>
#define i_static
#include "stc/cstr.h"

std::vector<std::string> read_file(const char* name)
{
    std::vector<std::string> data;
    c_scoped (FILE* f = fopen(name, "r"), f, fclose(f))
    c_scoped (cstr line = {0}, cstr_drop(&line)) {
        while (cstr_getline(&line, f))
            data.emplace_back(cstr_str(&line));
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

void initShortStringVec(std::vector<std::string>& vs, std::vector<std::string_view>& vsv)
{
    vs.clear();
    vsv.clear();

    vs = read_file("names.txt");
/*
    vs.push_back("Susan");
    vs.push_back("Jason");
    vs.push_back("Lily");
    vs.push_back("Michael");
    vs.push_back("Mary");

    vs.push_back("Jerry");
    vs.push_back("Jenny");
    vs.push_back("Klaus");
    vs.push_back("Celine");
    vs.push_back("Kenny");

    vs.push_back("Kelly");
    vs.push_back("Jackson");
    vs.push_back("Mandy");
    vs.push_back("Terry");
    vs.push_back("Sandy");

    vs.push_back("Billy");
    vs.push_back("Cindy");
    vs.push_back("Phil");
    vs.push_back("Lindy");
    vs.push_back("David");
*/
    size_t num = 0;
    for (size_t i = 0; i < vs.size(); ++i)
    {
        vsv.push_back(vs.at(i));
        num += vs.at(i).size();
    }
    std::cout << "num strings: " << vsv.size() << std::endl;
    std::cout << "avg str len: " << num / (float)vsv.size() << std::endl;
}

void initLongStringVec(std::vector<std::string>& vs, std::vector<std::string_view>& vsv)
{
    vs.clear();
    vsv.clear();

    vs = read_file("names.txt");
    for (size_t i = 1; i < vs.size(); ++i) {
        vs[i] += vs[i];
        vs[i] += vs[i];
        vs[i] += vs[i];
    }
/*
    vs.push_back("Susan Susan Susan Susan Susan Susan");
    vs.push_back("Jason Jason Jason Jason Jason Jason");
    vs.push_back("Lily Lily Lily Lily Lily Lily");
    vs.push_back("Michael Michael Michael Michael Michael Michael");
    vs.push_back("Mary Mary Mary Mary Mary Mary");

    vs.push_back("Jerry Jerry Jerry Jerry Jerry Jerry");
    vs.push_back("Jenny Jenny Jenny Jenny Jenny Jenny");
    vs.push_back("Klaus Klaus Klaus Klaus Klaus Klaus");
    vs.push_back("Celine Celine Celine Celine Celine Celine");
    vs.push_back("Kenny Kenny Kenny Kenny Kenny Kenny");

    vs.push_back("Kelly Kelly Kelly Kelly Kelly Kelly");
    vs.push_back("Jackson Jackson Jackson Jackson Jackson Jackson");
    vs.push_back("Mandy Mandy Mandy Mandy Mandy Mandy");
    vs.push_back("Terry Terry Terry Terry Terry Terry");
    vs.push_back("Sandy Sandy Sandy Sandy Sandy Sandy");

    vs.push_back("Billy Billy Billy Billy Billy Billy");
    vs.push_back("Cindy Cindy Cindy Cindy Cindy Cindy");
    vs.push_back("Phil Phil Phil Phil Phil Phil");
    vs.push_back("Lindy Lindy Lindy Lindy Lindy Lindy");
    vs.push_back("David David David David David David");
*/
    size_t num = 0;
    for (size_t i = 0; i < vs.size(); ++i)
    {
        vsv.push_back(vs.at(i));
        num += vs.at(i).size();
    }
    std::cout << "num strings: " << vsv.size() << std::endl;
    std::cout << "avg str len: " << num / (float)vsv.size() << std::endl;
}

void initMapNormal(const std::vector<std::string>& vs, std::map<std::string, size_t>& mapNormal)
{
    mapNormal.clear();
    for (size_t i = 0; i < vs.size(); ++i)
    {
        mapNormal.insert(std::make_pair(vs.at(i), i));
    }
}

void initMapTrans(const std::vector<std::string>& vs, std::map<std::string, size_t, std::less<> >& mapTrans)
{
    mapTrans.clear();
    for (size_t i = 0; i < vs.size(); ++i)
    {
        mapTrans.insert(std::make_pair(vs.at(i), i));
    }
}

struct MyEqual : public std::equal_to<>
{
    using is_transparent = void;
};

struct string_hash {
    using is_transparent = void;
    using key_equal = std::equal_to<>;  // Pred to use
    using hash_type = std::hash<std::string_view>;  // just a helper local type
    size_t operator()(std::string_view txt) const { return hash_type{}(txt); }
    size_t operator()(const std::string& txt) const { return hash_type{}(txt); }
    size_t operator()(const char* txt) const { return hash_type{}(txt); }
};

void initUnorderedMapNormal(const std::vector<std::string>& vs, std::unordered_map<std::string, size_t>& unordmapNormal)
{
    unordmapNormal.clear();
    for (size_t i = 0; i < vs.size(); ++i)
    {
        unordmapNormal.insert(std::make_pair(vs.at(i), i));
    }
}

void initUnorderedMapTrans(const std::vector<std::string>& vs, std::unordered_map<std::string, size_t, string_hash, MyEqual>& unordmapTrans)
{
    unordmapTrans.clear();
    for (size_t i = 0; i < vs.size(); ++i)
    {
        unordmapTrans.insert(std::make_pair(vs.at(i), i));
    }
}

void benchmark(
    const std::vector<std::string>& vec_shortstr,
    const std::vector<std::string_view>& vec_shortstrview,
    const std::map<std::string, size_t>& mapNormal,
    const std::map<std::string, size_t, std::less<> >& mapTrans,
    const std::unordered_map<std::string, size_t>& unordmapNormal,
    const std::unordered_map<std::string, size_t, string_hash, MyEqual>& unordmapTrans);

//const size_t MAX_LOOP = 1000000;
const size_t MAX_LOOP = 2000;

int main(void)
{
    std::vector<std::string> vec_shortstr;
    std::vector<std::string_view> vec_shortstrview;

    std::map<std::string, size_t> mapNormal;
    std::map<std::string, size_t, std::less<> > mapTrans;
    initShortStringVec(vec_shortstr, vec_shortstrview);
    initMapNormal(vec_shortstr, mapNormal);
    initMapTrans(vec_shortstr, mapTrans);

    std::unordered_map<std::string, size_t> unordmapNormal;
    std::unordered_map<std::string, size_t, string_hash, MyEqual> unordmapTrans;
    initUnorderedMapNormal(vec_shortstr, unordmapNormal);
    initUnorderedMapTrans(vec_shortstr, unordmapTrans);

    std::cout << "Short String Benchmark" << std::endl;
    std::cout << "======================" << std::endl;

    for (int i=0; i<3; ++i) benchmark(
        vec_shortstr,
        vec_shortstrview,
        mapNormal,
        mapTrans,
        unordmapNormal,
        unordmapTrans);

    std::cout << "Long String Benchmark" << std::endl;
    std::cout << "=====================" << std::endl;

    initLongStringVec(vec_shortstr, vec_shortstrview);
    initMapNormal(vec_shortstr, mapNormal);
    initMapTrans(vec_shortstr, mapTrans);

    initUnorderedMapNormal(vec_shortstr, unordmapNormal);
    initUnorderedMapTrans(vec_shortstr, unordmapTrans);

    for (int i=0; i<3; ++i) benchmark(
        vec_shortstr,
        vec_shortstrview,
        mapNormal,
        mapTrans,
        unordmapNormal,
        unordmapTrans);

    return 0;
}

void benchmark(
    const std::vector<std::string>& vec_shortstr,
    const std::vector<std::string_view>& vec_shortstrview,
    const std::map<std::string, size_t>& mapNormal,
    const std::map<std::string, size_t, std::less<> >& mapTrans,
    const std::unordered_map<std::string, size_t>& unordmapNormal,
    const std::unordered_map<std::string, size_t, string_hash, MyEqual>& unordmapTrans)
{
    size_t grandtotal = 0;
    size_t total = 0;
    timer stopwatch;
/*
    total = 0;
    stopwatch.start("Normal Map with string");
    for (size_t i = 0; i < MAX_LOOP; ++i)
    {
        for (size_t j = 0; j < vec_shortstr.size(); ++j)
        {
            const auto& it = mapNormal.find(vec_shortstr[j]);
            if(it!=mapNormal.cend())
                total += it->second;
        }
    }
    grandtotal += total;
    stopwatch.stop();

    total = 0;
    stopwatch.start("Normal Map with char*");
    for (size_t i = 0; i < MAX_LOOP; ++i)
    {
        for (size_t j = 0; j < vec_shortstr.size(); ++j)
        {
            const auto& it = mapNormal.find(vec_shortstr[j].c_str());
            if (it != mapNormal.cend())
                total += it->second;
        }
    }
    grandtotal += total;
    stopwatch.stop();
*/
    total = 0;
    stopwatch.start("Trans Map with char*");
    for (size_t i = 0; i < MAX_LOOP; ++i)
    {
        for (size_t j = 0; j < vec_shortstr.size(); ++j)
        {
            const auto& it = mapTrans.find(vec_shortstr[j].c_str());
            if (it != mapTrans.cend())
                total += it->second;
        }
    }
    grandtotal += total;
    stopwatch.stop();

    total = 0;
    stopwatch.start("Trans Map with string_view");
    for (size_t i = 0; i < MAX_LOOP; ++i)
    {
        for (size_t j = 0; j < vec_shortstrview.size(); ++j)
        {
            const auto& it = mapTrans.find(vec_shortstrview[j]);
            if (it != mapTrans.cend())
                total += it->second;
        }
    }
    grandtotal += total;
    stopwatch.stop();
/*
    total = 0;
    stopwatch.start("Normal Unord Map with string");
    for (size_t i = 0; i < MAX_LOOP; ++i)
    {
        for (size_t j = 0; j < vec_shortstr.size(); ++j)
        {
            const auto& it = unordmapNormal.find(vec_shortstr[j]);
            if (it != unordmapNormal.cend())
                total += it->second;
        }
    }
    grandtotal += total;
    stopwatch.stop();

    total = 0;
    stopwatch.start("Normal Unord Map with char*");
    for (size_t i = 0; i < MAX_LOOP; ++i)
    {
        for (size_t j = 0; j < vec_shortstr.size(); ++j)
        {
            const auto& it = unordmapNormal.find(vec_shortstr[j].c_str());
            if (it != unordmapNormal.cend())
                total += it->second;
        }
    }
    grandtotal += total;
    stopwatch.stop();
*/
    total = 0;
    stopwatch.start("Trans Unord Map with char*");
    for (size_t i = 0; i < MAX_LOOP; ++i)
    {
        for (size_t j = 0; j < vec_shortstr.size(); ++j)
        {
            const auto& it = unordmapTrans.find(vec_shortstr[j].c_str());
            if (it != unordmapTrans.cend())
                total += it->second;
        }
    }
    grandtotal += total;
    stopwatch.stop();

    total = 0;
    stopwatch.start("Trans Unord Map with string_view");
    for (size_t i = 0; i < MAX_LOOP; ++i)
    {
        for (size_t j = 0; j < vec_shortstrview.size(); ++j)
        {
            const auto& it = unordmapTrans.find(vec_shortstrview[j]);
            if (it != unordmapTrans.cend())
                total += it->second;
        }
    }
    grandtotal += total;

    stopwatch.stop();

    std::cout << "grandtotal:" << grandtotal << " <--- Ignore this\n" << std::endl;

}
