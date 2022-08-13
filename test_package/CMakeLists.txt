cmake_minimum_required(VERSION 3.22)
project(PromisedFuture_Test CXX)

find_package(PromisedFuture REQUIRED)
find_package(GTest REQUIRED)

add_executable(${PROJECT_NAME}
	main.cpp
)

target_link_libraries(${PROJECT_NAME}
	PUBLIC PromisedFuture::PromisedFuture
	PUBLIC GTest::gtest
)

set_target_properties(${PROJECT_NAME} PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
	RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/bin
	RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin
)
