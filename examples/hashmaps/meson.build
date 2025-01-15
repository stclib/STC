foreach sample : [
  'birthday',
  'books',
  'hashmap',
  'mapmap',
  'new_map',
  'phonebook',
  'unordered_set',
  'vikings',
]
  test(
    sample,
    executable(
      sample,
      files(f'@sample@.c'),
      dependencies: example_deps,
      install: false,
    ),
    suite: 'hmap',
  )
endforeach
