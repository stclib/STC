foreach sample : [
  'intrusive',
  'list_erase',
  'lists',
  'list_splice',
  'new_list',
]
  test(
    sample,
    executable(
      sample,
      files(f'@sample@.c'),
      dependencies: example_deps,
      install: false,
    ),
    suite: 'list',
  )
endforeach
