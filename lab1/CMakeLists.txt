# Устанавливаем имя проекта для lab1
project(Lab1)

# Добавляем исполняемый файл для lab1
add_executable(parent src/parent.cpp)
add_executable(child1 src/child1.cpp)
add_executable(child2 src/child2.cpp)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /utf-8")
set(sourceDir ${CMAKE_SOURCE_DIR})