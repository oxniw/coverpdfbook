# Description: menu_state.c / menu_state.h

## Overview
Pure C state machine managing the interaction lifecycle (IDLE, PENDING, ACTIVE), threshold verification, and active sector tracking.

## Exports & Functions
- `menu_state_init(sm: MenuStateMachine*, drag_threshold: float, inner_radius: float) -> void`
  - **Description**: Initializes state machine with threshold configuration.
- `menu_state_on_down(sm: MenuStateMachine*, x: int32_t, y: int32_t) -> void`
  - **Description**: Records initial right-click anchor position and enters `GESTURE_STATE_PENDING`.
- `menu_state_on_move(sm: MenuStateMachine*, x: int32_t, y: int32_t) -> int`
  - **Description**: Evaluates mouse movement against drag threshold. Transitions from `PENDING` to `ACTIVE` when exceeded and resolves active sector.
  - **Returns**: `1` if just activated on this move, `0` otherwise.
- `menu_state_on_up(sm: MenuStateMachine*, confirmed_sector: RadialSector*) -> int`
  - **Description**: Handles button release, outputs confirmed sector, and resets to `GESTURE_STATE_IDLE`.
  - **Returns**: `1` if the gesture was active, `0` if it was a quick tap.
- `menu_state_reset(sm: MenuStateMachine*) -> void`
  - **Description**: Forces state back to `GESTURE_STATE_IDLE`.

## Dependencies & Data Flow
- **Imports**: `radial_types.h`, `math_geometry.h`.
- **Invoked By**: `radial_controller.cpp`, `test_geometry.c`.
- **Data Flow**: Mouse hook events -> state transitions -> current sector updates.
