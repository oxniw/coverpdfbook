# Description: geometry_calc.py

## Overview
Pure domain calculation module in Python implementing geometry algorithms for snapping windows to cursor positions and clamping them inside monitor work boundaries. Free of side effects or OS dependencies.

## Exports & Functions
- `calculate_window_target_position(mouse_pos, window_width, window_height, mode, offset_x=0, offset_y=0) -> Point2D`
  - **Description**: Calculates top-left position based on alignment mode (`CENTER`, `TOP_LEFT`, `CUSTOM_OFFSET`).
- `clamp_position_to_bounds(desired_pos, window_width, window_height, bounds) -> Point2D`
  - **Description**: Ensures window does not extend beyond monitor boundaries.

## Dependencies & Data Flow
- **Imports**: `config.py` (`AlignmentMode`, `Point2D`, `Rect2D`).
- **Invoked By**: `test_suite.py`, `move_window_api.py`.
- **Data Flow**: Pure in-memory coordinate transformations.
