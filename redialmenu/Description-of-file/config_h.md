# Description: config.h

## Overview
Defines design constants, sizing metrics, radius thresholds, color palettes, and window class definitions for the C/C++ rendering and overlay engine.

## Exports & Constants
- `RADIAL_INNER_RADIUS_PX (45.0f)`: Radius of the center "Nothing" circle.
- `RADIAL_OUTER_RADIUS_PX (150.0f)`: Outer perimeter radius for the 4 quadrants.
- `RADIAL_WINDOW_SIZE_PX (340)`: Width and height of the layered square overlay window.
- `RADIAL_DRAG_THRESHOLD_PX (18.0f)`: Distance the mouse must move before gesture triggers.
- `COLOR_SLICE_DEFAULT_FILL`: Default background color of sectors.
- `COLOR_SLICE_HOVER_FILL`: Highlight fill color when sector is hovered.
- `COLOR_CENTER_DEFAULT_FILL`: Normal center circle fill.
- `COLOR_CENTER_HOVER_FILL`: Center circle fill when cancel zone is active.
- `COLOR_BORDER_LINE`: Border stroke color.
- `RADIAL_WINDOW_CLASS_NAME`: Win32 window class identifier.

## Dependencies & Data Flow
- **Imports**: `<stdint.h>`.
- **Invoked By**: `render_overlay.cpp`, `overlay_window.cpp`, `radial_controller.cpp`.
- **Data Flow**: Controls dimensions and colors passed to GDI+ drawing commands.
