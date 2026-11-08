# Description: math_geometry.c / math_geometry.h

## Overview
Pure C domain mathematics module containing Euclidean distance, angular calculations, and quadrant hit-testing logic without any operating system or GUI dependencies.

## Exports & Functions
- `math_distance(x1: double, y1: double, x2: double, y2: double) -> double`
  - **Description**: Calculates Euclidean distance between two points.
  - **Returns**: $\sqrt{(x_2 - x_1)^2 + (y_2 - y_1)^2}$.
- `math_angle_deg(x0: double, y0: double, x: double, y: double) -> double`
  - **Description**: Computes angle in degrees $[0.0, 360.0)$ from origin $(x_0, y_0)$ to point $(x, y)$.
  - **Returns**: Clockwise angular position where $0^\circ$ is positive X axis.
- `math_resolve_sector_n(origin_x: double, origin_y: double, current_x: double, current_y: double, inner_radius: double, sector_count: int) -> GeometryResult`
  - **Description**: Dynamically calculates which of the N outer sectors (1 to N) or center circle (0) the cursor lies in.
  - **Returns**: `GeometryResult` with distance, angle, and resolved sector (0=Nothing, 1..N).
- `math_resolve_sector(origin_x: double, origin_y: double, current_x: double, current_y: double, inner_radius: double) -> GeometryResult`
  - **Description**: Evaluates whether point is inside center dead zone or one of the 4 outer sectors.
  - **Parameters**:
    - `origin_x`, `origin_y`: Anchor center where right-click began.
    - `current_x`, `current_y`: Current cursor position.
    - `inner_radius`: Radius of the center "Nothing" circle.
  - **Returns**: `GeometryResult` containing distance, angle, and resolved `RadialSector`:
    - `RADIAL_SECTOR_NOTHING (0)`: If distance $\le$ inner_radius.
    - `RADIAL_SECTOR_FUNCTION1 (1)`: Top-Right ($dx \ge 0, dy < 0$).
    - `RADIAL_SECTOR_FUNCTION2 (2)`: Bottom-Right ($dx \ge 0, dy \ge 0$).
    - `RADIAL_SECTOR_FUNCTION3 (3)`: Bottom-Left ($dx < 0, dy \ge 0$).
    - `RADIAL_SECTOR_FUNCTION4 (4)`: Top-Left ($dx < 0, dy < 0$).

## Dependencies & Data Flow
- **Imports**: `radial_types.h`, `<math.h>`.
- **Invoked By**: `menu_state.c`, `test_geometry.c`.
- **Data Flow**: Raw cursor coordinates -> pure geometric calculation -> classified `RadialSector`.
