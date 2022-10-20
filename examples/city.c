#include <stc/cstr.h>

typedef struct {
    cstr name;
    cstr country;
    float lat, lon;
    int population;
} City;

int City_cmp(const City* a, const City* b);
uint64_t City_hash(const City* a);
City City_clone(City c);
void City_drop(City* c);

#define i_type CityArc
#define i_val_class City
#define i_opt c_no_hash

#include <stc/cbox.h>
//#include <stc/carc.h> // try instead of cbox.h

#define i_type Cities
#define i_key_arcbox CityArc
#include <stc/cvec.h>

#define i_type CityMap
#define i_key int
#define i_val_arcbox CityArc
#include <stc/csmap.h>


int City_cmp(const City* a, const City* b) {
    int c = cstr_cmp(&a->name, &b->name);
    return c ? c : cstr_cmp(&a->country, &b->country);
}

uint64_t City_hash(const City* a) {
    return cstr_hash(&a->name) ^ cstr_hash(&a->country);
}

City City_clone(City c) {
    c.name = cstr_clone(c.name);
    c.country = cstr_clone(c.country);
    return c;
}

void City_drop(City* c) {
    printf("drop %s\n", cstr_str(&c->name));
    c_drop(cstr, &c->name, &c->country);
}


int main(void)
{
    c_auto (Cities, cities, copy)
    c_auto (CityMap, map)
    {
        c_forlist (i, City, {
            {cstr_new("New York"), cstr_new("US"), 4.3f, 23.2f, 9000000},
            {cstr_new("Paris"), cstr_new("France"), 4.3f, 23.2f, 9000000},
            {cstr_new("Berlin"), cstr_new("Germany"), 4.3f, 23.2f, 9000000},
            {cstr_new("London"), cstr_new("UK"), 4.3f, 23.2f, 9000000},
        }) Cities_emplace(&cities, *i.ref); // NB. creates smart pointers!

        Cities_sort(&cities);

        printf("Vec:\n");
        c_foreach (c, Cities, cities)
            printf("city:%s, %d, use:%ld\n", cstr_str(&c.ref->get->name),
                                             c.ref->get->population,
                                             CityArc_use_count(c.ref));
        puts("");
        copy = Cities_clone(cities); // share each element!

        int k = 0, id[] = {8, 4, 3, 9, 2, 5};
        c_foreach (i, Cities, cities)
            CityMap_insert(&map, id[k++], CityArc_clone(*i.ref));

        Cities_pop(&cities);
        Cities_pop(&cities);

        printf("Vec:\n");
        c_foreach (c, Cities, cities)
            printf("city:%s, %d, use:%ld\n", cstr_str(&c.ref->get->name),
                                             c.ref->get->population,
                                             CityArc_use_count(c.ref));
        printf("\nMap:\n");
        c_forpair (id, city, CityMap, map)
            printf("id:%d, city:%s, %d, use:%ld\n", *_.id, cstr_str(&_.city->get->name),
                                                     _.city->get->population, CityArc_use_count(_.city));
        puts("");
    }
}
