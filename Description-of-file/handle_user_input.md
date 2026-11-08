# Description: handle_user_input

## Overview
Coordinates user input handling across mouse clicks, window drag repositioning, hover state tracking, keyboard hotkey shortcuts, and default PDF application launching.

## Exports & Functions
- `hitTestGridItem(int x, int y, const GridLayoutResult& layout, const std::vector<RECT>& itemRects) -> int`
  - **Description**: Determines which PDF item index in the master collection contains the specified client coordinates. Returns `-1` if no card is hit.
- `handleOverlayMouseDown(HWND hwndOverlay, int x, int y, AppSettings& settings, const GridLayoutResult& layout, const std::vector<RECT>& itemRects) -> void`
  - **Description**: Initiates window dragging if background space is clicked.
- `handleOverlayMouseMove(HWND hwndOverlay, int x, int y, AppSettings& settings, const std::vector<PdfItem>& items, const GridLayoutResult& layout, const std::vector<RECT>& itemRects) -> bool`
  - **Description**: Translates window position when dragging is active or detects cover card hover changes to trigger dynamic glowing highlights. Returns `true` when a redraw is required.
- `handleOverlayMouseUp(HWND hwndOverlay, AppSettings& settings) -> void`
  - **Description**: Releases mouse capture and terminates window drag state.
- `handleOverlayDoubleClick(int x, int y, const std::vector<PdfItem>& items, const GridLayoutResult& layout, const std::vector<RECT>& itemRects, std::wstring& outNewFolderPath) -> bool`
  - **Description**: Detects double-clicks over a book cover or folder card. If a folder or parent entry (`..`) is double-clicked, populates `outNewFolderPath` to change directory. If a PDF document is clicked, launches it with `openPdfFile`.
- `handleAppKeyCommand(WPARAM wParam, AppSettings& settings, int totalItems, int totalPages, HWND hwndOverlay, HWND hwndPanel) -> bool`
  - **Description**: Processes keyboard shortcuts (`+`/`-` for size, `[`/`]` for columns, Up/Down for rows, Left/Right for pagination, `T` for click-through, `H` for panel, `Esc` to quit). Returns `true` if state changed.
- `toggleClickThrough(HWND hwndOverlay, AppSettings& settings) -> void`
  - **Description**: Dynamically toggles the `WS_EX_TRANSPARENT` extended window style on the overlay window.
- `openPdfFile(const std::wstring& filePath) -> void`
  - **Description**: Uses `ShellExecuteW` to launch the PDF in Microsoft Edge, Acrobat, or whatever default reader the user has configured.

## Dependencies & Data Flow
- **Imports**: `handle_user_input.h`, `config.h`, `<windows.h>`, `<shellapi.h>`, `<algorithm>`.
- **Invoked By**: Called from `main.cpp` (`OverlayWndProc`) during `WM_LBUTTONDOWN`, `WM_MOUSEMOVE`, `WM_LBUTTONUP`, `WM_LBUTTONDBLCLK`, `WM_KEYDOWN`, and hotkeys.
- **Data Flow**: Hardware input -> State updates in `AppSettings` -> Triggering `refreshDisplay()` and OS shell actions.
