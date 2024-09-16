foreach sample : [
  'matmult',
  'mdspan',
  'multidim',
  'printspan',
  'submdspan',
]
  test(
    sample,
    executable(
      sample,
      files(f'@sample@.c'),
      dependencies: example_deps,
      install: false,
    ),
    suite: 'cspan',
  )
endforeach
