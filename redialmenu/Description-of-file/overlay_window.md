# Description: overlay_window.cpp / overlay_window.h

## Overview
Win32 layered window management module (`WS_EX_LAYERED`). Creates a click-through, topmost floating popup window that presents the rendered radial menu without stealing user focus from active background applications.

## Exports & Functions
- `overlay_window_create(win: OverlayWindow*, width: int, height: int) -> int`
  - **Description**: Registers window class, creates layered popup window, and allocates a 32-bit ARGB DIB Section.
  - **Returns**: `1` on success, `0` on failure.
- `overlay_window_show_at(win: OverlayWindow*, center_x: int, center_y: int) -> void`
  - **Description**: Centers the window around `(center_x, center_y)` using `SWP_NOACTIVATE` and triggers initial presentation.
- `overlay_window_hide(win: OverlayWindow*) -> void`
  - **Description**: Hides overlay window (`ShowWindow(..., SW_HIDE)`).
- `overlay_window_update_animated(win: OverlayWindow*, scale: float, alpha: float, hover_weights: const float*) -> void`
  - **Description**: Re-renders layered window with animated scale, master alpha, and smooth per-sector hover blend weights.
- `overlay_window_update_sector(win: OverlayWindow*, sector: RadialSector) -> void`
  - **Description**: Redraws menu with the updated sector highlight and updates alpha layer via `UpdateLayeredWindow`.
- `overlay_window_destroy(win: OverlayWindow*) -> void`
  - **Description**: Cleans up DIB section, memory DC, and destroys HWND.

## Dependencies & Data Flow
- **Imports**: `<windows.h>`, `radial_types.h`, `config.h`, `render_overlay.h`.
- **Invoked By**: `radial_controller.cpp`.
- **Data Flow**: Controller position commands -> window positioning -> `render_draw_menu` -> `UpdateLayeredWindow` to screen.
