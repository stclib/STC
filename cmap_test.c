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
        if (i < 10) printf("%zu: %s\n", i, line);
        cmap_si_put(map, line, i);
    }

    fclose(fp);
    return 0;
}

int main()
{
    int i = 0;

    CMap(si) words = cmap_INIT;
    printf("read words\n");
    read_words(&words);

    CMapEntry(si)* num = NULL;
    num = cmap_si_get(words, "hello");
    if (num) printf("%s: %d\n", num->key.str, num->value);
    
    num = cmap_si_get(words, "funny");
    if (num) printf("%s: %d\n", num->key.str, num->value);

    printf("words size: %d, capacity %d\n", cmap_size(words), cmap_capacity(words));
    cmap_si_clear(&words);

    CVector(cs) strv = cvector_INIT;
    CString hello = cstring_make("Hello");
    cstring_assign(&hello, "Awesome");

    cvector_cs_push(&strv, cstring_make("E1"));
    cvector_cs_push(&strv, cstring_make("E2"));
    cvector_cs_push(&strv, cstring_make("E3"));
    CVectorIter(cs) it1; cforeach (it1, cvector_cs, strv) {
        printf("  %s\n", it1.item->str);
    }
    
    for (i = 0; i < cvector_size(strv); ++i) {
        printf("  %s\n", strv.data[i].str);
    }
    cvector_cs_destroy(&strv);

    CMap(ss) smap = cmap_INIT;
    cmap_ss_put(&smap, "KEY1", cstring_make("VAL1"));
    cmap_ss_put(&smap, "KEY2", cstring_make("VAL2"));
    cmap_ss_put(&smap, "hello", cstring_makeCopy(hello));
    
    cstring_destroy(&hello);
    
    CMapIter(ss) it2 = cmap_ss_begin(smap), end2 = cmap_ss_end(smap);
    for (; it2.item != end2.item; it2 = cmap_ss_next(it2)) {
        printf("  %s: %s\n", it2.item->key.str, it2.item->value.str);
    }
    cmap_ss_destroy(&smap);

    CMap(id) mymap = cmap_INIT;
    for (i = 0; i < 600000; ++i)
        cmap_id_put(&mymap, i*i, i * 1.0);
        
    for (i = 1000; i < 1010; ++i)
        printf("look %d: %f\n", i*i, *cmap_id_get(mymap, i*i));

    cmap_id_destroy(&mymap);
}
