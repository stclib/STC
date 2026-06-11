// https://iq.opengenus.org/containers-cpp-stl/
// C program to demonstrate various function of stc hset
#include "stc/cstr.h"
#define T StrSet, cstr, (c_pro_key)
#include "stc/hashset.h"

int main(void)
{
    // declaring set for storing string data-type
    StrSet stringSet = c_make(StrSet, {"code", "in", "C", "is", "fast"});

    const char* key = "slow";

    //     find returns end iterator if key is not found,
    //  else it returns iterator to that key

    if (StrSet_contains(&stringSet, key))
        printf("Found \"%s\"\n", key);
    else
        printf("\"%s\" not found\n", key);

    key = "C";
    if (StrSet_contains(&stringSet, key))
        printf("Found \"%s\"\n", key);
    else
        printf("\"%s\" not found\n", key);

    // now iterating over whole set and printing its
    // content
    printf("All elements :\n");
    for (c_each(itr, StrSet, stringSet))
        printf("%s\n", cstr_str(itr.ref));

    StrSet_drop(&stringSet);
}
