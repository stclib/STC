
#include <stc/cstr.h>
#include <stc/cmap.h>
#include <stc/clist.h>
#include <stc/cvec.h>

using_cvec_str();
using_clist_str();
using_cmap_strkey(si, int);


int main1()
{
    clist_str lwords = clist_INIT;
    c_push_items(&lwords, clist_str, {
        "this", "sentence", "is", "not", "a", "sentence",
        "this", "sentence", "is", "a", "hoax"
    });
    clist_str_push_back(&lwords, cstr_from("%f", 123897.0 / 23.0));
    c_foreach (w, clist_str, lwords)
        printf("%s\n", w.val->str);
    puts("");

    cvec_str words = cvec_INIT;
    c_push_items(&words, cvec_str, {
        "this", "sentence", "is", "not", "a", "sentence",
        "this", "sentence", "is", "a", "hoax"
    });

    cmap_si word_map = cmap_INIT;
    c_foreach (w, cvec_str, words)
        cmap_si_emplace(&word_map, w.val->str, 0).first->second += 1;

    c_foreach (i, cmap_si, word_map) {
        printf("%d occurrences of word '%s'\n", i.val->second, i.val->first.str);
    }

    cmap_si_del(&word_map);
    cvec_str_del(&words);
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