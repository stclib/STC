foreach sample : [
  'cointerleave',
  'coread',
  'coroutines',
  'cotasks1',
  'cotasks2',
  'dining_philosophers',
  'fibonacci',
  'filetask',
  'generator',
  'scheduler',
  'triples',
]
  test(
    sample,
    executable(
      sample,
      files(f'@sample@.c'),
      dependencies: example_deps,
      install: false,
    ),
  )
endforeach
