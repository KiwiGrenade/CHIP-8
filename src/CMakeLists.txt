add_executable(main main.cpp)
add_library(chip8 SHARED chip8.cpp display.cpp memory.cpp)
target_link_libraries(chip8 PUBLIC sfml-graphics)
target_link_libraries(main PUBLIC chip8)
