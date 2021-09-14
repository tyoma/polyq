cmake_minimum_required(VERSION 2.8)

project(pq)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/_lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/_bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/_bin)
set(UTEE_NO_TESTS)

include_directories(.)

set(GCC_CLANG_COMPILER_OPTIONS -fPIC -Wpedantic -Wall)

add_compile_options(
	"$<$<CXX_COMPILER_ID:GNU>:$<$<COMPILE_LANGUAGE:C>:${GCC_CLANG_COMPILER_OPTIONS}>>"
	"$<$<CXX_COMPILER_ID:GNU>:$<$<COMPILE_LANGUAGE:CXX>:${GCC_CLANG_COMPILER_OPTIONS}>>"
)

add_definitions(-D_SCL_SECURE_NO_WARNINGS -D_CRT_SECURE_NO_WARNINGS)

add_subdirectory(tests)
if (NOT TARGET utee)
	add_subdirectory(libraries/utee)
endif()
