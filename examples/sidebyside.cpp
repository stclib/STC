#include <iostream>
#include <map>
#include <string>

#define i_key_str
#define i_val int
#define i_tag si
#include <stc/cmap.h>

#define i_key int
#define i_val int
#define i_tag ii
#include <stc/csmap.h>

int main() {
    {
        std::map<std::string, int> food = 
            {{"burger", 5}, {"pizza", 12}, {"steak", 15}};
        
        for (auto i: food)
            std::cout << i.first << ", " << i.second << std::endl;
        std::cout << std::endl;
    }
    c_auto (cmap_si, food)
    {
        c_apply(v, cmap_si_emplace(&food, c_pair(v)), cmap_si_raw,
            {{"burger", 5}, {"pizza", 12}, {"steak", 15}});
        
        c_foreach (i, cmap_si, food)
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
    c_auto (csmap_ii, hist)
    {
        ++ csmap_ii_insert(&hist, 12, 100).ref->second;
        ++ csmap_ii_insert(&hist, 13, 100).ref->second;
        ++ csmap_ii_insert(&hist, 12, 100).ref->second;
        
        c_foreach (i, csmap_ii, hist)
            printf("%d, %d\n", i.ref->first, i.ref->second);
        puts("");
    }
}
