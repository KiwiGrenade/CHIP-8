cmake_minimum_required(VERSION 3.26)
project(CHIP-8 LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(-pthread)
add_link_options(-pthread)

find_package(Qt6 REQUIRED COMPONENTS Widgets Core)
qt_standard_project_setup()

add_subdirectory(src)
