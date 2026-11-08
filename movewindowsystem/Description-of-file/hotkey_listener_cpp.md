# Description: hotkey_listener.cpp

## Overview
Implements the Windows message pump loop for global hotkey registration and notification. Handles thread-safe startup, shutdown, and hotkey cleanup.

## Exports & Functions
- `HotkeyListener::start_async`: Spawns async worker thread running `run_loop`.
- `HotkeyListener::start_blocking`: Runs `run_loop` on current thread.
- `HotkeyListener::stop`: Posts `WM_QUIT` using `PostThreadMessage` and unregisters hotkey.
- `HotkeyListener::run_loop`: Manages `RegisterHotKey`, `GetMessage`, dispatch, and callback invocation.

## Dependencies & Data Flow
- **Imports**: `hotkey_listener.hpp`.
- **Invoked By**: `main.cpp`.
- **Data Flow**: Listens for system `WM_HOTKEY` messages and dispatches action requests.
