# Description: build.ps1

## Overview
PowerShell build script providing one-click configuration and compilation of the C shared library (`move_window.dll`) and C++ CLI executable (`move_window_cli.exe`) via CMake and MinGW.

## Exports & Functions
- Parameter `$BuildType`: Specifies CMake build type (`Release` or `Debug`).
- Configures CMake targeting `MinGW Makefiles`.
- Compiles targets and copies `move_window.dll` into the `python/` directory.

## Dependencies & Data Flow
- **Imports**: CMake, MinGW GCC/G++.
- **Invoked By**: Developer / User terminal (`.\build.ps1`).
- **Data Flow**: Invokes CMake toolchain, producing binaries in `build/` and syncing `python/move_window.dll`.
