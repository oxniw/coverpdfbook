# Description: geometry_calc.h

## Overview
Declares pure domain functions for computing 2D window target placement and clamping window bounds within screen boundaries. Free of third-party OS dependencies, enabling pure unit testing.

## Exports & Functions
- `Point2D calculate_window_target_position(Point2D mouse_pos, int window_width, int window_height, AlignmentMode mode, int offset_x, int offset_y)`
  - **Description**: Computes the top-left coordinate for a window given the cursor position and alignment mode.
  - **Parameters**:
    - `mouse_pos`: Cursor position structure containing X and Y coordinates.
    - `window_width`: Width of target window in pixels.
    - `window_height`: Height of target window in pixels.
    - `mode`: Target anchor alignment mode (`ALIGN_CENTER`, `ALIGN_TOP_LEFT`, `ALIGN_CUSTOM_OFFSET`).
    - `offset_x`: Delta X pixel offset for custom offset mode.
    - `offset_y`: Delta Y pixel offset for custom offset mode.
  - **Returns**: Computed `Point2D` (top-left corner).
- `Point2D clamp_position_to_bounds(Point2D desired_pos, int window_width, int window_height, Rect2D bounds)`
  - **Description**: Constrains window coordinates within monitor boundaries.
  - **Parameters**:
    - `desired_pos`: Proposed top-left coordinate.
    - `window_width`: Window width.
    - `window_height`: Window height.
    - `bounds`: Monitor rectangle (`Rect2D`).
  - **Returns**: Clamped `Point2D` ensuring the window remains fully inside the monitor boundary.

## Dependencies & Data Flow
- **Imports**: `config.h`.
- **Invoked By**: `geometry_calc.c`, `move_window_api.c`.
- **Data Flow**: Consumes mouse and window geometry data, returns calculated target positions.
