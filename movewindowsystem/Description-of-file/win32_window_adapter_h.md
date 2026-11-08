# Description: win32_window_adapter.h

## Overview
Declares low-level Win32 adapter functions wrapping Windows `user32.dll` APIs (`GetCursorPos`, `GetWindowRect`, `MonitorFromPoint`, `SetWindowPos`) behind domain-friendly data structures (`Point2D`, `Rect2D`).

## Exports & Functions
- `int adapter_get_cursor_position(Point2D* out_pos)`
  - **Description**: Fetches current mouse cursor coordinates.
- `int adapter_get_window_geometry(HWND hwnd, Rect2D* out_geometry)`
  - **Description**: Retrieves coordinates and dimensions of a target window.
- `int adapter_get_monitor_geometry(Point2D pt, Rect2D* out_monitor_bounds)`
  - **Description**: Queries the work area bounds of the monitor containing the given point.
- `int adapter_set_window_position(HWND hwnd, Point2D pos)`
  - **Description**: Sets window position using `SetWindowPos` with `SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE`.
- `int adapter_is_window_valid(HWND hwnd)`
  - **Description**: Checks whether the handle represents a valid and visible window.

## Dependencies & Data Flow
- **Imports**: `config.h`, `<windows.h>`.
- **Invoked By**: `move_window_api.c`, `window_finder.cpp`.
- **Data Flow**: Acts as the boundary seam between the Windows OS and domain logic.
