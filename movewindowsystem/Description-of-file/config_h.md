# Description: config.h

## Overview
Shared C/C++ configuration header defining standard exit/status codes, coordinate structures (`Rect2D`, `Point2D`), default hotkey bindings, and alignment mode enumerations (`AlignmentMode`) used across the move-window system.

## Exports & Functions
- `AlignmentMode`: Enumeration specifying window anchor relative to cursor:
  - `ALIGN_CENTER`: Snaps window center to mouse.
  - `ALIGN_TOP_LEFT`: Snaps window top-left corner to mouse.
  - `ALIGN_CUSTOM_OFFSET`: Offsets window top-left by specified delta X and delta Y.
- `Rect2D`: Bounding box structure containing `x`, `y`, `width`, `height`.
- `Point2D`: 2D point structure containing `x`, `y`.
- Error Codes:
  - `MOVE_WINDOW_SUCCESS` (0)
  - `MOVE_WINDOW_ERR_INVALID_ARG` (-1)
  - `MOVE_WINDOW_ERR_NOT_FOUND` (-2)
  - `MOVE_WINDOW_ERR_WIN32_CALL` (-3)
  - `MOVE_WINDOW_ERR_HOTKEY_FAIL` (-4)

## Dependencies & Data Flow
- **Imports**: Standard C language constructs.
- **Invoked By**: `geometry_calc.h`, `win32_window_adapter.h`, `move_window_api.h`, `main.cpp`.
- **Data Flow**: Supplies type definitions and shared constants consumed during coordinate computation, Win32 API calls, and Python interop.
