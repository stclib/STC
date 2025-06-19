foreach sample : [
  'filterdemos',
  'forloops',
  'random',
  'shape',
  'tree',
  'tree_rc',
]
  test(
    sample,
    executable(
      sample,
      files(f'@sample@.c'),
      dependencies: example_deps,
      install: false,
    ),
    suite: 'algorithm',
  )
endforeach
