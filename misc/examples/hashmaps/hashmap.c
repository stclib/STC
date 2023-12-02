// https://doc.rust-lang.org/rust-by-example/std/hash.html
#define i_implement
#include "stc/cstr.h"
#define i_key_str
#define i_val_str
#include <stdio.h>
#include "stc/hmap.h"

const char* call(const char* number) {
    if (!strcmp(number, "798-1364"))
        return "We're sorry, the call cannot be completed as dialed."
               " Please hang up and try again.";
    else if (!strcmp(number, "645-7689"))
        return "Hello, this is Mr. Awesome's Pizza. My name is Fred."
               " What can I get for you today?";
    else
        return "Hi! Who is this again?";
}

int main(void) {
    hmap_str contacts = {0};

    hmap_str_emplace(&contacts, "Daniel", "798-1364");
    hmap_str_emplace(&contacts, "Ashley", "645-7689");
    hmap_str_emplace(&contacts, "Katie", "435-8291");
    hmap_str_emplace(&contacts, "Robert", "956-1745");

    const hmap_str_value* v;
    if ((v = hmap_str_get(&contacts, "Daniel")))
        printf("Calling Daniel: %s\n", call(cstr_str(&v->second)));
    else
        printf("Don't have Daniel's number.");

    hmap_str_emplace_or_assign(&contacts, "Daniel", "164-6743");

    if ((v = hmap_str_get(&contacts, "Ashley")))
        printf("Calling Ashley: %s\n", call(cstr_str(&v->second)));
    else
        printf("Don't have Ashley's number.");

    hmap_str_erase(&contacts, "Ashley");

    puts("");
    c_forpair (contact, number, hmap_str, contacts) {
        printf("Calling %s: %s\n", cstr_str(_.contact), call(cstr_str(_.number)));
    }
    puts("");

    hmap_str_drop(&contacts);
}
