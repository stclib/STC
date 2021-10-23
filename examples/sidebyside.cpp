#include <iostream>
#include <map>
#include <string>

#define i_key_str
#define i_val int
#include <stc/cmap.h>

#define i_key int
#define i_val int
#include <stc/cmap.h>

int main() {
    {
        std::map<std::string, int> food = {{"burger", 5}, {"pizza", 12}, {"steak", 15}};
        for (auto i: food)
            std::cout << i.first << ", " << i.second << std::endl;
        std::cout << std::endl;
    }
    c_auto (cmap_str, food)
    {
        c_apply_pair(cmap_str, emplace, &food, {{"burger", 5}, {"pizza", 12}, {"steak", 15}});
        c_foreach (i, cmap_str, food)
            printf("%s, %d\n", i.ref->first.str, i.ref->second);
        puts("");
    }

    {
        std::map<int, int> hist;
        ++ hist.emplace(12, 100).first->second;
        ++ hist.emplace(13, 100).first->second;
        ++ hist.emplace(12, 100).first->second;
        for (auto i: hist)
            std::cout << i.first << ", " << i.second << std::endl;
        std::cout << std::endl;
    }
    c_auto (cmap_int, hist)
    {
        ++ cmap_int_emplace(&hist, 12, 100).ref->second;
        ++ cmap_int_emplace(&hist, 13, 100).ref->second;
        ++ cmap_int_emplace(&hist, 12, 100).ref->second;
        c_foreach (i, cmap_int, hist)
            printf("%d, %d\n", i.ref->first, i.ref->second);
        puts("");
    }
}