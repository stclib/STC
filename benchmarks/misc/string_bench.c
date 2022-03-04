// https://www.codeproject.com/Tips/5255442/Cplusplus14-20-Heterogeneous-Lookup-Benchmark
// https://github.com/shaovoon/cpp_hetero_lookup_bench

#include <time.h>

#define i_val_str
#include <stc/cvec.h>

#define i_key_str
#define i_val size_t
#include <stc/cmap.h>

#define i_key_str
#define i_val size_t
#include <stc/csmap.h>


cvec_str read_file(const char* name)
{
    cvec_str data = cvec_str_init();
    c_auto (cstr, line)
    c_autovar (FILE* f = fopen(name, "r"), fclose(f))
        while (cstr_getline(&line, f))
            cvec_str_emplace_back(&data, cstr_str(&line));
    return data;
}

void initShortStringVec(cvec_str* vs)
{
    cvec_str_clear(vs);

    *vs = read_file("names.txt");
    size_t lengths = 0;
    c_foreach (i, cvec_str, *vs)
    {
        cstr_append_s(i.ref, *i.ref);
        cstr_append_s(i.ref, *i.ref);
        lengths += cstr_size(*i.ref);
    }
    printf("avg len: %f\n", (float)lengths / cvec_str_size(*vs));
}

void initLongStringVec(cvec_str* vs)
{
    cvec_str_clear(vs);
    *vs = read_file("names.txt");
    cstr* s = vs->data;
    size_t lengths = 0;
    cstr_append_s(s, s[1]);
    cstr_append_s(s, s[2]);
    cstr_append_s(s, s[3]);
    for (int i=1; i < cvec_str_size(*vs); ++i)
    {
        cstr* t = vs->data + i;
        cstr_append_s(t, *t);
        cstr_append_s(t, *t);
        cstr_append_s(t, *s);
        cstr_append_s(t, *t);
        cstr_append_s(t, *t);
        lengths += cstr_size(*t);
    }
    printf("avg len: %f\n", (float)lengths / cvec_str_size(*vs));
}

struct Maps {
    csmap_str* snormal;
    cmap_str* unormal;
};

void initMaps(cvec_str vs, struct Maps maps)
{
    csmap_str_clear(maps.snormal);
    cmap_str_clear(maps.unormal);

    for (size_t i = 0; i < cvec_str_size(vs); ++i)
    {
        cstr str = *cvec_str_at(&vs, i);
        csmap_str_insert(maps.snormal, cstr_clone(str), i);
        cmap_str_insert(maps.unormal, cstr_clone(str), i);
    }
}

void benchmark(cvec_str vec_string, struct Maps maps);

static const size_t MAX_LOOP = 2000;

int main()
{
    c_auto (cvec_str, vec_string)
    c_auto (csmap_str, snormal)
    c_auto (cmap_str, unormal)
    {
        struct Maps maps = { &snormal, &unormal };

        initShortStringVec(&vec_string);
        initMaps(vec_string, maps);

        puts("Short String Benchmark");
        puts("======================");

        benchmark(vec_string, maps);

        puts("\nLong String Benchmark");
        puts("======================");

        initLongStringVec(&vec_string);
        initMaps(vec_string, maps);

        benchmark(vec_string, maps);
    }
}

void benchmark(cvec_str vec_string, struct Maps maps)
{
    size_t grandtotal = 0;
    size_t total;
    clock_t stopwatch;

    total = 0;
    printf("%32s", "Trans Map with char*");
    stopwatch = clock();
    for (size_t i = 0; i < MAX_LOOP; ++i)
    {
        csmap_str_iter it, end = csmap_str_end(maps.snormal); 
        for (size_t j = 0; j < cvec_str_size(vec_string); ++j)
        {
            csmap_str_find_it(maps.snormal, cstr_str(&vec_string.data[j]), &it);
            if (it.ref != end.ref)
                total += it.ref->second;
        }
    }
    grandtotal += total;
    printf(" timing:%5.0fms\n", (clock() - stopwatch) / (float)CLOCKS_PER_SEC * 1000.0f);


    total = 0;
    printf("%32s", "Trans Unord Map with char*");
    stopwatch = clock();
    for (size_t i = 0; i < MAX_LOOP; ++i)
    {
        cmap_str_iter it, end = cmap_str_end(maps.unormal); 
        for (size_t j = 0; j < cvec_str_size(vec_string); ++j)
        {
            it = cmap_str_find(maps.unormal, cstr_str(&vec_string.data[j]));
            if (it.ref != end.ref)
                total += it.ref->second;
        }
    }
    grandtotal += total;
    printf(" timing:%5.0fms\n", (clock() - stopwatch) / (float)CLOCKS_PER_SEC * 1000.0f);

    printf("C grandtotal: %" PRIuMAX " <--- Ignore this\n", grandtotal);
}
