# Description: config.py

## Overview
Defines Python-side configuration constants, status error codes, data containers (`Point2D`, `Rect2D`), and the `AlignmentMode` enumeration matching the C header constants.

## Exports & Functions
- `AlignmentMode`: Enum with values `CENTER`, `TOP_LEFT`, `CUSTOM_OFFSET`.
- `Point2D`: NamedTuple for `(x, y)` coordinates.
- `Rect2D`: NamedTuple for `(x, y, width, height)` bounding rectangles.
- Error code constants (`MOVE_WINDOW_SUCCESS`, `MOVE_WINDOW_ERR_NOT_FOUND`, etc.).
- Default configuration settings (`DEFAULT_OFFSET_X`, `DEFAULT_OFFSET_Y`, `DEFAULT_CLAMP_SCREEN`).

## Dependencies & Data Flow
- **Imports**: `enum.IntEnum`, `typing.NamedTuple`.
- **Invoked By**: `geometry_calc.py`, `move_window_api.py`, `window_finder.py`, `hotkey_listener.py`, `main.py`.
- **Data Flow**: Supplies common constants and types across all Python modules.
