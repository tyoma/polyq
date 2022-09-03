include(utee-test)

add_compile_options("$<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-std=c++11>")

set(PQ_TEST_SOURCES
	CircularBufferTests.cpp
	PolyEntryTests.cpp
	QueueTests.cpp
)

add_library(poly-queue.tests SHARED ${PQ_TEST_SOURCES})
target_link_libraries(poly-queue.tests polyq)
add_utee_test(poly-queue.tests)
