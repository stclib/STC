cc = meson.get_compiler('c')

if cc.get_id() == 'gcc'
  span_deps = [example_deps, dependency('openmp')]
else
  span_deps = example_deps
endif

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
      dependencies: span_deps,
      install: false,
    ),
    suite: 'cspan',
  )
endforeach
