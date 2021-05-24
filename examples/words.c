#include <math.h>
#include <stc/cstr.h>
#include <stc/cmap.h>
#include <stc/clist.h>
#include <stc/cvec.h>

using_cvec_str();
using_clist_str();
using_cmap_strkey(si, int);


int main1()
{
    c_var (clist_str, lwords, {
        "this", "sentence", "is", "not", "a", "sentence",
        "this", "sentence", "is", "a", "hoax"
    });
    c_fordefer (clist_str_del(&lwords))
    {
        clist_str_push_back(&lwords, cstr_from_fmt("%.15f", sqrt(2)));
        c_foreach (w, clist_str, lwords)
            printf("%s\n", w.ref->str);
        puts("");

        c_var (cvec_str, words, {
            "this", "sentence", "is", "not", "a", "sentence",
            "this", "sentence", "is", "a", "hoax"
        });
        c_fordefer (cvec_str_del(&words))
        {
            cmap_si word_map = cmap_si_init();
            c_foreach (w, cvec_str, words)
                cmap_si_emplace(&word_map, w.ref->str, 0).ref->second += 1;

            c_foreach (i, cmap_si, word_map) {
                printf("%d occurrences of word '%s'\n", i.ref->second, i.ref->first.str);
            }
        }
    }
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