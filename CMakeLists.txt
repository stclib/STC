cmake_minimum_required(VERSION 3.15)

project(STC
  VERSION 6.0
  DESCRIPTION "A modern, user friendly, generic, type-safe and fast C99 container library."
  LANGUAGES C)

option(STC_BUILD_EXAMPLES "Build STC examples" ON)
option(STC_BUILD_TESTS "Build STC tests and enable CTest" ON)
option(STC_INSTALL_DOCS "Install docs directory" OFF)

include(GNUInstallDirs)

# Sources
set(STC_SOURCES
  src/cregex.c
  src/cspan.c
  src/cstr_core.c
  src/cstr_io.c
  src/cstr_utf8.c
  src/csview.c
  src/fmt.c
  src/random.c
  src/stc_core.c
)

add_library(stc ${STC_SOURCES})
add_library(stc::stc ALIAS stc)

target_include_directories(stc
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

target_compile_features(stc PUBLIC c_std_11)

# Link libm when available on non-MSVC platforms
if(NOT MSVC)
  include(CheckLibraryExists)
  check_library_exists(m cos "" HAVE_LIB_M)
  if(HAVE_LIB_M)
    target_link_libraries(stc PUBLIC m)
  endif()
endif()

set_target_properties(stc PROPERTIES
  OUTPUT_NAME stc
  C_VISIBILITY_PRESET hidden
  VISIBILITY_INLINES_HIDDEN YES)

# Install headers (entire include tree)
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

# Install library and export targets
install(TARGETS stc
  EXPORT stcTargets
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

# Package config generation
include(CMakePackageConfigHelpers)

write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/stcConfigVersion.cmake"
  VERSION ${PROJECT_VERSION}
  COMPATIBILITY AnyNewerVersion)

configure_package_config_file(
  "${CMAKE_CURRENT_LIST_DIR}/cmake/stcConfig.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/stcConfig.cmake"
  INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/stc)

install(EXPORT stcTargets
  FILE stcTargets.cmake
  NAMESPACE stc::
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/stc)

install(FILES
  "${CMAKE_CURRENT_BINARY_DIR}/stcConfig.cmake"
  "${CMAKE_CURRENT_BINARY_DIR}/stcConfigVersion.cmake"
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/stc)

# Optionally install docs
if(STC_INSTALL_DOCS)
  install(DIRECTORY docs/ DESTINATION ${CMAKE_INSTALL_DOCDIR} OPTIONAL)
endif()

# Examples
if(STC_BUILD_EXAMPLES)
  add_subdirectory(examples)
endif()

# Tests
if(STC_BUILD_TESTS)
  enable_testing()
  add_subdirectory(tests)
endif()
