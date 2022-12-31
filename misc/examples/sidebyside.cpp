#include <iostream>
#include <map>
#include <string>
#include <stc/cstr.h>

#define i_type IIMap
#define i_key int
#define i_val int
#include <stc/csmap.h>

#define i_type SIMap
#define i_key_str
#define i_val int
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
    
    c_AUTO (IIMap, hist)
    {
        IIMap_insert(&hist, 12, 100).ref->second += 1;
        IIMap_insert(&hist, 13, 100).ref->second += 1;
        IIMap_insert(&hist, 12, 100).ref->second += 1;
        
        c_FOREACH (i, IIMap, hist)
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

    c_AUTO (SIMap, food)
    {
        c_FORLIST (i, SIMap_raw, {{"burger", 5}, {"pizza", 12}, {"steak", 15}})
            SIMap_emplace(&food, c_PAIR(i.ref));
        
        c_FOREACH (i, SIMap, food)
            printf("%s, %d\n", cstr_str(&i.ref->first), i.ref->second);
        puts("");
    }
}
