This example is based on https://stackoverflow.com/questions/17016175/c-unordered-map-using-a-custom-class-type-as-the-key/17017281#17017281, adapted to use CMap and CString instead of std::unordered_map and std::string.

To be able to use CMap (or one of the other unordered associative containers) with a user-defined key-type, you need to define two things:

1. A hash function; this must be a function that calculates the hash value given an object of the key-type.

2. A comparison function for equality; this is required because the hash cannot rely on the fact that the hash function will always provide a unique hash value for every distinct key (i.e., it needs to be able to deal with collisions), so it needs a way to compare two given keys for an exact match.

The difficulty with the hash function is that if your key type consists of several members, you will usually have the hash function calculate hash values for the individual members, and then somehow combine them into one hash value for the entire object. For good performance (i.e., few collisions) you should think carefully about how to combine the individual hash values to ensure you avoid getting the same output for different objects too often.

Assuming a key-type like this, and want string as value, we define the functions person_make(), person_destroy() and person_compare():
```
#include <c_lib/cstring.h>

struct Person
{
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

int person_compare(const struct Person* x, const struct Person *y, size_t ignore) {
  int c;
  c = strcmp(x->name.str, y->name.str);   if (c != 0) return c;
  c = strcmp(x->surname.str, y->surname.str); if (c != 0) return c;
  return memcmp(&x->age, &y->age, sizeof(x->age));
}
```
Here is a simple hash function that combines the three member's hashes:
```
size_t person_hash(const struct Person* p, size_t ignore) {
  // Compute individual hash values for name, surname and age
  // http://stackoverflow.com/a/1646913/126995

  size_t res = 17;  
  res = res * 31 + c_defaultHash(p->name.str, cstring_size(p->name));
  res = res * 31 + c_defaultHash(p->surname.str, cstring_size(p->surname));
  res = res * 31 + c_defaultHash(&p->age, sizeof(p->age));
  return res;
}
```
With this in place, you can instantiate a CMap with Person => CString:
```
#include <c_lib/CMap.h>
declare_CMap(ex, struct Person, CString, cstring_destroy, person_compare, person_hash, person_destroy);

int main()
{
  CMap_ex m6 = cmap_initializer;
  cmap_ex_put(&m6, person_make("John", "Doe", 12), cstring_make("example"));
  cmap_ex_put(&m6, person_make("Mary", "Sue", 21), cstring_make("another"));
  
  // ...
  c_foreach (it, cmap_ex, m6) {
      if (cstring_equals(it.item->key.name, "John"))
          printf("%s %s %d -> %s\n", it.item->key.name.str, it.item->key.surname.str, it.item->key.age,
                                     it.item->value.str);
  }
  
  cmap_ex_destroy(&m6);
}
```
It will automatically use person_hash() as defined above for the hash value calculations, and the person_compare() for equality checks.
The cmap_ex_destroy() function will free CStrings name, surname and the value for each item in the map, in addition to the CMap hash table itself.

