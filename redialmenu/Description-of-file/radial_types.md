# Description: radial_types.h

## Overview
Defines core data structures, enums, callback function pointers, and fundamental types used across the C and C++ radial menu subsystem.

## Exports & Structures
- `enum RadialSector`
  - `RADIAL_SECTOR_NOTHING (0)`: Center dead-zone circle (Cancel).
  - `RADIAL_SECTOR_FUNCTION1 (1)`: Top-Right quadrant.
  - `RADIAL_SECTOR_FUNCTION2 (2)`: Bottom-Right quadrant.
  - `RADIAL_SECTOR_FUNCTION3 (3)`: Bottom-Left quadrant.
  - `RADIAL_SECTOR_FUNCTION4 (4)`: Top-Left quadrant.
- `enum GestureState`
  - `GESTURE_STATE_IDLE (0)`: Waiting for input.
  - `GESTURE_STATE_PENDING (1)`: Right mouse down, assessing drag threshold.
  - `GESTURE_STATE_ACTIVE (2)`: Drag threshold exceeded, overlay visible and active.
- `struct GeometryResult`
  - `distance`: Euclidean distance from gesture center.
  - `angle_deg`: Angle in degrees [0, 360).
  - `sector`: Resolved RadialSector.
- `struct RadialPoint`
  - `x`: Integer 32-bit horizontal coordinate.
  - `y`: Integer 32-bit vertical coordinate.
- `typedef void (*RadialActionCallback)(int32_t sector, void* user_data)`
  - Function pointer prototype for action execution notification.

## Dependencies & Data Flow
- **Imports**: `<stdint.h>`, `<stdbool.h>`.
- **Invoked By**: Included by all C/C++ engine components (`math_geometry`, `menu_state`, `overlay_window`, `radial_controller`, `radial_api`).
- **Data Flow**: Carries sector selections and coordinates between mouse hooks, state machines, and rendering routines.
