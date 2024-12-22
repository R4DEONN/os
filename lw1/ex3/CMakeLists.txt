project(ex)

set(CMAKE_CXX_STANDARD 20)

# TODO: прочитать документацию CMake
file(GLOB_RECURSE SRC "src/*.cpp" "src/*.h")
add_executable(app ${SRC})