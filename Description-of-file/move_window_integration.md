# Description: move_window_integration

## Overview
Integration module connecting the `movewindowsystem` library to the PDF bookshelf application. Repositions target windows (such as the Settings Control Panel) directly under the user's current mouse cursor position with automatic multi-monitor boundary clamping.

## Exports & Functions
- `moveWindowToCursor(HWND hwnd) -> bool`
  - **Description**: Invokes `move_window_to_cursor` from `move_window.dll` using center alignment (`ALIGN_CENTER`) and screen clamping enabled (`clamp_to_screen = 1`), positioning the target window around the active mouse cursor.
  - **Parameters**:
    - `hwnd`: Handle to the target window (e.g., Settings Control Panel).
  - **Returns**: `true` on successful repositioning, `false` otherwise.

## Dependencies & Data Flow
- **Imports**: `<windows.h>`, `move_window.dll` (`move_window_to_cursor`).
- **Invoked By**: Called from `main.cpp` when Sector 3 ("Settings") is triggered from the radial gesture menu.
- **Data Flow**: Radial menu sector 3 confirmation -> `main.cpp` receives command -> `moveWindowToCursor(g_hwndPanel)` positions settings panel at current cursor location.
