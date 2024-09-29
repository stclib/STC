project(
  'stc',
  'c',
  version: '5.0-dev',
  license: 'MIT',
  default_options: ['c_std=c11', 'warning_level=2', 'default_library=both']
)

cc = meson.get_compiler('c')

if cc.get_id() == 'gcc' or cc.get_id() == 'clang'
  add_project_arguments('-Wno-missing-field-initializers', '-Wno-clobbered', language: 'c')
elif cc.get_id() == 'msvc'
  add_project_arguments('-D_CRT_SECURE_NO_WARNINGS', language: 'c')
endif

m_dep = cc.find_library('m', required: false)

inc = include_directories('include')
stc_lib = library(
  'stc',
  files(
    'src/cregex.c',
    'src/cspan.c',
    'src/cstr_core.c',
    'src/cstr_io.c',
    'src/cstr_u8.c',
    'src/csview.c',
    'src/fmt.c',
    'src/random.c',
  ),
  dependencies: [m_dep],
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
    command: [flex_exe, '-o', '@OUTPUT@', files('src/checkscoped.l')],
    output: 'lex.yy.c',
  )
  checkscoped_exe = executable(
    'checkscoped',
    lex_src,
    native: true,
  )
  meson.override_find_program('checkscoped', checkscoped_exe)
endif

subdir('misc')

datadir = get_option('datadir')

install_subdir(
  'docs',
  install_dir: datadir / 'doc' / meson.project_name(),
  strip_directory: true,
)
