foreach sample : [
  'regex1',
  'regex2',
  'regex_match',
  'regex_replace',
]
  test(
    sample,
    executable(
      sample,
      files(f'@sample@.c'),
      dependencies: example_deps,
      install: false,
    ),
    suite: 'cregex',
  )
endforeach
