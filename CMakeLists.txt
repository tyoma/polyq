cmake_minimum_required(VERSION 3.13)

project(pq)

enable_testing()

option(POLYQ_NO_TESTS "Do not build test modules." OFF)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/_lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/_bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/_bin)

add_compile_options("$<$<CXX_COMPILER_ID:GNU>:-fPIC;-Wpedantic;-Wall>")

add_library(polyq INTERFACE)
target_include_directories(polyq INTERFACE .)

if (NOT POLYQ_NO_TESTS)
	if (NOT TARGET utee)
		set(UTEE_NO_TESTS ON)
		set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/libraries/utee/make/build.props)
		add_subdirectory(libraries/utee)
	endif()
	add_subdirectory(tests)
endif()

