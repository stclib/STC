#include <stdio.h>
#include <stc/types.h>

declare_list(list_i32, int32_t);
declare_list(list_pnt, struct Point);

typedef struct {
    list_i32 intlist;
    list_pnt pntlist;
} MyStruct;

#define T list_i32, int32_t, (c_declared)
#include <stc/list.h>

typedef struct Point { int x, y; } Point;
int point_cmp(const Point* a, const Point* b) {
    int c = a->x - b->x;
    return c ? c : a->y - b->y;
}

#define T list_pnt, Point, (c_declared)
#define i_cmp point_cmp
#include <stc/list.h>

// use < and == operators for comparison
#define T list_float, float, (c_use_cmp)
#include <stc/list.h>

void MyStruct_drop(MyStruct* s);

// exclude cloning support because of class/drop:
#define T MyList, MyStruct, (c_keyclass | c_no_clone)
#include <stc/list.h>

void MyStruct_drop(MyStruct* s) {
    list_i32_drop(&s->intlist);
    list_pnt_drop(&s->pntlist);
}


int main(void)
{
    MyStruct my = {0};
    list_i32_push_back(&my.intlist, 123);
    list_pnt_push_back(&my.pntlist, c_literal(Point){123, 456});
    MyStruct_drop(&my);

    list_pnt plist = c_make(list_pnt, {{42, 14}, {32, 94}, {62, 81}});
    list_pnt_sort(&plist);

    for (c_each_item(i, list_pnt, plist))
        printf(" (%d %d)", i->x, i->y);
    puts("");
    list_pnt_drop(&plist);


    list_float flist = c_make(list_float, {123.3f, 321.2f, -32.2f, 78.2f});
    list_float_sort(&flist);

    for (c_each_item(i, list_float, flist))
        printf(" %g", (double)*i);

    puts("");
    list_float_drop(&flist);
}
