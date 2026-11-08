# Description: main.cpp

## Overview
Application entry point that initializes COM and GDI+ subsystems, loads PDF records, creates the full-desktop-sized click-through (uninteractable) overlay and floating control panel windows, manages the Win32 message pump, and coordinates graceful teardown.

## Exports & Functions
- `wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR, int nCmdShow) -> int`
  - **Description**: Standard Unicode Win32 GUI entry point initializing subsystems, creating the full-screen transparent click-through overlay window, registering global hotkeys, and running the event pump.
  - **Parameters**:
    - `hInstance`: Current module instance.
    - `nCmdShow`: Window display flag.
  - **Returns**: Application exit code.
- `main() -> int`
  - **Description**: Standard C++ entry point forwarding execution to `wWinMain`.
- `refreshDisplay() -> void`
  - **Description**: Recalculates layout centered on the full monitor, re-renders the layered GDI+ overlay window, and updates the control toolbar status.
- `reloadPdfFolder() -> void`
  - **Description**: Flushes thumbnail cache, triggers batch extraction of covers, rescans the `pdf/` directory, and triggers a complete refresh.

## Dependencies & Data Flow
- **Imports**: `config.h`, `scan_pdf_directory.h`, `extract_pdf_cover.h`, `calculate_grid_layout.h`, `render_overlay_canvas.h`, `create_control_panel.h`, `handle_user_input.h`, `<windows.h>`, `<gdiplus.h>`, `<vector>`, `<algorithm>`.
- **Invoked By**: Operating system loader on executable launch (`coverpdfbook.exe`).
- **Data Flow**: System launch -> Orchestrates scanning, full-screen layout math, control panel, and rendering passes -> Dispatches OS messages until exit.
