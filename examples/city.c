#include <stc/cstr.h>

typedef struct {
    cstr name;
    cstr country;
    float lat, lon;
    int population;
} City;

static inline int City_cmp(const City* a, const City* b) {
    int c = cstr_cmp(&a->name, &b->name);
    return c ? c : cstr_cmp(&a->country, &b->country);
}

static inline uint64_t City_hash(const City* a) {
    printf("hash %s\n", cstr_str(&a->name));
    return cstr_hash(&a->name) ^ cstr_hash(&a->country);
}

static inline City City_clone(City c) {
    printf("clone %s\n", cstr_str(&c.name));
    c.name = cstr_clone(c.name);
    c.country = cstr_clone(c.country);
    return c;
}

static inline void City_drop(City* c) {
    printf("drop %s\n", cstr_str(&c->name));
    c_drop(cstr, &c->name, &c->country);
}


#define i_type CityArc
#define i_key_bind City
#include <stc/cbox.h>
//#include <stc/carc.h> // try instead of cbox.h

#define i_type Cities
#define i_key_arcbox CityArc
#include <stc/cvec.h>

#define i_type CityMap
#define i_key int
#define i_val_arcbox CityArc
#include <stc/csmap.h>


int main(void)
{
    c_auto (Cities, cities, copy)
    c_auto (CityMap, map)
    {
        struct City_s { const char *name, *country; float lat, lon; int pop; };

        c_apply(c, Cities_push(&cities, CityArc_make((City){cstr_from(c->name), cstr_from(c->country),
                                                            c->lat, c->lon, c->pop})), struct City_s, {
            {"New York", "US", 4.3, 23.2, 9000000},
            {"Paris", "France", 4.3, 23.2, 9000000},
            {"Berlin", "Germany", 4.3, 23.2, 9000000},
            {"London", "UK", 4.3, 23.2, 9000000},
        });

        copy = Cities_clone(cities); // share each element!

        int k = 0, id[] = {8, 4, 3, 9, 2, 5};
        c_foreach (i, Cities, cities)
            CityMap_insert(&map, id[k++], CityArc_clone(*i.ref));

        Cities_pop(&cities);
        Cities_pop(&cities);

        printf("Vec:\n");
        c_foreach (c, Cities, cities)
            printf("city:%s, %d, use:%ld\n", cstr_str(&c.ref->get->name), c.ref->get->population, CityArc_use_count(*c.ref));

        printf("\nMap:\n");
        c_forpair (id, city, CityMap, map)
            printf("id:%d, city:%s, %d, use:%ld\n", *_.id, cstr_str(&_.city->get->name),
                                                    _.city->get->population, CityArc_use_count(*_.city));
        puts("");
    }
}
