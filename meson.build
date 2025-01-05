project(
  'stc',
  'c',
  version: '5.0-dev',
  license: 'MIT',
  default_options: ['c_std=c11', 'warning_level=3', 'default_library=static'],
  meson_version: '>=1.1',
)

stcversion = 5
cc = meson.get_compiler('c')

if cc.get_argument_syntax() == 'gcc'
  add_project_arguments(
    cc.get_supported_arguments(
      '-Wno-missing-field-initializers',
      '-Wno-stringop-overflow',
      '-Wno-clobbered',
    ),
    language: ['c', 'cpp'],
  )
elif cc.get_argument_syntax() == 'msvc'
  # no "unsecure" funcs warnings
  add_project_arguments(
    cc.get_supported_arguments(
      '-wd4996',
    ),
    language: ['c', 'cpp'],
  )
endif

libsrc = files(
  'src/cregex.c',
  'src/cspan.c',
  'src/cstr_core.c',
  'src/cstr_io.c',
  'src/cstr_utf8.c',
  'src/csview.c',
  'src/fmt.c',
  'src/random.c',
  'src/stc_core.c',
)

inc = include_directories('include')

if cc.get_id() == 'msvc'
  # only static library for MSVC atm.
  stc_lib = static_library(
    'stc',
    libsrc,
    name_prefix: '',
    name_suffix: 'lib',
    include_directories: inc,
    install: true,
  )
else
  m_dep = cc.find_library('m', required: false)

  stc_lib = library(
    'stc',
    libsrc,
    dependencies: [m_dep],
    soversion: stcversion,
    include_directories: inc,
    install: true,
  )
endif

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
  'include/stc/sys/crange.h',
  'include/stc/sys/filter.h',
  'include/stc/sys/sumtype.h',
  'include/stc/sys/utility.h',
  subdir: 'stc/sys',
)

install_headers(
  'include/stc/priv/cstr_prv.h',
  'include/stc/priv/linkage.h',
  'include/stc/priv/linkage2.h',
  'include/stc/priv/queue_prv.h',
  'include/stc/priv/sort_prv.h',
  'include/stc/priv/template.h',
  'include/stc/priv/template2.h',
  'include/stc/priv/utf8_prv.h',
  subdir: 'stc/priv',
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

root = not meson.is_subproject()
subdir('tests')
subdir('examples')

if root
  datadir = get_option('datadir')
  docdir = get_option('docdir')
  if docdir == ''
    docdir = datadir / 'doc' / meson.project_name()
  endif

  install_subdir(
    'docs',
    install_dir: docdir,
    strip_directory: true,
  )
endif
