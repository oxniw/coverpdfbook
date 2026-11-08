# Description: radial_api.h

## Overview
Declares the public, stable C-ABI interface exported by `radial_menu.dll`. Serves as the explicit seam between native C++ graphics/hooking logic and consumer environments such as Python ctypes.

## Exports & Functions
- `radial_init() -> int`
  - **Description**: Initializes the radial menu engine, GDI+, and transparent window.
  - **Returns**: `1` on success, `0` on failure.
- `radial_set_sector_count(count: int) -> void`
  - **Description**: Configures dynamic number of outer sectors N (2 to 16).
- `radial_get_sector_count() -> int`
  - **Description**: Returns currently configured sector count N.
- `radial_set_sector_label(sector_idx: int, label: const wchar_t*) -> void`
  - **Description**: Configures custom display label for sector (1 to N).
- `radial_set_callback(callback: RadialActionCallback, user_data: void*) -> void`
  - **Description**: Registers a function callback that gets triggered when a quadrant is selected.
  - **Parameters**:
    - `callback`: Function pointer conforming to `RadialActionCallback`.
    - `user_data`: Opaque pointer passed through to the callback.
- `radial_start_hook() -> int`
  - **Description**: Installs the low-level Windows mouse hook (`WH_MOUSE_LL`).
  - **Returns**: `1` on success, `0` on failure.
- `radial_stop_hook() -> void`
  - **Description**: Removes the mouse hook and resets internal gesture states.
- `radial_poll_events() -> int`
  - **Description**: Non-blocking single message pump dispatch.
  - **Returns**: `1` if a message was dispatched, `0` if queue is empty, `-1` on `WM_QUIT`.
- `radial_run_loop() -> void`
  - **Description**: Blocking standard Windows message loop.
- `radial_shutdown() -> void`
  - **Description**: Stops hooks, tears down overlay windows, shuts down GDI+, and cleans up memory.
- `radial_get_gesture_state() -> int`
  - **Description**: Retrieves current state (`0=IDLE`, `1=PENDING`, `2=ACTIVE`).
- `radial_show_at(screen_x: int, screen_y: int) -> void`
  - **Description**: Manually positions and renders overlay at given screen coordinates.
- `radial_hide() -> void`
  - **Description**: Manually hides the overlay window.
- `radial_set_highlight(sector: int) -> void`
  - **Description**: Updates highlighted sector on overlay.

## Dependencies & Data Flow
- **Imports**: `radial_types.h`.
- **Invoked By**: `main.cpp`, `radial_bridge.py`.
- **Data Flow**: High-level commands enter from caller -> forwarded to `radial_controller`.
