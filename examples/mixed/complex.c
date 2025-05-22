
// Define similar c++ data types:
//
// using FloatStack = std::stack<float>;
// using StackList = std::forward_list<FloatStack>;
// using ListMap = std::unordered_map<int, StackList>;
// using MapMap = std::unordered_map<std::string, ListMap>;
#include <stc/cstr.h>

#define T FloatStack, float
#define i_keydrop(p) printf("drop %g\n", *p)
#define i_keyclone(v) v // only because i_keydrop was defined
#include <stc/stack.h>

// container as elements is "class"; has _clone() and _drop() "members"
#define T StackList, FloatStack, (c_keyclass)
#include <stc/list.h>

// same, but StackList is the mapped value type, not the key:
#define T ListMap, int, StackList, (c_valclass)
#include <stc/hashmap.h>

// cstr is "pro"; has _clone, _drop, _cmp, _hash, _toraw, and _from.
#define T MapMap, cstr, ListMap, (c_keypro | c_valclass)
#include <stc/hashmap.h>


int main(void)
{
    MapMap mmap = {0};

    // Put in some data in the structures
    ListMap* lmap = &MapMap_emplace(&mmap, "first", ListMap_init()).ref->second;
    StackList* list = &ListMap_insert(lmap, 42, StackList_init()).ref->second;
    FloatStack* stack = StackList_push_back(list, FloatStack_with_size(10, 42));
    stack->data[3] = 3.1415927f;

    // Access the data entry
    const ListMap* lmap_p = MapMap_at(&mmap, "first");
    const StackList* list_p = ListMap_at(lmap_p, 42);
    const FloatStack* stack_p = StackList_back(list_p);

    printf("value is: %g\n",
        stack_p->data[3] // pi
    );
    printf("directly: %g\n",
        StackList_back(ListMap_at(MapMap_at(&mmap, "first"), 42))->data[3] // pi
    );

    // Free everything
    MapMap_drop(&mmap);
}
