# Description: move_window_api.h

## Overview
Declares the public C ABI dynamic library interface for `move_window.dll`. It provides functions to search, query, and move arbitrary windows to the mouse cursor position using multiple selector strategies.

## Exports & Functions
- `move_window_to_cursor(HWND hwnd, AlignmentMode mode, int offset_x, int offset_y, int clamp_to_screen)`: Moves window by handle to cursor.
- `move_window_by_title(const char* title_substring, AlignmentMode mode, int offset_x, int offset_y, int clamp_to_screen)`: Finds window by title substring and moves it.
- `move_window_by_process_name(const char* process_name, AlignmentMode mode, int offset_x, int offset_y, int clamp_to_screen)`: Finds window by executable name and moves it.
- `move_active_window_to_cursor(AlignmentMode mode, int offset_x, int offset_y, int clamp_to_screen)`: Moves active foreground window to cursor.
- `find_window_by_title_substring(const char* title_substring)`: Returns `HWND` of window matching title.
- `find_window_by_process_name(const char* process_name)`: Returns `HWND` of window matching process name.
- `get_foreground_window_handle()`: Returns `HWND` of the foreground window.
- `get_cursor_coordinates(int* out_x, int* out_y)`: Retrieves mouse coordinates.
- `get_window_bounds(HWND hwnd, int* out_x, int* out_y, int* out_w, int* out_h)`: Retrieves window bounds.

## Dependencies & Data Flow
- **Imports**: `config.h`, `<windows.h>`.
- **Invoked By**: `move_window_api.c`, C++ client applications (`main.cpp`), and Python ctypes bindings (`move_window_api.py`).
- **Data Flow**: Core API entry point linking user commands to the Win32 adapter and domain math.
