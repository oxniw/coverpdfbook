# Description: window_finder.cpp

## Overview
Implementation file for the `WindowFinder` C++ class. Handles parsing of hexadecimal/decimal HWNDs, maps search queries to underlying C API functions, and validates handle liveness using `IsWindow`.

## Exports & Functions
- `WindowFinder::resolve`: Implements selector branching and returns `std::optional<HWND>`.
- `WindowFinder::get_window_title`: Calls Win32 `GetWindowTextA` and wraps result into `std::string`.

## Dependencies & Data Flow
- **Imports**: `window_finder.hpp`, `<sstream>`.
- **Invoked By**: `main.cpp`.
- **Data Flow**: Connects higher-level C++ CLI parameters to the C shared library query primitives.
