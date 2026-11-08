# Description: CMakeLists.txt

## Overview
Root CMake configuration file defining build targets for the C dynamic library (`move_window.dll`) and the C++ CLI application (`move_window_cli.exe`).

## Exports & Functions
- Target `move_window` (SHARED library):
  - Sources: `src/geometry_calc.c`, `src/win32_window_adapter.c`, `src/move_window_api.c`.
  - Links: `user32`, `psapi`.
- Target `move_window_cli` (Executable):
  - Sources: `src/main.cpp`, `src/window_finder.cpp`, `src/hotkey_listener.cpp`.
  - Links: `move_window`, `user32`, `psapi`.
- Post-build step: Copies `move_window.dll` into `python/`.

## Dependencies & Data Flow
- **Imports**: CMake 3.15+.
- **Invoked By**: CMake build generator.
- **Data Flow**: Directs compiler and linker flags across C11 and C++17 components.
