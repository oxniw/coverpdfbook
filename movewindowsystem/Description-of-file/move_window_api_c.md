# Description: move_window_api.c

## Overview
Implements exported C ABI functions in `move_window.dll`. Handles window lookup by title or process image via `EnumWindows`, connects to geometry math for alignment and clamping, and applies updates via the Win32 adapter.

## Exports & Functions
- `case_insensitive_contains`: Helper performing case-insensitive substring search.
- `enum_windows_title_proc`: Win32 callback enumerating windows matching title substring.
- `enum_windows_process_proc`: Win32 callback matching process image name.
- `move_window_to_cursor`: Orchestrates cursor position retrieval, window geometry calculation, screen clamping, and placement.
- `move_window_by_title`: Resolves window title and repositions window.
- `move_window_by_process_name`: Resolves process name and repositions window.
- `move_active_window_to_cursor`: Repositions active foreground window.
- `find_window_by_title_substring`: Returns matching `HWND`.
- `find_window_by_process_name`: Returns matching `HWND`.
- `get_foreground_window_handle`: Returns active `HWND`.
- `get_cursor_coordinates`: Returns cursor point.
- `get_window_bounds`: Returns window rect.

## Dependencies & Data Flow
- **Imports**: `move_window_api.h`, `geometry_calc.h`, `win32_window_adapter.h`, `<psapi.h>`, `<string.h>`, `<ctype.h>`.
- **Invoked By**: C/C++ applications and Python ctypes interface.
- **Data Flow**: Bridges client selector requests to the Win32 subsystem.
