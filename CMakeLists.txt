cmake_minimum_required(VERSION 3.16)
project(OC)

# Устанавливаем стандарт языка C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG release-1.12.1  
)

set(CPPZMQ_BUILD_TESTS OFF)

FetchContent_Declare(
    ZeroMQ
    GIT_REPOSITORY https://github.com/zeromq/libzmq.git
    GIT_TAG v4.3.5
)

FetchContent_Declare(
    cppzmq
    GIT_REPOSITORY https://github.com/zeromq/cppzmq.git
    GIT_TAG v4.10.0
    SOURCE_DIR ${CMAKE_CURRENT_BINARY_DIR}/cppzmq
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Wall /Zi /fsanitize=address")  
FetchContent_MakeAvailable(googletest ZeroMQ cppzmq)
enable_testing()

add_subdirectory(lab1)
add_subdirectory(lab2)
add_subdirectory(lab3)
add_subdirectory(lab4)
add_subdirectory(lab5-7)
add_subdirectory(CourseProject)

