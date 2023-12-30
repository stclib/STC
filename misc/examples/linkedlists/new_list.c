#include <stdio.h>
#include "stc/types.h"

forward_list(list_i32, int32_t);
forward_list(list_pnt, struct Point);

typedef struct {
    list_i32 intlist;
    list_pnt pntlist;
} MyStruct;

#define i_TYPE list_i32, int32_t
#define i_is_forward
#include "stc/list.h"

typedef struct Point { int x, y; } Point;
int point_cmp(const Point* a, const Point* b) {
    int c = a->x - b->x;
    return c ? c : a->y - b->y;
}

#define i_TYPE list_pnt, Point
#define i_cmp point_cmp
#define i_is_forward
#include "stc/list.h"

#define i_key float
#define i_use_cmp               // use < and == operators for comparison
#include "stc/list.h"

void MyStruct_drop(MyStruct* s);
#define i_type MyList
#define i_key MyStruct
#define i_keydrop MyStruct_drop   // define drop function
#define i_no_clone                // must explicitely exclude or define cloning support because of drop.
#include "stc/list.h"

void MyStruct_drop(MyStruct* s) {
    list_i32_drop(&s->intlist);
    list_pnt_drop(&s->pntlist);
}


int main(void)
{
    MyStruct my = {0};
    list_i32_push_back(&my.intlist, 123);
    list_pnt_push_back(&my.pntlist, c_LITERAL(Point){123, 456});
    MyStruct_drop(&my);

    list_pnt plist = c_init(list_pnt, {{42, 14}, {32, 94}, {62, 81}});
    list_pnt_sort(&plist);

    c_foreach (i, list_pnt, plist)
        printf(" (%d %d)", i.ref->x, i.ref->y);
    puts("");
    list_pnt_drop(&plist);


    list_float flist = c_init(list_float, {123.3f, 321.2f, -32.2f, 78.2f});
    list_float_sort(&flist);

    c_foreach (i, list_float, flist)
        printf(" %g", (double)*i.ref);

    puts("");
    list_float_drop(&flist);
}
