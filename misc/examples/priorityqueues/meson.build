foreach sample : [
  'functor',
  'new_pqueue',
  'priority',
]
  test(
    sample,
    executable(
      sample,
      files(f'@sample@.c'),
      dependencies: example_deps,
      install: false,
    ),
    suite: 'pqueue',
  )
endforeach
