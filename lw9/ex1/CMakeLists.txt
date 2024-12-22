cmake_minimum_required(VERSION 3.22)
project(MainApp)

include(FetchContent)

FetchContent_Declare(
        boost
        URL https://dl.bintray.com/boostorg/release/1.74.0/source/boost_1_74_0.tar.bz2
)

FetchContent_GetProperties(boost)

if(NOT boost_POPULATED)
    FetchContent_Populate(boost)
    set(BOOST_ROOT ${boost_SOURCE_DIR})
endif()

find_package(Boost 1.74.0 REQUIRED COMPONENTS system)

set(CMAKE_CXX_STANDARD 20)
set(EXECUTABLE_OUTPUT_PATH "bin")

add_executable(MainApp main.cpp)

target_link_libraries(${PROJECT_NAME} Boost::system)