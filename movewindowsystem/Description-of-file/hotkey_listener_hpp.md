# Description: hotkey_listener.hpp

## Overview
Declares the `HotkeyListener` C++ class which registers a system-wide hotkey using Win32 `RegisterHotKey` and pumps messages in a dedicated thread or blocking loop to trigger a callback function.

## Exports & Functions
- `class HotkeyListener`:
  - `HotkeyListener(int hotkey_id, UINT modifiers, UINT vk_key, ActionCallback callback)`: Initializes hotkey configuration.
  - `bool start_async()`: Launches hotkey message pump in a background `std::thread`.
  - `void start_blocking()`: Runs message loop synchronously on calling thread.
  - `void stop()`: Posts `WM_QUIT` and joins background thread.
  - `bool is_running() const`: Queries active status.

## Dependencies & Data Flow
- **Imports**: `config.h`, `<windows.h>`, `<functional>`, `<atomic>`, `<thread>`.
- **Invoked By**: `main.cpp`, `hotkey_listener.cpp`.
- **Data Flow**: Captures OS keyboard events and fires move actions.
