// MIT License
//
// Copyright (c) 2020 Tyge Løvset, NORCE, www.norceresearch.no
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include <stdio.h>
#include <stdlib.h>

#include "cmap.h"
#include "cstring.h"


declare_CStringVector(s);
declare_CMap_StringKey(ss, CString, cstring_destroy);
declare_CMap_StringKey(si, int);
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

int read_words(CMap_si* map)
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

    CMap_si words = cmap_initializer;
    
    printf("read words\n");
    read_words(&words);

    CMapEntry_si* num = NULL;
    num = cmap_si_get(words, "hello");
    if (num) printf("%s: %d\n", num->key.str, num->value);
    
    num = cmap_si_get(words, "funny");
    if (num) printf("%s: %d\n", num->key.str, num->value);

    printf("words size: %zd, capacity %zd\n", cmap_size(words), cmap_capacity(words));
    cmap_si_clear(&words);

    CVector_s strv = cvector_initializer;
    CString hello = cstring_make("Hello");
    cstring_assign(&hello, "Awesome");

    cvector_s_push(&strv, cstring_make("E0"));
    cvector_s_push(&strv, cstring_make("E1"));
    cvector_s_push(&strv, cstring_make("E2"));
    printf("  element %d: %s\n", 1, strv.data[1].str);
    
    cforeach (i, cvector_s, strv) {
        printf("  %s\n", i.item->str);
    }
    
    for (i = 0; i < cvector_size(strv); ++i) {
        printf("  %s\n", strv.data[i].str);
    }
    cvector_s_destroy(&strv);

    CMap_ss smap = cmap_initializer;
    cmap_ss_put(&smap, "KEY1", cstring_make("VAL1"));
    cmap_ss_put(&smap, "KEY2", cstring_make("VAL2"));
    cmap_ss_put(&smap, "hello", cstring_clone(hello));
    cstring_destroy(&hello);
    
    cforeach (i, cmap_ss, smap)
        printf("  %s: %s\n", i.item->key.str, i.item->value.str);
    cmap_ss_destroy(&smap);

    CMap_id mymap = cmap_initializer;
    for (i = 0; i < 600000; ++i)
        cmap_id_put(&mymap, i*i, i);
        
    for (i = 1000; i < 1010; ++i)
        printf("lookup %d: %f\n", i*i, cmap_id_get(mymap, i*i)->value);
    
    CMapEntry_id* me = cmap_id_get(mymap, 10000);
    printf("changed: %d %f\n", me->changed, me->value);
    cmap_id_put(&mymap, 10000, 101.2);
    printf("changed: %d %f\n", me->changed, me->value);

    cmap_id_destroy(&mymap);
}
