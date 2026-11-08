# Description: mouse_hook.cpp / mouse_hook.h

## Overview
Low-level Windows mouse hook module (`WH_MOUSE_LL`). Intercepts global right-click mouse events, differentiates between stationary clicks and drag gestures, and handles pass-through replaying of normal right-clicks.

## Exports & Functions
- `mouse_hook_install(callbacks: MouseHookCallbacks) -> int`
  - **Description**: Installs global `WH_MOUSE_LL` hook and assigns down/move/up event handlers.
  - **Returns**: `1` on success, `0` on error.
- `mouse_hook_uninstall() -> void`
  - **Description**: Unhooks `WH_MOUSE_LL` and unbinds event handlers.
- `mouse_hook_replay_right_click(x: int32_t, y: int32_t) -> void`
  - **Description**: Injects a synthetic right-down + right-up mouse event via `SendInput` with an injected flag to open standard Windows context menus when the user performs a quick click.

## Dependencies & Data Flow
- **Imports**: `<windows.h>`, `radial_types.h`.
- **Invoked By**: `radial_controller.cpp`.
- **Data Flow**: OS mouse messages -> `LowLevelMouseProc` -> `radial_controller` callbacks -> gesture execution or synthetic replay.
