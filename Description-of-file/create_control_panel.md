# Description: create_control_panel

## Overview
Win32 floating modeless control panel window that provides interactive buttons and real-time status readouts for controlling the $M \times N$ grid rows/columns, cover card dimensions, transparency level, grid alignment anchor, pagination, click-through mode, hiding/showing the PDF covers overlay, and closing the program.

## Exports & Functions
- `createControlPanelWindow(HINSTANCE hInstance, HWND hwndOverlay) -> HWND`
  - **Description**: Registers the window class, creates child controls including the "Hide PDF Covers (V)" / "Show PDF Covers (V)" toggle button and the "Close Program (Esc)" button, and routes commands to the main window.
  - **Parameters**:
    - `hInstance`: Application instance handle.
    - `hwndOverlay`: Handle to the main overlay window for message routing.
  - **Returns**: Handle to the newly instantiated control panel window.
- `updateControlPanelUI(HWND hwndPanel, const AppSettings& settings, int totalItems, int totalPages) -> void`
  - **Description**: Synchronizes the text labels on the control toolbar with the current live settings (current rows, columns, pixel size, percentage opacity, active alignment mode, active page numbers, click-through status, and whether PDF covers are shown or hidden).
  - **Parameters**:
    - `hwndPanel`: Handle to the control panel window.
    - `settings`: Current application settings.
    - `totalItems`: Total number of loaded PDFs.
    - `totalPages`: Total number of pages calculated.
  - **Returns**: None.

## Dependencies & Data Flow
- **Imports**: `create_control_panel.h`, `config.h`, `<windows.h>`, `<sstream>`.
- **Invoked By**: Instantiated by `main.cpp` on startup; updated during each display refresh.
- **Data Flow**: User clicks "Hide PDF Covers (V)" -> `ShowWindow(hwndOverlay, SW_HIDE)`. User clicks "Close Program (Esc)" -> `PostQuitMessage(0)`.
