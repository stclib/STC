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
      build_by_default: false,
    ),
    suite: 'queue',
  )
endforeach
