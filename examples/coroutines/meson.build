foreach sample : [
  'cointerleave',
  'coread',
  'coroutines',
  'coroutine_tasks',
  'producer',
  'producer_task',
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
    suite: 'coroutine',
  )
endforeach
