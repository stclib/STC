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
#define i_valclass City
#include <stc/cbox.h>
//#include <stc/carc.h> // try instead of cbox.h

#define i_type Cities
#define i_keyboxed CityArc
#include <stc/cvec.h>

#define i_type CityMap
#define i_key int
#define i_valboxed CityArc
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
        // Create a cvec with smart pointers to City objects!
        cities = c_make(Cities, {
            {cstr_lit("New York"), cstr_lit("US"), 40.71427f, -74.00597f, 8804190},
            {cstr_lit("Paris"), cstr_lit("France"), 48.85341f, 2.3488f, 2138551},
            {cstr_lit("Berlin"), cstr_lit("Germany"), 52.52437f, 13.41053f, 3426354},
            {cstr_lit("London"), cstr_lit("UK"), 51.50853f, -0.12574f, 8961989},
        });

        Cities_sort(&cities);

        printf("Vec:\n");
        c_foreach (c, Cities, cities)
            printf("city: %8s, %8d, use: %ld\n", cstr_str(&c.ref->get->name),
                                             c.ref->get->population,
                                             CityArc_use_count(c.ref));
        puts("");
        copy = Cities_clone(cities); // share each city!

        int k = 0, id[] = {8, 4, 3, 9, 2, 5};
        c_foreach (i, Cities, cities)
            CityMap_insert(&map, id[k++], CityArc_clone(*i.ref));

        Cities_pop(&cities);
        Cities_pop(&cities);

        printf("Vec:\n");
        c_foreach (c, Cities, cities)
            printf("city: %8s, %8d, use: %ld\n", cstr_str(&c.ref->get->name),
                                                 c.ref->get->population,
                                                 CityArc_use_count(c.ref));
        printf("\nMap:\n");
        c_forpair (id, city, CityMap, map)
            printf("city: %8s, %8d, use: %ld, id:%d\n", cstr_str(&_.city->get->name),
                    _.city->get->population, CityArc_use_count(_.city), *_.id);
        puts("");
    }
}
