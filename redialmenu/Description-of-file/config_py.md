# Description: config.py

## Overview
Centralized Python configuration module defining enums, default dimensions, DLL file naming, and sector mapping labels.

## Exports & Functions
- `class RadialSector(IntEnum)`
  - Enumeration corresponding to sectors: `NOTHING (0)`, `FUNCTION1 (1)`, `FUNCTION2 (2)`, `FUNCTION3 (3)`, `FUNCTION4 (4)`.
- `DLL_NAME`: String name of compiled C-ABI dynamic library (`radial_menu.dll`).
- `INNER_RADIUS_PX`: 45.0 px dead-zone radius.
- `OUTER_RADIUS_PX`: 150.0 px outer radius.
- `WINDOW_SIZE_PX`: 340 px overlay dimensions.
- `DRAG_THRESHOLD_PX`: 18.0 px movement activation limit.
- `SECTOR_LABELS`: Mapping from `RadialSector` to human-readable strings.

## Dependencies & Data Flow
- **Imports**: `enum.IntEnum`.
- **Invoked By**: `radial_bridge.py`, `action_handlers.py`, `main.py`.
- **Data Flow**: Supplies static parameters to bridge and action handlers.
