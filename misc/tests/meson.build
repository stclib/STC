tests = get_option('tests').enable_auto_if(root)

if tests.enabled()
  foreach suite : ['algorithm', 'cregex', 'cspan', 'deque', 'list']
    test(
      f'stctest_@suite@',
      executable(
        f'@suite@_test',
        files(f'@suite@_test.c', 'main.c'),
        include_directories: inc,
        c_args: ['-D_GNU_SOURCE'],
        install: false,
      ),
    )
  endforeach

  install_headers('ctest.h', subdir: 'stc')
endif
