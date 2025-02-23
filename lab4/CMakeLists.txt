 project(Lab4)

 add_library(lab4_static src/func1.cpp)

 add_executable(lab4_main1 src/main1.cpp)
 target_link_libraries(lab4_main1 PRIVATE lab4_static)

 # Создаем динамическую библиотеку из func2.cpp
 add_library(lab4_dynamic SHARED src/func2.cpp)
 target_compile_definitions(lab4_dynamic PRIVATE BUILD_DLL)

 # Создаем исполняемый файл main2 и связываем его с динамической библиотекой
 add_executable(lab4_main2 src/main2.cpp)
 target_link_libraries(lab4_main2 PRIVATE lab4_dynamic)
 enable_testing()
 add_test(NAME TestLab4 COMMAND lab4_main2.exe 1 3.14 1e-5)

 set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /utf-8")
 set(sourceDir ${CMAKE_SOURCE_DIR})
