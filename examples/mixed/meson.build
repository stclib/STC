foreach sample : [
  'astar',
  'complex',
  'convert',
  'demos',
  'FMD',
  'inits',
  'read',
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
