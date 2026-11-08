# Description: radial_bridge.py

## Overview
High-level Python ctypes wrapper interfacing directly with `radial_menu.dll`. Handles DLL loading, function signature bindings, C callback marshaling, and lifecycle management.

## Exports & Functions
- `class RadialMenuBridge`
  - `__init__(dll_path: str = None)`: Loads dynamic library and binds C prototypes.
  - `initialize() -> bool`: Calls native `radial_init()`.
  - `set_sector_count(count: int) -> None`: Sets dynamic number of outer slices N (2 to 16).
  - `get_sector_count() -> int`: Returns current sector count.
  - `set_sector_label(sector_index: int, label: str) -> None`: Sets custom text label.
  - `set_callback(py_callback: Callable[[int], None]) -> None`: Wraps Python function into a C-compatible function pointer and registers it.
  - `start_hook() -> bool`: Calls native `radial_start_hook()`.
  - `stop_hook() -> None`: Calls native `radial_stop_hook()`.
  - `poll_events() -> int`: Dispatches one Win32 message (non-blocking).
  - `run_loop() -> None`: Starts blocking message loop.
  - `shutdown() -> None`: Tears down native resources.

## Dependencies & Data Flow
- **Imports**: `ctypes`, `os`, `sys`, `typing.Callable`, `config.DLL_NAME`, `logger.setup_logger`.
- **Invoked By**: `main.py`.
- **Data Flow**: Python requests -> ctypes FFI calls -> `radial_menu.dll` -> native C++ execution.
