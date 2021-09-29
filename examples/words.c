#include <math.h>
#include <stc/cstr.h>

#define i_val_str
#include <stc/cvec.h>

#define i_tag strn
#define i_key_str
#define i_val int
#include <stc/cmap.h>

int main1()
{
    c_auto (cvec_str, words)
    c_auto (cmap_strn, word_map)
    {
        c_apply(cvec_str, emplace_back, &words, {
            "this", "sentence", "is", "not", "a", "sentence",
            "this", "sentence", "is", "a", "hoax"
        });

        c_foreach (w, cvec_str, words) {
            cmap_strn_emplace(&word_map, w.ref->str, 0).ref->second += 1;
        }

        c_foreach (i, cmap_strn, word_map) {
            printf("%d occurrences of word '%s'\n",
                   i.ref->second, i.ref->first.str);
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
        word_map[w] += 1;
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