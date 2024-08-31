qt_add_executable(emulator main.cpp)

qt_add_library(screen STATIC Screen.cpp)
qt_add_library(memory STATIC Memory.cpp)
qt_add_library(chip8 STATIC Chip8.cpp)
qt_add_library(widget STATIC EmulationScreenWidget.cpp)
qt_add_library(main_window STATIC MainWindow.cpp MainWindow.ui)

target_compile_options(memory PRIVATE -pthread)

target_link_libraries(memory PRIVATE Qt6::Core)
target_link_libraries(screen PRIVATE Qt6::Core)
target_link_libraries(chip8 PRIVATE Qt6::Core screen memory)
target_link_libraries(widget PRIVATE Qt6::Widgets)
target_link_libraries(main_window PUBLIC Qt::Core Qt::Widgets chip8 widget)
target_link_libraries(emulator PRIVATE main_window)

target_include_directories(main_window PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
