
#include <stc/cstr.h>
#include <stc/cmap.h>
#include <stc/clist.h>
#include <stc/cvec.h>

declare_cvec_str();
declare_clist_str();
declare_cmap_str(si, int);

typedef const char* input_t;
 
int main1()
{
    clist_str lwords = clist_init;
    c_push(&lwords, clist_str, c_items(
        "this", "sentence", "is", "not", "a", "sentence",
        "this", "sentence", "is", "a", "hoax"
    ));
    c_foreach (w, clist_str, lwords) {
        printf("%s\n", w.item->value);
    }
    
    
    cvec_str words = cvec_init;
    c_push(&words, cvec_str, c_items(
        "this", "sentence", "is", "not", "a", "sentence",
        "this", "sentence", "is", "a", "hoax"
    ));

    cmap_si word_map = cmap_init;
    c_foreach (w, cvec_str, words) {
        ++cmap_si_insert(&word_map, w.item->str, 0)->value;
    }
 
    c_foreach (pair, cmap_si, word_map) {
        printf("%d occurrences of word '%s'\n",
               pair.item->value,
               pair.item->key.str);
    }

    cmap_si_destroy(&word_map);
    cvec_str_destroy(&words);
    return 0;
}

#ifdef __cplusplus
#include <string>
#include <iostream>
#include <vector>
#include <unordered_map>
 
int main2()
{
    std::vector<std::string> words = { 
        "this", "sentence", "is", "not", "a", "sentence",
        "this", "sentence", "is", "a", "hoax" 
    };
 
    std::unordered_map<std::string, size_t>  word_map;
    for (const auto &w : words) {
        ++word_map[w];
    }
 
    for (const auto &pair : word_map) {
        std::cout << pair.second
                  << " occurrences of word '"
	              << pair.first << "'\n";
    }
    return 0;
}

int main() {
    main1();
    puts("");
    main2();
}
#else
int main() {
    main1();
}
#endif    