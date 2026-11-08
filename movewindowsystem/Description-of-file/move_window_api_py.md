# Description: move_window_api.py

## Overview
Python binding library that dynamically loads `move_window.dll` using `ctypes`. Exposes high-level methods to move windows to the mouse location, resolve windows by title or process, query cursor coordinates, and retrieve bounding boxes.

## Exports & Functions
- `class MoveWindowNativeAPI`:
  - `__init__(dll_path=None)`: Resolves and loads the DLL, configuring ctypes argument and return signatures.
  - `move_window_to_cursor(hwnd, mode, offset_x, offset_y, clamp) -> int`: Moves window by handle.
  - `move_by_title(title, mode, offset_x, offset_y, clamp) -> int`: Finds window by title and moves it.
  - `move_by_process(process_name, mode, offset_x, offset_y, clamp) -> int`: Finds window by process name and moves it.
  - `move_active_window(mode, offset_x, offset_y, clamp) -> int`: Repositions the foreground window.
  - `find_by_title(title) -> Optional[int]`: Returns window handle.
  - `find_by_process(process_name) -> Optional[int]`: Returns window handle.
  - `get_foreground_window() -> Optional[int]`: Returns active window handle.
  - `get_cursor_position() -> Optional[Point2D]`: Returns current cursor position.
  - `get_window_bounds(hwnd) -> Optional[Rect2D]`: Returns window geometry.

## Dependencies & Data Flow
- **Imports**: `ctypes`, `logging`, `os`, `config.py`.
- **Invoked By**: `python/main.py`, `python/test_suite.py`.
- **Data Flow**: Marshals Python strings and integers across the C ABI boundary into `move_window.dll`.
