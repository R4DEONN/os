cmake_minimum_required(VERSION 3.22)
project(MainApp)

set(CMAKE_CXX_STANDARD 20)
set(EXECUTABLE_OUTPUT_PATH "bin")

add_executable(MainApp main.cpp)

find_package(X11 REQUIRED)

if (X11_FOUND)
    include_directories(${X11_INCLUDE_DIR})
    message(STATUS "X11_INCLUDE_DIR: ${X11_INCLUDE_DIR}")
    message(STATUS "X11_LIBRARIES: ${X11_LIBRARIES}")
else ()
    message(FATAL_ERROR "X11 library not found")
endif ()

target_link_libraries(MainApp ${X11_LIBRARIES})