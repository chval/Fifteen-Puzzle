cmake_minimum_required (VERSION 2.6)
project (15puzzle)

add_subdirectory (src)
include_directories("${PROJECT_SOURCE_DIR}/include")

add_executable(15puzzle main.cpp)
target_link_libraries (15puzzle optimized Board optimized ConsoleLogger optimized IDA_Star optimized Logger optimized Solver)
