cmake_minimum_required(VERSION 3.13)

set(PQ_TEST_SOURCES
	CircularBufferTests.cpp
	PolyEntryTests.cpp
	QueueTests.cpp
)

add_library(poly-queue.tests SHARED ${PQ_TEST_SOURCES})
target_link_libraries(poly-queue.tests polyq)
add_utee_test(poly-queue.tests)
