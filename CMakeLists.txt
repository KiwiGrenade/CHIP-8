cmake_minimum_required(VERSION 3.26)
project(CHIP-8 LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

Include(FetchContent)

# FetchContent_Declare(
#         Catch2
#         GIT_REPOSITORY https://github.com/catchorg/Catch2.git
#         GIT_TAG        v3.4.0 # or a later release
# )
FetchContent_Declare(
        SFML
        GIT_REPOSITORY https://github.com/SFML/SFML.git
        GIT_TAG        2.6.x
)

FetchContent_MakeAvailable(SFML)

set(CMAKE_CXX_STANDARD 23)

include(CTest)
add_subdirectory(src)
