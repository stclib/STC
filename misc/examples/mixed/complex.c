
// Define similar c++ data types:
//
// using FloatStack = std::stack<float>;
// using StackList = std::stack<FloatStack>;
// using ListMap = std::unordered_map<int, std::forward_list<StackList>>;
// using MapMap = std::unordered_map<std::string, ListMap>;
#define i_implement
#include "stc/cstr.h"

#define i_TYPE FloatStack,float
#include "stc/stack.h"

#define i_type StackList
#define i_keyclass FloatStack  // "class" picks up _clone, _drop, _cmp
#include "stc/list.h"

#define i_type ListMap
#define i_key int
#define i_valclass StackList   // "class" picks up _clone, _drop
#include "stc/hmap.h"

#define i_type MapMap
#define i_key_str
#define i_valclass ListMap
#include "stc/hmap.h"


int main(void)
{
    MapMap mmap = {0};

    // Put in some data in the structures
    ListMap* lmap = &MapMap_emplace(&mmap, "first", ListMap_init()).ref->second;
    StackList* list = &ListMap_insert(lmap, 42, StackList_init()).ref->second;
    FloatStack* stack = StackList_push_back(list, FloatStack_with_size(10, 0));
    stack->data[3] = 3.1415927f;
    printf("stack size: %" c_ZI "\n", FloatStack_size(stack));

    // Access the data entry
    const ListMap* lmap_p = MapMap_at(&mmap, "first");
    const StackList* list_p = ListMap_at(lmap_p, 42);
    const FloatStack* stack_p = StackList_back(list_p);
    printf("value is: %f\n", (double)*FloatStack_at(stack_p, 3)); // pi

    MapMap_drop(&mmap);
}
