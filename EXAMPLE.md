This example is based on https://stackoverflow.com/questions/17016175/c-unordered-map-using-a-custom-class-type-as-the-key/17017281#17017281, adapted to use CMap and CString instead of std::unordered_map and std::string.

To be able to use CMap (or one of the other unordered associative containers) with a user-defined key-type, you need to define two things:

1. A hash function; this must be a function that calculates the hash value given an object of the key-type.

2. A comparison function for equality; this is required because the hash cannot rely on the fact that the hash function will always provide a unique hash value for every distinct key (i.e., it needs to be able to deal with collisions), so it needs a way to compare two given keys for an exact match.

The difficulty with the hash function is that if your key type consists of several members, you will usually have the hash function calculate hash values for the individual members, and then somehow combine them into one hash value for the entire object. For good performance (i.e., few collisions) you should think carefully about how to combine the individual hash values to ensure you avoid getting the same output for different objects too often.

Assuming a key-type like this, and want string as value, we define the functions key_make(), key_destroy() and key_compare():
```
#include <clib/cstring.h>

struct Key
{
  CString first;
  CString second;
  int third;
};

struct Key key_make(const char* first, const char* second, int third) {
  struct Key k = {cstring_make(first), cstring_make(second), third};
  return k;
}

void key_destroy(struct Key* key) {
  cstring_destroy(&key->first);
  cstring_destroy(&key->second);
}

int key_compare(const struct Key* x, const struct Key *y, size_t ignore) {
  int c;
  c = strcmp(x->first.str, y->first.str);   if (c != 0) return c;
  c = strcmp(x->second.str, y->second.str); if (c != 0) return c;
  return memcmp(&x->third, &y->third, sizeof(x->third));
}
```
Here is a simple hash function that combines the three member's hashes:
```
size_t key_hash(const struct Key* k, size_t ignore) {
  // Compute individual hash values for first, second and third
  // http://stackoverflow.com/a/1646913/126995

  size_t res = 17;  
  res = res * 31 + c_murmurHash(k->first.str, cstring_size(k->first));
  res = res * 31 + c_murmurHash(k->second.str, cstring_size(k->second));
  res = res * 31 + c_murmurHash(&k->third, sizeof(k->third));
  return res;
}
```
With this in place, you can instantiate a CMap with Key => CString:
```
#include <clib/CMap.h>
declare_CMap(mm, struct Key, CString, cstring_destroy, key_compare, key_hash, key_destroy);

int main()
{
  CMap_mm m6 = cmap_initializer;
  cmap_mm_put(&m6, key_make("John", "Doe", 12), cstring_make("example"));
  cmap_mm_put(&m6, key_make("Mary", "Sue", 21), cstring_make("another"));
  
  // ...
  c_foreach (it, cmap_mm, m6) {
      if (cstring_equals(it.item->key.first, "John"))
          printf("%s %s %d -> %s\n", it.item->key.first.str, it.item->key.second.str, it.item->key.third,
                                     it.item->value.str);
  }
  
  cmap_mm_destroy(&m6);
}
```
It will automatically use key_hash() as defined above for the hash value calculations, and the key_compare() for equality checks.
The cmap_mm_destroy() function will free all first, second CString's, and the CString value for each item in the map, in addition to the CMap hash table itself.

