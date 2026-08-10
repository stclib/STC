foreach sample : [
  'bits2',
  'bits',
  'prime',
]
  test(
    sample,
    executable(
      sample,
      files(f'@sample@.c'),
      dependencies: example_deps,
      install: false,
      build_by_default: false,
    ),
    suite: 'cbits',
  )
endforeach
