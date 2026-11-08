# Description: render_overlay.cpp / render_overlay.h

## Overview
GDI+ graphics renderer responsible for drawing the high-DPI, anti-aliased donut radial wheel, quadrant borders, text labels, and hover highlights into a 32-bit ARGB memory Device Context (DC).

## Exports & Functions
- `render_init_gdiplus() -> int`
  - **Description**: Initializes the Windows GDI+ graphics subsystem.
  - **Returns**: `1` on success, `0` on error.
- `render_shutdown_gdiplus() -> void`
  - **Description**: Shuts down GDI+ and releases associated tokens.
- `render_draw_menu(hdcTarget: HDC, width: int, height: int, scale: float, hover_weights: const float*) -> void`
  - **Description**: Renders transparent background, outer pie slices, center circle, divider lines, and text labels ("Nothing", "function1" through "function4") with geometric scaling and smooth hover color transitions.
  - **Parameters**:
    - `hdcTarget`: 32-bit ARGB memory DC handle.
    - `width`, `height`: Pixel dimensions of target surface.
    - `scale`: Current geometric scale multiplier (e.g. 0.45 to 1.0).
    - `hover_weights`: Array of 5 float weights [0.0, 1.0] for smooth per-sector color blending.

## Dependencies & Data Flow
- **Imports**: `<windows.h>`, `<gdiplus.h>`, `radial_types.h`, `config.h`.
- **Invoked By**: `overlay_window.cpp`.
- **Data Flow**: Target memory DC + active sector -> GDI+ vector rendering -> 32-bit ARGB raster bitmap.
