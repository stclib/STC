foreach sample : [
  'gauss2',
  'listmap',
  'mapmap',
  'multimap',
  'new_smap',
  'smap_erase',
  'smap_find',
  'smap_insert',
  'sorted_map',
  'sset_erase',
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
