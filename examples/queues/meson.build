foreach sample : [
  'new_queue',
  'queue',
]
  test(
    sample,
    executable(
      sample,
      files(f'@sample@.c'),
      dependencies: example_deps,
      install: false,
    ),
    suite: 'queue',
  )
endforeach
