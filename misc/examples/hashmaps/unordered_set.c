// https://iq.opengenus.org/containers-cpp-stl/
// C program to demonstrate various function of stc hset
#define i_implement
#include "stc/cstr.h"
#define i_key_cstr
#include "stc/hset.h"

int main(void)
{
    // declaring set for storing string data-type
    hset_cstr stringSet = {0};
    c_defer(
        hset_cstr_drop(&stringSet)
    ){
        // inserting various string, same string will be stored
        // once in set
        hset_cstr_emplace(&stringSet, "code");
        hset_cstr_emplace(&stringSet, "in");
        hset_cstr_emplace(&stringSet, "C");
        hset_cstr_emplace(&stringSet, "is");
        hset_cstr_emplace(&stringSet, "fast");

        const char* key = "slow";

        //     find returns end iterator if key is not found,
        //  else it returns iterator to that key

        if (hset_cstr_find(&stringSet, key).ref == NULL)
            printf("\"%s\" not found\n", key);
        else
            printf("Found \"%s\"\n", key);

        key = "C";
        if (!hset_cstr_contains(&stringSet, key))
            printf("\"%s\" not found\n", key);
        else
            printf("Found \"%s\"\n", key);

        // now iterating over whole set and printing its
        // content
        printf("All elements :\n");
        c_foreach (itr, hset_cstr, stringSet)
            printf("%s\n", cstr_str(itr.ref));
    }
}
