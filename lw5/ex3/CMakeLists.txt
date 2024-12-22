cmake_minimum_required(VERSION 3.22)
project(MainApp)

enable_testing()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

set(CMAKE_CXX_STANDARD 20)
set(EXECUTABLE_OUTPUT_PATH "bin")

add_subdirectory(src)

add_executable(MainApp main.cpp src/ThreadPool.h)

add_subdirectory(test)

add_dependencies(MainApp run_tests)