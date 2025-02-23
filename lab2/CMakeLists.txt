project(Lab2)

set(SRC_FILES 
    src/main.cpp
    src/filters.cpp
    src/matrix.cpp
    )

add_executable(lab2 ${SRC_FILES})
add_executable(lab2_test test/filters_test.cpp)
target_link_libraries(lab2)
target_link_libraries(lab2_test gtest gtest_main)

add_test(NAME Lab2Test COMMAND lab2_test)
