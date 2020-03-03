#include <stdio.h>
#include <stdlib.h>

#include "cmap.h"
#include "cstring.h"


declare_CVector(cs, CString, cstring_destroy);
declare_CMap_STR(ss, CString, cstring_destroy);
declare_CMap_STR(si, int);
declare_CMap(id, uint64_t, double);


// like fgets, but removes any newline
char *fgetstr(char *string, int n, FILE *stream)
{
	char *res = fgets(string, n, stream);
	if (!res) return res;
    int last = strlen(string);
    if (last) {
        --last; if (string[last] == '\n') string[last] = '\0';
    }
	return string;
}

int read_words(CMap(si)* map)
{
    FILE * fp;
#   define bufferLength 1024
    char line[bufferLength];
    size_t len = 0, i = 0;
    size_t read;

    fp = fopen("words_dictionary.txt", "r");
    if (fp == NULL)
        return -1;

    while (fgetstr(line, bufferLength, fp)) {
        ++i;
        if (i < 20) printf("%zu: %s\n", i, line);
        cmap_si_put(map, line, i);
    }

    fclose(fp);
    return 0;
}

int main()
{
    uint32_t i = 11, n = 671523123;

    CMap(si) words = cmap_init();
    printf("read\n");
    read_words(&words);
    //for (i=0; i<100; i++) { printf("%d: %d\n", i, graph[i]); graph[i] = 0; }
    CMapEntry(si)* num = NULL;
    num = cmap_si_get(words, "hello");
    if (num) printf("%s: %d\n", num->key.str, num->value);
    
    num = cmap_si_get(words, "funny");
    if (num) printf("%s: %d\n", num->key.str, num->value);

    printf("%d, %d\n", cmap_size(words), cmap_capacity(words));
    cmap_si_clear(&words);

/*
    CVector(cs) strv = cvector_INIT;
    
    CString hello = cstring_make("Hello");
    cstring_assign(&hello, "Awesome");
    
    

    cvector_cs_push(&strv, cstring_make("E1"));
    cvector_cs_push(&strv, cstring_make("E2"));
    cvector_cs_push(&strv, cstring_make("E3"));
    {CVectorIter(cs) it; cforeach (it, cvector_cs, strv) {
        printf("  %s\n", it.item->str);
    }}
    
    {int i; for (i = 0; i < cvector_size(strv); ++i) {
        printf("  %s\n", strv.data[i].str);
    }}
    cvector_cs_destroy(&strv);

    CMap(ss) smap = cmap_init();
    cmap_ss_put(&smap, "KEY1", cstring_make("VAL1"));
    cmap_ss_put(&smap, "KEY2", cstring_make("VAL2"));
    cmap_ss_put(&smap, "hello", cstring_makeCopy(hello));
    
    cstring_destroy(&hello);
    
    {CMapIter(ss) it = cmap_ss_begin(smap), end = cmap_ss_end(smap);
    for (; it.item != end.item; it = cmap_ss_next(it)) {
        printf("  %s: %s\n", it.item->key.str, it.item->value.str);
    }}
    cmap_ss_destroy(&smap);
*/    
    CMap(id) mymap = cmap_init();
    //for (i = 0; i < 600000; ++i)
    //    cmap_id_put(&mymap, ((uint64_t)i)*i, i * 1.0);


    
    //for (i=0; i<100; i++) { printf("%d: %d\n", i, graph[i]); graph[i] = 0; }
    uint64_t tn = 75000;
    cmap_id_put(&mymap, tn * tn, tn * 1.0);
    printf("look %llu: %f\n", tn*tn, cmap_id_get(mymap, tn*tn) ? cmap_id_get(mymap, tn*tn)->value : -999.9);
        
    //for (i = 0; i < cmap_size(mymap); ++i)
    //    printf("look %d: %f\n", i*i, *cmap_id_get(mymap, i*i));
    /*
    {CMapIter(id) it; int k = 0; 
    cforeach (it, cmap_id, mymap, k < 10, ++k) {
        printf("  %d: %f\n", it.item->key, it.item->value);
    }}
    */
    cmap_id_destroy(&mymap);
}
