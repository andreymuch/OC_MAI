cmake_minimum_required(VERSION 3.16)
project(OC)

# Устанавливаем стандарт языка C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Добавим Google Test как зависимость
include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG release-1.12.1  # Указываем конкретную версию
)

# Загружаем Google Test
FetchContent_MakeAvailable(googletest)

# Устанавливаем флаг для Google Test
enable_testing()


# Добавляем лабораторные как подпроекты
add_subdirectory(lab1)
add_subdirectory(lab2)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /utf-8")
