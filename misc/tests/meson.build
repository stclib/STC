tests = get_option('tests').enable_auto_if(root)

if tests.enabled()
  tests_deps = [
    stc_dep,
    cc.find_library('m', required: false),
  ]
  foreach suite, filter : {
    'algorithm': [
      'cstr_append',
      'c_find_if',
      'c_filter',
    ],
    'cregex': [
      'ISO8601_parse_result',
      'compile_match_char',
      'compile_match_anchors',
      'compile_match_quantifiers1',
      'compile_match_quantifiers2',
      'compile_match_escaped_chars',
      'compile_match_class_simple',
      'compile_match_or',
      'compile_match_class_complex_0',
      'compile_match_class_complex_1',
      'compile_match_cap',
      'search_all',
      'captures_len',
      'captures_cap',
      'replace',
    ],
    'cspan': [
      'subdim',
      'slice',
      'slice2',
      'equality',
    ],
    'hmap': [
      'mapdemo1',
      'mapdemo2',
      'mapdemo3',
    ],
    'smap': [
      'erase',
      'insert',
    ],
    'vec': [
      'basics',
    ],
    'deque': [
      'basics',
    ],
    'list': [
      'splice',
      'erase',
      'misc',
    ],
  }
    test_exe = executable(
      f'@suite@_test',
      files(f'@suite@_test.c', 'main.c'),
      include_directories: inc,
      c_args: ['-D_GNU_SOURCE'],
      dependencies: tests_deps,
      install: false,
    )
    foreach unit : filter
      test(
        unit,
        test_exe,
        args: [unit],
        suite: suite,
      )
    endforeach
  endforeach

  install_headers('ctest.h', subdir: 'stc')
endif
