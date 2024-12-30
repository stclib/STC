foreach sample : [
  'cstr_match',
  'replace',
  'splitstr',
  'sso_map',
  'sso_substr',
  'sview_split',
  'utf8replace_c',
]
  test(
    sample,
    executable(
      sample,
      files(f'@sample@.c'),
      dependencies: example_deps,
      install: false,
    ),
    suite: 'cstr',
  )
endforeach
