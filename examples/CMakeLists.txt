# Build all example programs found in the listed subdirectories

set(_EXAMPLE_DIRS
  algorithms
  bitsets
  coroutines
  hashmaps
  linkedlists
  mixed
  priorityqueues
  queues
  regularexpressions
  smartpointers
  sortedmaps
  spans
  strings
  vectors)

foreach(_dir IN LISTS _EXAMPLE_DIRS)
  if(EXISTS "${CMAKE_CURRENT_LIST_DIR}/${_dir}")
    file(GLOB _c_files CONFIGURE_DEPENDS "${_dir}/*.c")
    foreach(_src IN LISTS _c_files)
      get_filename_component(_name_we "${_src}" NAME_WE)
      set(_exe "ex_${_dir}_${_name_we}")
      add_executable(${_exe} "${_src}")
      target_link_libraries(${_exe} PRIVATE stc::stc)
      # Link libm if stc didn't already (safe to repeat)
      if(NOT MSVC)
        target_link_libraries(${_exe} PRIVATE m)
      endif()
    endforeach()
  endif()
endforeach()
