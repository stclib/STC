// https://www.codeproject.com/Tips/5255442/Cplusplus14-20-Heterogeneous-Lookup-Benchmark
// https://github.com/shaovoon/cpp_hetero_lookup_bench

#include <iostream>
#include <iomanip>
#include <chrono>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>

#define i_val_str
#include <stc/cvec.h>


std::vector<std::string> read_file(const char* name)
{
    std::vector<std::string> data;
    c_auto (cstr, line)
    c_autovar (FILE* f = fopen(name, "r"), fclose(f))
        while (cstr_getline(&line, f))
            data.emplace_back(cstr_str(&line));
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

void initShortStringVec(std::vector<std::string>& vs)
{
    vs.clear();
    vs = read_file("names.txt");
    size_t num = 0;
    
    for (size_t i = 0; i < vs.size(); ++i)
    {
        num += vs[i].size();
    }
    printf("avg len: %f\n", (float)num / vs.size());
}

void initLongStringVec(std::vector<std::string>& vs)
{
    vs.clear();
    vs = read_file("names.txt");
    size_t num = 0;
    vs[0] += vs[1];
    vs[0] += vs[2];
    vs[0] += vs[3];            
    for (size_t i = 1; i < vs.size(); ++i)
    {
        vs[i] += vs[i];
        vs[i] += vs[i];
        vs[i] += vs[0];
        vs[i] += vs[i];
        vs[i] += vs[i];
        num += vs[i].size();
    }
    printf("avg len: %f\n", (float)num / vs.size());
}

void initMapNormal(const std::vector<std::string>& vs, 
                   std::map<std::string, size_t>& mapNormal,
                   std::unordered_map<std::string, size_t>& unordmapNormal)
{
    mapNormal.clear();
    unordmapNormal.clear();
    for (size_t i = 0; i < vs.size(); ++i)
    {
        mapNormal.insert(std::make_pair(vs.at(i), i));
        unordmapNormal.insert(std::make_pair(vs.at(i), i));
    }
}
/*
struct string_hash {
    using is_transparent = void;
    using hash_type = std::hash<std::string_view>;  // just a helper local type
    size_t operator()(const std::string& txt) const { return hash_type{}(txt); }
    size_t operator()(const char* txt) const        { return hash_type{}(txt); }
};*/

void benchmark(
    const std::vector<std::string>& vec_string,
    const std::map<std::string, size_t>& mapNormal,
    const std::unordered_map<std::string, size_t>& unordmapNormal);

const size_t MAX_LOOP = 2000;

int main()
{
    std::vector<std::string> vec_string;

    std::map<std::string, size_t> mapNormal;
    std::unordered_map<std::string, size_t> unordmapNormal;

    initShortStringVec(vec_string);
    initMapNormal(vec_string, mapNormal, unordmapNormal);

    std::cout << "Short String Benchmark" << std::endl;
    std::cout << "======================" << std::endl;

    benchmark(vec_string, mapNormal, unordmapNormal);

    std::cout << "Long String Benchmark" << std::endl;
    std::cout << "=====================" << std::endl;

    initLongStringVec(vec_string);
    initMapNormal(vec_string, mapNormal, unordmapNormal);

    benchmark(vec_string, mapNormal, unordmapNormal);
    return 0;
}

void benchmark(
    const std::vector<std::string>& vec_string, 
    const std::map<std::string, size_t>& mapNormal,
    const std::unordered_map<std::string, size_t>& unordmapNormal)
{
    size_t grandtotal = 0;
    size_t total = 0;
    timer stopwatch;

    total = 0;
    stopwatch.start("Normal Map with string");
    for (size_t i = 0; i < MAX_LOOP; ++i)
    {
        for (size_t j = 0; j < vec_string.size(); ++j)
        {
            const auto& it = mapNormal.find(vec_string[j]);
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
        for (size_t j = 0; j < vec_string.size(); ++j)
        {
            const auto& it = mapNormal.find(vec_string[j].c_str());
            if (it != mapNormal.cend())
                total += it->second;
        }
    }
    grandtotal += total;
    stopwatch.stop();

    total = 0;
    stopwatch.start("Normal Unord Map with string");
    for (size_t i = 0; i < MAX_LOOP; ++i)
    {
        for (size_t j = 0; j < vec_string.size(); ++j)
        {
            const auto& it = unordmapNormal.find(vec_string[j]);
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
        for (size_t j = 0; j < vec_string.size(); ++j)
        {
            const auto& it = unordmapNormal.find(vec_string[j].c_str());
            if (it != unordmapNormal.cend())
                total += it->second;
        }
    }
    grandtotal += total;
    stopwatch.stop();

    std::cout << "C++ grandtotal:" << grandtotal << " <--- Ignore this\n" << std::endl;
}