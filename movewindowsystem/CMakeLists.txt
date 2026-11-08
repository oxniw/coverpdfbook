cmake_minimum_required(VERSION 3.15)
project(MoveWindowSystem C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

# Compile C Shared Dynamic Library (move_window.dll)
add_library(move_window SHARED
    src/geometry_calc.c
    src/win32_window_adapter.c
    src/move_window_api.c
)

target_include_directories(move_window PUBLIC include)
target_link_libraries(move_window user32 psapi)
set_target_properties(move_window PROPERTIES PREFIX "")

# Compile C++ CLI Executable (move_window_cli.exe)
add_executable(move_window_cli
    src/main.cpp
    src/window_finder.cpp
    src/hotkey_listener.cpp
)

target_include_directories(move_window_cli PRIVATE include)
target_link_libraries(move_window_cli move_window user32 psapi)

# Post-build copy to python directory for immediate binding tests
add_custom_command(TARGET move_window POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    $<TARGET_FILE:move_window>
    ${CMAKE_CURRENT_SOURCE_DIR}/python/move_window.dll
)
