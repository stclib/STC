// https://doc.rust-lang.org/rust-by-example/std/hash.html
#include <stdio.h>
#include <stc/cstr.h>
#define T StrMap, cstr, cstr, (c_keypro | c_valpro)
#include <stc/hashmap.h>

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
    StrMap contacts = {0};

    StrMap_emplace(&contacts, "Daniel", "798-1364");
    StrMap_emplace(&contacts, "Ashley", "645-7689");
    StrMap_emplace(&contacts, "Katie", "435-8291");
    StrMap_emplace(&contacts, "Robert", "956-1745");

    const StrMap_value* v;
    if ((v = StrMap_get(&contacts, "Daniel")))
        printf("Calling Daniel: %s\n", call(cstr_str(&v->second)));
    else
        printf("Don't have Daniel's number.");

    StrMap_emplace_or_assign(&contacts, "Daniel", "164-6743");

    if ((v = StrMap_get(&contacts, "Ashley")))
        printf("Calling Ashley: %s\n", call(cstr_str(&v->second)));
    else
        printf("Don't have Ashley's number.");

    StrMap_erase(&contacts, "Ashley");

    puts("");
    for (c_each_kv(contact, number, StrMap, contacts)) {
        printf("Calling %s: %s\n", cstr_str(contact), call(cstr_str(number)));
    }
    puts("");

    StrMap_drop(&contacts);
}
