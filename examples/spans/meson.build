omp = dependency('openmp')

foreach sample : [
  'matmult',
  'mdspan',
  'multidim',
  'printspan',
  'submdspan',
  'span_demo',
]
  test(
    sample,
    executable(
      sample,
      files(f'@sample@.c'),
      dependencies: [example_deps, omp],
      install: false,
    ),
    suite: 'cspan',
  )
endforeach
