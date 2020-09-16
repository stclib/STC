
#include <stc/cstr.h>
#include <stc/cmap.h>
#include <stc/clist.h>
#include <stc/cvec.h>

typedef_cvec_str();
typedef_clist_str();
typedef_cmap_strkey(si, int);


int main1()
{
    clist_str lwords = clist_ini;
    c_push_items(&lwords, clist_str, {
        "this", "sentence", "is", "not", "a", "sentence",
        "this", "sentence", "is", "a", "hoax"
    });
    clist_str_push_back(&lwords, cstr_from("%f", 123897.0 / 23.0));
    c_foreach (w, clist_str, lwords)
        printf("%s\n", w.get->value.str);
    puts("");

    cvec_str words = cvec_ini;
    c_push_items(&words, cvec_str, {
        "this", "sentence", "is", "not", "a", "sentence",
        "this", "sentence", "is", "a", "hoax"
    });

    cmap_si word_map = cmap_ini;
    c_foreach (w, cvec_str, words)
        cmap_si_emplace(&word_map, w.get->str, 0).first->second += 1;

    c_foreach (pair, cmap_si, word_map) {
        printf("%d occurrences of word '%s'\n",
               pair.get->second,
               pair.get->first.str);
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