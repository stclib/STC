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
#include <time.h>

#include "c_hashmap.h"
#include "c_string.h"


c_declare_Vector_string(s);
c_declare_Hashmap_stringkey(ss, c_String, c_string_destroy);
c_declare_Hashmap_stringkey(si, int);
c_declare_Hashmap(id, uint64_t, double);


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

int read_words(c_Hashmap_si* map)
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
        c_hashmap_si_put(map, line, i);
    }

    fclose(fp);
    return 0;
}

void stringSpeed(int limit) {
    c_String s = c_string_initializer;
    char ch[2] = {0, 0};
    size_t x = 0;
    size_t p = 0;
    for (int n = 0; n < limit; ++n) { ch[0] = 'A' + (rand() % 26); c_string_append(&s, ch); }
    const char* search = "ABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZABCDEFGHIJKLMNOPQRSTUVWXYZ";
    c_string_append(&s, search);
    clock_t before = clock();
    for (int n = 0; n < limit; ++n) {
        p = c_string_find(s, 0, search + (n % 100));
        if (p != c_string_npos) x += p;
    }
    clock_t diff = clock() - before;
    printf("string length = %llu / %llu, sum %llu speed: %f\n", c_string_size(s), c_string_capacity(s), x, 1.0 * diff / CLOCKS_PER_SEC);
}

int main()
{
    int i = 0;

    stringSpeed(20000);
    
    c_String cs = c_string_make("one-nine-three-seven-five");
    printf("%s.\n", cs.str);
    c_string_insert(&cs, 3, "-two");
    printf("%s.\n", cs.str);
    c_string_erase(&cs, 7, 5); // -nine
    printf("%s.\n", cs.str);
    c_string_replace(&cs, 0, "seven", "four");
    printf("%s.\n", cs.str);
    
    printf("found: %s\n", cs.str + c_string_find(cs, 0, "four"));
    c_string_assign(&cs, "one two three four five six seven");
    c_string_replace(&cs, 0, "four", "F-O-U-R");
    printf("replace: %s\n", cs.str);
    
    c_Hashmap_si words = c_hashmap_initializer;
    printf("read words\n");
    read_words(&words);

    c_HashmapEntry_si* num = NULL;
    num = c_hashmap_si_get(words, "hello");
    if (num) printf("%s: %d\n", num->key.str, num->value);
    
    num = c_hashmap_si_get(words, "funny");
    if (num) printf("%s: %d\n", num->key.str, num->value);

    printf("words size: %llu, capacity %llu\n", c_hashmap_size(words), c_hashmap_buckets(words));
    c_hashmap_si_clear(&words);

    c_Vector_s strv = c_vector_initializer;
    c_String myday = c_string_make("my day");
    c_string_assign(&myday, "great");

    c_vector_s_push(&strv, c_string_make("E0"));
    c_vector_s_push(&strv, c_string_make("E1"));
    c_vector_s_push(&strv, c_string_make("E2"));
    printf("  element %d: %s\n", 1, strv.data[1].str);
    
    c_foreach (i, c_vector_s, strv) {
        printf("  %s\n", i.item->str);
    }
    
    for (i = 0; i < c_vector_size(strv); ++i) {
        printf("  %s\n", strv.data[i].str);
    }
    c_vector_s_destroy(&strv);

    c_Hashmap_ss smap = c_hashmap_initializer;
    c_hashmap_ss_put(&smap, "KEY1", c_string_make("VAL1"));
    c_hashmap_ss_put(&smap, "KEY2", c_string_make("VAL2"));
    c_hashmap_ss_put(&smap, "hello", c_string_makeCopy(myday));
    c_string_destroy(&myday);
    
    c_foreach (i, c_hashmap_ss, smap)
        printf("  %s: %s\n", i.item->key.str, i.item->value.str);
    c_hashmap_ss_destroy(&smap);

    c_Hashmap_id mymap = c_hashmap_initializer;
    for (i = 0; i < 600000; ++i)
        c_hashmap_id_put(&mymap, i*i, i);
        
    for (i = 1000; i < 1010; ++i)
        printf("lookup %d: %f\n", i*i, c_hashmap_id_get(mymap, i*i)->value);
    
    c_HashmapEntry_id* me = c_hashmap_id_get(mymap, 10000);
    printf("changed: %d %f\n", me->changed, me->value);
    c_hashmap_id_put(&mymap, 10000, 101.2);
    printf("changed: %d %f\n", me->changed, me->value);

    c_hashmap_id_destroy(&mymap);
}
