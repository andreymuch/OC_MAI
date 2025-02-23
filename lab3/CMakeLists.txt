
project(Lab3)

add_executable(parent_lab3 src/parent.cpp)
add_executable(child1_lab3 src/child1.cpp)
add_executable(child2_lab3 src/child2.cpp)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /utf-8")
set(sourceDir ${CMAKE_SOURCE_DIR})