cmake_minimum_required(VERSION 3.12)

project(stclib)
add_library(stcinc INTERFACE)
#target_include_directories(stcinc INTERFACE include)

file(GLOB sources "src/*.c")
add_library(stc STATIC ${sources})

if (UNIX)
	find_package(FLEX)
	flex_target(checkscoped src/checkscoped.l ${CMAKE_CURRENT_BINARY_DIR}/lex.yy.c)
	add_executable(checkscoped ${FLEX_checkscoped_OUTPUTS})
endif()


include(CTest)

if (BUILD_TESTING)
	file(GLOB examples misc/examples/*.c)
	foreach (file IN LISTS examples)
		get_filename_component(name "${file}" NAME_WE)
		add_executable(${name} ${file})
		target_compile_options(${name} PRIVATE "-DSTC_STATIC")
		if (CMAKE_THREAD_LIBS_INIT)
			target_link_libraries(${name} PRIVATE "${CMAKE_THREAD_LIBS_INIT}")
		endif()
		target_link_libraries(${name} PRIVATE stc m)
		add_test(NAME ${name} COMMAND ${name})
	endforeach()

	file(GLOB test_files misc/tests/*_test.c)
	add_executable(stctest ${test_files} misc/tests/main.c)
	target_link_libraries(stctest PRIVATE stc m)
	add_test(NAME stctest COMMAND stctest)

	# foreach (name IN ITEMS deque list hmap smap vec)
	#	add_executable(${name} misc/benchmarks/plotbench/${name}_benchmark.cpp)
	#	target_link_libraries(${name} PRIVATE stc m)
	#	add_test(NAME ${name} COMMAND ${name})
	# endforeach()
endif()

