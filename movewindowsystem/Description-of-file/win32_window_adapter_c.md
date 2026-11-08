# Description: win32_window_adapter.c

## Overview
Implements OS interaction routines with the Windows Win32 subsystem (`user32.dll`), translating native structures (`POINT`, `RECT`, `MONITORINFO`) into platform-agnostic domain representations (`Point2D`, `Rect2D`).

## Exports & Functions
- `adapter_get_cursor_position`: Uses Win32 `GetCursorPos`.
- `adapter_get_window_geometry`: Uses Win32 `GetWindowRect`.
- `adapter_get_monitor_geometry`: Uses Win32 `MonitorFromPoint` and `GetMonitorInfoW`.
- `adapter_set_window_position`: Calls `SetWindowPos`.
- `adapter_is_window_valid`: Calls `IsWindow` and `IsWindowVisible`.

## Dependencies & Data Flow
- **Imports**: `win32_window_adapter.h`.
- **Invoked By**: `move_window_api.c`.
- **Data Flow**: Connects directly to Windows Kernel and User32 subsystem.
