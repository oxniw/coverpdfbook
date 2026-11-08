# Description: geometry_calc.c

## Overview
Implementation file containing pure mathematical calculations for snapping a window to the mouse cursor and constraining coordinates within monitor display bounds.

## Exports & Functions
- `Point2D calculate_window_target_position(Point2D mouse_pos, int window_width, int window_height, AlignmentMode mode, int offset_x, int offset_y)`
  - **Description**: Evaluates alignment logic and calculates new X and Y coordinates.
- `Point2D clamp_position_to_bounds(Point2D desired_pos, int window_width, int window_height, Rect2D bounds)`
  - **Description**: Ensures window does not render off-screen, respecting multi-monitor layouts.

## Dependencies & Data Flow
- **Imports**: `geometry_calc.h`.
- **Invoked By**: `move_window_api.c`.
- **Data Flow**: Arithmetic operations on pure scalar coordinates without calling Windows OS APIs.
