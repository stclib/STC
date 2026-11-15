// serialize.h
#include <stc/c11/serialize.h>

#define T DVec, double, (c_use_serde)
#include <stc/stack.h>
#define T IMap, int, int, (c_use_serde)
#include <stc/hashmap.h>
#define T DBox, double, (c_use_serde)
#include <stc/box.h>

#define serde_struct_Point \
    (x,, double), \
    (y,, double), \
    (z,, double),

#define serde_struct_Config \
    (task, cstr,), /* NB! only one comma before non-basic types! */ \
    (amount,, int), \
    (point, Point,), \
    (edge_point, Point,, CSERDE_PTR), \
    (name,, const char*), \
    (dbox, DBox,), \
    (intmap, IMap,), \
    (intptr2,, int, CSERDE_PTR), \
    (user_id,, uint64_t), \
    (intptr,, int, CSERDE_PTR), \
    (top_point, Point,, CSERDE_PTR), \
    (numbers, DVec,), \
    (dptr,, double, CSERDE_PTR), \
    (cost,, float), /* final comma is optional */

cserde_define_struct(Point);
cserde_define_struct(Config);

int main()
{
    Config cfg = {
        .task=cstr_from("Hello, this is a cstr"),
        .amount=42,
        .point={1, 2, 42},
        .edge_point=c_new(Point, {1,2,3}),
        .name="Alan\tTuring",
        .intmap = c_make(IMap, {{12, 32}, {77, 99}, {232,333}}),
        .user_id=123456,
        .dbox = DBox_from(3.1415),
        .cost=299.40f,
        .numbers = c_make(DVec, {3.213, 54.33, 2.231, 2.32}),
    };
    cfg.top_point = cfg.edge_point;
    cfg.intptr = &IMap_get_mut(&cfg.intmap, 77)->second;
    cfg.intptr2 = cfg.intptr;
    cfg.dptr = cfg.dbox.get;
    cserde_archive ar = {cserde_pretty, {stdout}};
    Config_to_json(&cfg, &ar);
    puts("");
    return cfg.amount == 0;
}

cserde_implement_save(Point);
cserde_implement_save(Config);
