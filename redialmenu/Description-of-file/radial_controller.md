# Description: radial_controller.cpp / radial_controller.h

## Overview
Central coordinator tying together the mouse hook, state machine, overlay window, and registered action callbacks into a cohesive operational workflow.

## Exports & Functions
- `radial_controller_init() -> int`
  - **Description**: Bootstraps GDI+, state machine, and overlay window.
  - **Returns**: `1` on success, `0` on failure.
- `radial_controller_start() -> int`
  - **Description**: Activates the mouse hook.
- `radial_controller_stop() -> void`
  - **Description**: Stops mouse hook and hides overlay.
- `radial_controller_set_callback(cb: RadialActionCallback, user_data: void*) -> void`
  - **Description**: Binds callback function for confirmed selections.
- `radial_controller_on_mouse_down(x: int32_t, y: int32_t) -> void`
  - **Description**: Forwards mouse down event to state machine.
- `radial_controller_on_mouse_move(x: int32_t, y: int32_t) -> void`
  - **Description**: Checks for gesture activation, shows overlay at anchor, and updates highlight.
- `radial_controller_on_mouse_up(x: int32_t, y: int32_t) -> int`
  - **Description**: Finalizes gesture, hides overlay, and invokes action callback if a sector was chosen.
  - **Returns**: `1` if a drag gesture was handled, `0` if normal click.
- `radial_controller_shutdown() -> void`
  - **Description**: Cleans up all subsystem resources.
- `radial_controller_get_instance() -> RadialController*`
  - **Description**: Returns pointer to singleton controller.

## Dependencies & Data Flow
- **Imports**: `radial_types.h`, `menu_state.h`, `overlay_window.h`, `mouse_hook.h`, `render_overlay.h`, `config.h`.
- **Invoked By**: `radial_api.cpp`.
- **Data Flow**: Mouse hook input -> state evaluation -> window rendering -> client action callback.
