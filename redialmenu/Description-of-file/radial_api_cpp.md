# Description: radial_api.cpp

## Overview
Implements exported C-ABI functions defined in `radial_api.h` allowing foreign function interfaces (FFI), notably Python ctypes, to communicate with `radial_menu.dll`.

## Exports & Functions
- `radial_init() -> int`: Delegates to `radial_controller_init()`.
- `radial_set_callback(RadialActionCallback, void*) -> void`: Binds action callback.
- `radial_start_hook() -> int`: Starts hook listener.
- `radial_stop_hook() -> void`: Stops hook listener.
- `radial_poll_events() -> int`: Processes single Win32 message.
- `radial_run_loop() -> void`: Runs modal message pump.
- `radial_shutdown() -> void`: Cleans up engine and posts quit message.
- `radial_get_gesture_state() -> int`: Queries current gesture state.
- `radial_show_at(int, int) -> void`: Manually displays window.
- `radial_hide() -> void`: Manually hides window.
- `radial_set_highlight(int) -> void`: Manually highlights sector.

## Dependencies & Data Flow
- **Imports**: `radial_api.h`, `radial_controller.h`, `<windows.h>`.
- **Invoked By**: External DLL consumers (`radial_bridge.py`, `main.cpp`).
- **Data Flow**: High-level C calls -> internal controller methods.
