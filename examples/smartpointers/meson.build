foreach sample : [
  'arc_containers',
  'arc_demo',
  'arcvec_erase',
  'box2',
  'box',
  'map_box',
  'map_ptr',
  'music_arc',
  'new_sptr',
  'person_arc',
]
  test(
    sample,
    executable(
      sample,
      files(f'@sample@.c'),
      dependencies: example_deps, install: false,
    ),
    suite: 'arc',
  )
endforeach
