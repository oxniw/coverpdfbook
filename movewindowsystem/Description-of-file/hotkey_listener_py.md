# Description: hotkey_listener.py

## Overview
Provides a background hotkey listener daemon (`HotkeyDaemon`) in Python using `ctypes` bindings to Windows `user32.dll` (`RegisterHotKey`, `GetMessageW`, `UnregisterHotKey`, `PostThreadMessageW`).

## Exports & Functions
- `class HotkeyDaemon`:
  - `__init__(callback, modifiers, vk, hotkey_id)`: Configures hotkey keys and action callback.
  - `start_blocking()`: Enters Windows message pump on calling thread.
  - `start_async()`: Launches message pump in a background daemon thread.
  - `stop()`: Signals exit via `WM_QUIT` and cleans up hotkey registration.

## Dependencies & Data Flow
- **Imports**: `ctypes`, `logging`, `threading`, `config.py`.
- **Invoked By**: `python/main.py`.
- **Data Flow**: Listens for system global key combinations and triggers move operations.
