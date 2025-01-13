// https://doc.rust-lang.org/rust-by-example/std/hash.html
#include "stc/cstr.h"
#define i_keypro cstr
#define i_valpro cstr
#include <stdio.h>
#include "stc/hmap.h"

const char* call(const char* number) {
    if (strcmp(number, "798-1364") == 0)
        return "We're sorry, the call cannot be completed as dialed."
               " Please hang up and try again.";
    else if (strcmp(number, "645-7689") == 0)
        return "Hello, this is Mr. Awesome's Pizza. My name is Fred."
               " What can I get for you today?";
    else
        return "Hi! Who is this again?";
}

int main(void) {
    hmap_cstr contacts = {0};

    hmap_cstr_emplace(&contacts, "Daniel", "798-1364");
    hmap_cstr_emplace(&contacts, "Ashley", "645-7689");
    hmap_cstr_emplace(&contacts, "Katie", "435-8291");
    hmap_cstr_emplace(&contacts, "Robert", "956-1745");

    const hmap_cstr_value* v;
    if ((v = hmap_cstr_get(&contacts, "Daniel")))
        printf("Calling Daniel: %s\n", call(cstr_str(&v->second)));
    else
        printf("Don't have Daniel's number.");

    hmap_cstr_emplace_or_assign(&contacts, "Daniel", "164-6743");

    if ((v = hmap_cstr_get(&contacts, "Ashley")))
        printf("Calling Ashley: %s\n", call(cstr_str(&v->second)));
    else
        printf("Don't have Ashley's number.");

    hmap_cstr_erase(&contacts, "Ashley");

    puts("");
    for (c_each_kv(contact, number, hmap_cstr, contacts)) {
        printf("Calling %s: %s\n", cstr_str(contact), call(cstr_str(number)));
    }
    puts("");

    hmap_cstr_drop(&contacts);
}
