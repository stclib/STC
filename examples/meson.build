examples = get_option('examples')

if examples.allowed()
  example_deps = [
    stc_dep,
    cc.find_library('m', required: false),
  ]
  subdir('algorithms')
  subdir('bitsets')
  subdir('coroutines')
  subdir('hashmaps')
  subdir('linkedlists')
  subdir('mixed')
  subdir('priorityqueues')
  subdir('queues')
  subdir('regularexpressions')
  subdir('smartpointers')
  subdir('sortedmaps')
  subdir('spans')
  subdir('strings')
  subdir('vectors')
endif
