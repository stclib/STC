#include <stc/cstr.h>
#include <stc/forward.h>

forward_cdeq(cdeq_i32, int);
forward_cdeq(cdeq_pnt, struct Point);

struct MyStruct {
    cdeq_i32 intvec;
    cdeq_pnt pntvec;
} typedef MyStruct;


#define i_val int
#define i_opt c_is_forward
#define i_tag i32
#include <stc/cdeq.h>

struct Point { int x, y; } typedef Point;
int point_cmp(const Point* a, const Point* b) {
    int c = a->x - b->x;
    return c ? c : a->y - b->y;
}

#define i_val Point
#define i_cmp point_cmp
#define i_opt c_is_forward
#define i_tag pnt
#include <stc/cdeq.h>


int main()
{
    cdeq_pnt pvec = {0};

    cdeq_pnt_push_back(&pvec, (Point){42, 14});
    cdeq_pnt_push_back(&pvec, (Point){32, 94});
    cdeq_pnt_push_front(&pvec, (Point){62, 81});

    cdeq_pnt_sort(&pvec);
    
    c_foreach (i, cdeq_pnt, pvec)
        printf(" (%d %d)", i.ref->x, i.ref->y);
    puts("");
    
    cdeq_pnt_drop(&pvec);
}
