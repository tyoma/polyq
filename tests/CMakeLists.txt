include_directories(../libraries/utee)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(PQ_TEST_SOURCES
	CircularBufferTests.cpp
	PolyEntryTests.cpp
	QueueTests.cpp
)

add_library(poly-queue.tests SHARED ${PQ_TEST_SOURCES})

target_link_libraries(poly-queue.tests utee)
