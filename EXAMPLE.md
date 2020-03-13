To be able to use std::unordered_map (or one of the other unordered associative containers) with a user-defined key-type, you need to define two things:

A hash function; this must be a class that overrides operator() and calculates the hash value given an object of the key-type. One particularly straight-forward way of doing this is to specialize the std::hash template for your key-type.

A comparison function for equality; this is required because the hash cannot rely on the fact that the hash function will always provide a unique hash value for every distinct key (i.e., it needs to be able to deal with collisions), so it needs a way to compare two given keys for an exact match. You can implement this either as a class that overrides operator(), or as a specialization of std::equal, or – easiest of all – by overloading operator==() for your key type (as you did already).

The difficulty with the hash function is that if your key type consists of several members, you will usually have the hash function calculate hash values for the individual members, and then somehow combine them into one hash value for the entire object. For good performance (i.e., few collisions) you should think carefully about how to combine the individual hash values to ensure you avoid getting the same output for different objects too often.

A fairly good starting point for a hash function is one that uses bit shifting and bitwise XOR to combine the individual hash values. For example, assuming a key-type like this:
```
struct Key
{
  std::string first;
  std::string second;
  int         third;

  bool operator==(const Key &other) const
  { return (first == other.first
            && second == other.second
            && third == other.third);
  }
};
```
Here is a simple hash function (adapted from the one used in the cppreference example for user-defined hash functions):
```
namespace std {

  template <>
  struct hash<Key>
  {
    std::size_t operator()(const Key& k) const
    {
      using std::size_t;
      using std::hash;
      using std::string;

      // Compute individual hash values for first,
      // second and third and combine them using XOR
      // and bit shifting:

      return ((hash<string>()(k.first)
               ^ (hash<string>()(k.second) << 1)) >> 1)
               ^ (hash<int>()(k.third) << 1);
    }
  };
}
```
With this in place, you can instantiate a std::unordered_map for the key-type:
```
int main()
{
  std::unordered_map<Key,std::string> m6 = {
    { {"John", "Doe", 12}, "example"},
    { {"Mary", "Sue", 21}, "another"}
  };
}
```
It will automatically use std::hash<Key> as defined above for the hash value calculations, and the operator== defined as member function of Key for equality checks.

If you don't want to specialize template inside the std namespace (although it's perfectly legal in this case), you can define the hash function as a separate class and add it to the template argument list for the map:
```
struct KeyHasher
{
  std::size_t operator()(const Key& k) const
  {
    using std::size_t;
    using std::hash;
    using std::string;

    return ((hash<string>()(k.first)
             ^ (hash<string>()(k.second) << 1)) >> 1)
             ^ (hash<int>()(k.third) << 1);
  }
};

int main()
{
  std::unordered_map<Key,std::string,KeyHasher> m6 = {
    { {"John", "Doe", 12}, "example"},
    { {"Mary", "Sue", 21}, "another"}
  };
}
```
How to define a better hash function? As said above, defining a good hash function is important to avoid collisions and get good performance. For a real good one you need to take into account the distribution of possible values of all fields and define a hash function that projects that distribution to a space of possible results as wide and evenly distributed as possible.

This can be difficult; the XOR/bit-shifting method above is probably not a bad start. For a slightly better start, you may use the hash_value and hash_combine function template from the Boost library. The former acts in a similar way as std::hash for standard types (recently also including tuples and other useful standard types); the latter helps you combine individual hash values into one. Here is a rewrite of the hash function that uses the Boost helper functions:

#include <boost/functional/hash.hpp>
```
struct KeyHasher
{
  std::size_t operator()(const Key& k) const
  {
      using boost::hash_value;
      using boost::hash_combine;

      // Start with a hash value of 0    .
      std::size_t seed = 0;

      // Modify 'seed' by XORing and bit-shifting in
      // one member of 'Key' after the other:
      hash_combine(seed,hash_value(k.first));
      hash_combine(seed,hash_value(k.second));
      hash_combine(seed,hash_value(k.third));

      // Return the result.
      return seed;
  }
};
```
And here’s a rewrite that doesn’t use boost, yet uses good method of combining the hashes:
```
namespace std
{
    template <>
    struct hash<Key>
    {
        size_t operator()( const Key& k ) const
        {
            // Compute individual hash values for first, second and third
            // http://stackoverflow.com/a/1646913/126995
            size_t res = 17;
            res = res * 31 + hash<string>()( k.first );
            res = res * 31 + hash<string>()( k.second );
            res = res * 31 + hash<int>()( k.third );
            return res;
        }
    };
}
```
