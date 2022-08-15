cmake_minimum_required(VERSION 3.22)
project(PromisedFuture CXX)

find_package(GTest REQUIRED)

file(GLOB_RECURSE headers ../src/*.h ../test_package/*.h)
file(GLOB_RECURSE sources ../src/*.cpp ../test_package/*.cpp)

add_executable(${PROJECT_NAME}
	${headers}
	${sources}
)

target_include_directories(${PROJECT_NAME}
	PUBLIC ../src
	PUBLIC ../test_package
)

target_link_libraries(${PROJECT_NAME}
	PUBLIC GTest::gtest
)

set_target_properties(${PROJECT_NAME} PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
	RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/bin
	RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin
)
