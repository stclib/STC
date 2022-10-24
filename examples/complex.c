
// Define similar c++ data types:
//
// using FloatStack = std::stack<float>;
// using StackList = std::stack<FloatStack>;
// using ListMap = std::unordered_map<int, std::forward_list<StackList>>;
// using MapMap = std::unordered_map<std::string, ListMap>;

#include <stc/cstr.h>


#define i_type FloatStack
#define i_val float
#include <stc/cstack.h>

#define i_type StackList
#define i_valclass FloatStack  // "class" picks up _clone, _drop
#define i_opt c_no_cmp         // no FloatStack_cmp()
#include <stc/clist.h>

#define i_type ListMap
#define i_key int
#define i_valclass StackList   // "class" picks up _clone, _drop
#include <stc/cmap.h>

#define i_type MapMap
#define i_key_str
#define i_valclass ListMap
#include <stc/cmap.h>


int main()
{
    c_auto (MapMap, mmap)
    {
        FloatStack stack = FloatStack_with_size(10, 0);

        // Put in some data in the structures
        stack.data[3] = 3.1415927f;
        printf("stack size: %" c_ZU "\n", FloatStack_size(&stack));

        StackList list = StackList_init();
        StackList_push_back(&list, stack);

        ListMap lmap = ListMap_init();
        ListMap_insert(&lmap, 42, list);
        MapMap_insert(&mmap, cstr_from("first"), lmap);

        // Access the data entry
        const ListMap* lmap_p = MapMap_at(&mmap, "first");
        const StackList* list_p = ListMap_at(lmap_p, 42);
        const FloatStack* stack_p = StackList_back(list_p);
        printf("value is: %f\n", *FloatStack_at(stack_p, 3)); // pi
    }
}
