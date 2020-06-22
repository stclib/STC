/*To be able to use CHash with a user-defined key-type, you need to define two things:

1. A hash function; this must be a function that calculates the hash value given an object of the key-type.

2. A comparison function for equality; this is required because the hash cannot rely on the fact that the hash function will always provide a unique hash value for every distinct key (i.e., it needs to be able to deal with collisions), so it needs a way to compare two given keys for an exact match.

The difficulty with the hash function is that if your key type consists of several members, you will usually have the hash function calculate hash values for the individual members, and then somehow combine them into one hash value for the entire object. For good performance (i.e., few collisions) you should think carefully about how to combine the individual hash values to ensure you avoid getting the same output for different objects too often.

Assuming a key-type like this, and want string as value, we define the functions person_make(), person_destroy() and person_compare():
```*/
#include <stdio.h>
#include "../stc/chash.h"
#include "../stc/cstring.h"

struct Person {
  CString name;
  CString surname;
  int age;
};

struct Person person_make(const char* name, const char* surname, int age) {
  struct Person person = {cstring_make(name), cstring_make(surname), age};
  return person;
}

void person_destroy(struct Person* p) {
  cstring_destroy(&p->name);
  cstring_destroy(&p->surname);
}
/*```
In order to use Person as a map key, provide a "view" of your class that owns no resources (e.g. CString):
```*/
struct PersonView {
  const char* name;
  const char* surname;
  int age;
};

struct PersonView person_getView(struct Person* p) {
  return (struct PersonView) {p->name.str, p->surname.str, p->age};
}

struct Person person_fromView(struct PersonView pv) {
  return (struct Person) {cstring_make(pv.name), cstring_make(pv.surname), pv.age};
}

int personview_compare(const struct PersonView* x, const struct PersonView* y) {
  int c;
  c = strcmp(x->name, y->name);       if (c != 0) return c;
  c = strcmp(x->surname, y->surname); if (c != 0) return c;
  return x->age - y->age;
}
/*```
And a hash function that combines the three member's hashes:
```*/
size_t personview_hash(const struct PersonView* pv, size_t ignore) {
  // http://stackoverflow.com/a/1646913/126995
  size_t res = 17;  
  res = res * 31 + c_defaultHash(pv->name, strlen(pv->name));
  res = res * 31 + c_defaultHash(pv->surname, strlen(pv->surname));
  res = res * 31 + c_defaultHash(&pv->age, sizeof(pv->age));
  return res;
}
/*```
With this in place, we can declare the map Person -> int:
```*/
declare_CHash(ex, MAP, struct Person, int, c_emptyDestroy, personview_hash, personview_compare, 
                  struct PersonView, person_destroy, person_getView, person_fromView);
/*```
Note we use struct PersonView to put keys in the map, but keys are stored as struct Person with proper dynamically allocated CStrings to store name and surname.
```*/
int main()
{
  CHash_ex m6 = chash_init;
  chash_ex_put(&m6, (struct PersonView){"John", "Doe", 24}, 1001);
  chash_ex_put(&m6, (struct PersonView){"Jane", "Doe", 21}, 1002);
  chash_ex_put(&m6, (struct PersonView){"John", "Travolta", 66}, 1003);

  c_foreach (it, chash_ex, m6) {
      if (cstring_equals(it.item->key.name, "John"))
          printf("%s %s %d -> %d\n", it.item->key.name.str, it.item->key.surname.str, it.item->key.age,
                                     it.item->value);
  }

  chash_ex_destroy(&m6);
}
/*```
CHash uses personview_hash() for hash value calculations, and the personview_compare() for equality checks. The chash_ex_destroy() function will free CStrings name, surname and the value for each item in the map, in addition to the CHash hash table itself.
*/
