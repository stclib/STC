examples = get_option('examples').enable_auto_if(root)

if examples.enabled()
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
