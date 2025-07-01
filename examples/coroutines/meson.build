foreach sample : [
  'cointerleave',
  'coread',
  'coroutines1',
  'coroutines2',
  'coroutines3',
  'producer',
  'producer_task',
  'dining_philosophers',
  'fibonacci',
  'filetask',
  'generator',
  'scheduler',
  'triples',
  'waitgroup',
]
  test(
    sample,
    executable(
      sample,
      files(f'@sample@.c'),
      dependencies: example_deps,
      install: false,
    ),
    suite: 'coroutine',
  )
endforeach
