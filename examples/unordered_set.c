// https://iq.opengenus.org/containers-cpp-stl/
// C program to demonstrate various function of stc cset
#define i_key_str
#include <stc/cset.h>

int main()
{
    // declaring set for storing string data-type
    c_auto (cset_str, stringSet)
    {
        // inserting various string, same string will be stored
        // once in set
        cset_str_emplace(&stringSet, "code");
        cset_str_emplace(&stringSet, "in");
        cset_str_emplace(&stringSet, "C");
        cset_str_emplace(&stringSet, "is");
        cset_str_emplace(&stringSet, "fast");

        const char* key = "slow";

        //     find returns end iterator if key is not found,
        //  else it returns iterator to that key

        if (cset_str_find(&stringSet, key).ref == cset_str_end(&stringSet).ref)
            printf("\"%s\" not found\n", key);
        else
            printf("Found \"%s\"\n", key);

        key = "C";
        if (!cset_str_contains(&stringSet, key))
            printf("\"%s\" not found\n", key);
        else
            printf("Found \"%s\"\n", key);

        // now iterating over whole set and printing its
        // content
        printf("All elements :\n");
        c_foreach (itr, cset_str, stringSet)
            printf("%s\n", cstr_str(itr.ref));
    }
}
