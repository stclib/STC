foreach sample : [
  'lower_bound',
  'new_vec',
  'stack',
]
  test(
    sample,
    executable(
      sample,
      files(f'@sample@.c'),
      dependencies: example_deps,
      install: false,
    ),
    suite: 'vec',
  )
endforeach
