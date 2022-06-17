#include <iostream>
#include <map>
#include <string>
#include <stc/cstr.h>

#define i_key int
#define i_val int
#define i_tag ii
#include <stc/csmap.h>

#define i_key_str
#define i_val int
#define i_tag si
#include <stc/cmap.h>

int main() {
    {
        std::map<int, int> hist;
        hist.emplace(12, 100).first->second += 1;
        hist.emplace(13, 100).first->second += 1;
        hist.emplace(12, 100).first->second += 1;
        
        for (auto i: hist)
            std::cout << i.first << ", " << i.second << std::endl;
        std::cout << std::endl;
    }
    
    c_auto (csmap_ii, hist)
    {
        csmap_ii_insert(&hist, 12, 100).ref->second += 1;
        csmap_ii_insert(&hist, 13, 100).ref->second += 1;
        csmap_ii_insert(&hist, 12, 100).ref->second += 1;
        
        c_foreach (i, csmap_ii, hist)
            printf("%d, %d\n", i.ref->first, i.ref->second);
        puts("");
    }
    // ===================================================
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
            printf("%s, %d\n", cstr_str(&i.ref->first), i.ref->second);
        puts("");
    }
}
