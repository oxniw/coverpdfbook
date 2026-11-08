# Description: action_handlers.py

## Overview
OS automation and productivity shortcut dispatcher. Executes simulated keystrokes using native Windows `SendInput` via `ctypes` without requiring external third-party Python packages.

## Exports & Functions
- `send_hotkey(modifier_vk: int, key_vk: int) -> None`
  - **Description**: Synthesizes key-down and key-up events for modifier combinations (e.g. Ctrl + Key).
- `handle_function1() -> None`: Triggers Copy (Ctrl + C).
- `handle_function2() -> None`: Triggers Paste (Ctrl + V).
- `handle_function3() -> None`: Triggers Undo (Ctrl + Z).
- `handle_function4() -> None`: Triggers Redo (Ctrl + Y).
- `dispatch_action(sector_id: int) -> None`
  - **Description**: Receives sector ID from native callback, looks up registered handler, and executes it.

## Dependencies & Data Flow
- **Imports**: `ctypes`, `ctypes.wintypes`, `time`, `config.RadialSector`, `config.SECTOR_LABELS`, `logger.setup_logger`.
- **Invoked By**: `main.py` (via `bridge.set_callback(dispatch_action)`).
- **Data Flow**: Native callback sector integer -> sector lookup -> `SendInput` keyboard simulation to active foreground window.
