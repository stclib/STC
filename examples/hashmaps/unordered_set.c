// https://iq.opengenus.org/containers-cpp-stl/
// C program to demonstrate various function of stc hset
#include <stc/cstr.h>
#define T StrSet, cstr, (c_pro_key)
#include <stc/hashset.h>

int main(void)
{
    // declaring set for storing string data-type
    c_with (StrSet stringSet = {0}, StrSet_drop(&stringSet))
    {
        // inserting various string, same string will be stored
        // once in set
        StrSet_emplace(&stringSet, "code");
        StrSet_emplace(&stringSet, "in");
        StrSet_emplace(&stringSet, "C");
        StrSet_emplace(&stringSet, "is");
        StrSet_emplace(&stringSet, "fast");

        const char* key = "slow";

        //     find returns end iterator if key is not found,
        //  else it returns iterator to that key

        if (StrSet_find(&stringSet, key).ref == NULL)
            printf("\"%s\" not found\n", key);
        else
            printf("Found \"%s\"\n", key);

        key = "C";
        if (!StrSet_contains(&stringSet, key))
            printf("\"%s\" not found\n", key);
        else
            printf("Found \"%s\"\n", key);

        // now iterating over whole set and printing its
        // content
        printf("All elements :\n");
        for (c_each(itr, StrSet, stringSet))
            printf("%s\n", cstr_str(itr.ref));
    }
}
