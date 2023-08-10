#include <stdio.h>
#include <stc/forward.h>

forward_clist(clist_i32, int);
forward_clist(clist_pnt, struct Point);

typedef struct {
    clist_i32 intlist;
    clist_pnt pntlist;
} MyStruct;

#define i_key int
#define i_tag i32
#define i_is_forward
#include <stc/clist.h>

typedef struct Point { int x, y; } Point;
int point_cmp(const Point* a, const Point* b) {
    int c = a->x - b->x;
    return c ? c : a->y - b->y;
}

#define i_key Point
#define i_cmp point_cmp
#define i_is_forward
#define i_tag pnt
#include <stc/clist.h>

#define i_key float
#define i_use_cmp               // use < and == operators for comparison 
#include <stc/clist.h>

void MyStruct_drop(MyStruct* s);
#define i_type MyList
#define i_key MyStruct
#define i_keydrop MyStruct_drop   // define drop function
#define i_no_clone                // must explicitely exclude or define cloning support because of drop.
#include <stc/clist.h>

void MyStruct_drop(MyStruct* s) {
    clist_i32_drop(&s->intlist);
    clist_pnt_drop(&s->pntlist);
}


int main(void)
{
    MyStruct my = {0};
    clist_i32_push_back(&my.intlist, 123);
    clist_pnt_push_back(&my.pntlist, c_LITERAL(Point){123, 456});
    MyStruct_drop(&my);

    clist_pnt plist = c_init(clist_pnt, {{42, 14}, {32, 94}, {62, 81}});
    clist_pnt_sort(&plist);

    c_foreach (i, clist_pnt, plist) 
        printf(" (%d %d)", i.ref->x, i.ref->y);
    puts("");
    clist_pnt_drop(&plist);


    clist_float flist = c_init(clist_float, {123.3f, 321.2f, -32.2f, 78.2f});
    clist_float_sort(&flist);

    c_foreach (i, clist_float, flist)
        printf(" %g", (double)*i.ref);

    puts("");
    clist_float_drop(&flist);
}
