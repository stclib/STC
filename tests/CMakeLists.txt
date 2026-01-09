# Build test executables and register CTest tests

include(GNUInstallDirs)

file(GLOB _TEST_SOURCES CONFIGURE_DEPENDS "*_test.c")

set(_COMMON_SRC main.c)

set(_TEST_TARGETS)

foreach(_suite_src IN LISTS _TEST_SOURCES)
  get_filename_component(_suite_name "${_suite_src}" NAME_WE)
  add_executable(${_suite_name} ${_suite_src} ${_COMMON_SRC})
  target_compile_definitions(${_suite_name} PRIVATE _GNU_SOURCE)
  target_link_libraries(${_suite_name} PRIVATE stc::stc)
  if(NOT MSVC)
    target_link_libraries(${_suite_name} PRIVATE m)
  endif()
  # Register test using the full path to the built executable to avoid PATH lookup issues
  add_test(NAME ${_suite_name} COMMAND $<TARGET_FILE:${_suite_name}>)
  list(APPEND _TEST_TARGETS ${_suite_name})
endforeach()

# Convenience aggregate target to build all tests in one go
if(_TEST_TARGETS)
  add_custom_target(tests ALL DEPENDS ${_TEST_TARGETS})
endif()

# Install ctest.h like Meson does
install(FILES ctest.h DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/stc)
