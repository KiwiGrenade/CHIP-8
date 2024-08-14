qt_add_executable(emu mainwindow.cpp mainwindow.ui main.cpp)
qt_add_library(chip8 SHARED chip8.cpp display.cpp memory.cpp)
target_link_libraries(chip8 PUBLIC Qt6::Widgets)
target_link_libraries(emu PUBLIC chip8)
