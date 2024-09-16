project(
  'stc',
  'c',
  version: '5.0-dev',
  license: 'MIT',
  default_options: ['c_std=c11', 'warning_level=2'],
)

cc = meson.get_compiler('c')

inc = include_directories('include')
stc_lib = library(
  'stc',
  files(
    'src/cregex.c',
    'src/cspan.c',
    'src/cstr_core.c',
    'src/cstr_unicode.c',
    'src/csview.c',
    'src/fmt.c',
  ),
  soversion: 5,
  include_directories: inc,
  install: true,
)

install_headers(
  'include/stc/algorithm.h',
  'include/stc/arc.h',
  'include/stc/box.h',
  'include/stc/cbits.h',
  'include/stc/common.h',
  'include/stc/coption.h',
  'include/stc/coroutine.h',
  'include/stc/cregex.h',
  'include/stc/cspan.h',
  'include/stc/cstr.h',
  'include/stc/csview.h',
  'include/stc/deque.h',
  'include/stc/hmap.h',
  'include/stc/hset.h',
  'include/stc/list.h',
  'include/stc/pqueue.h',
  'include/stc/queue.h',
  'include/stc/random.h',
  'include/stc/smap.h',
  'include/stc/sort.h',
  'include/stc/sset.h',
  'include/stc/stack.h',
  'include/stc/types.h',
  'include/stc/utf8.h',
  'include/stc/vec.h',
  'include/stc/zsview.h',
  subdir: 'stc',
)

install_headers(
  'include/c11/fmt.h',
  subdir: 'c11',
)

stc_dep = declare_dependency(
  link_with: [stc_lib],
  include_directories: inc,
)

meson.override_dependency('stc', stc_dep)

flex_exe = find_program('flex', required: get_option('checkscoped'))
if flex_exe.found()
  lex_src = custom_target(
    'checkscoped_lex',
    command: [flex_exe, files('src/checkscoped.l')],
    output: 'lex.yy.c',
  )
  executable(
    'checkscoped',
    lex_src,
    install: true,
  )
endif

subdir('misc')
