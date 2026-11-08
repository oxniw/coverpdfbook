# Description: window_finder.py

## Overview
Window resolution strategy component for Python. Translates user commands (`active`, `title`, `process`, `hwnd`) into resolved Windows handles (`int` / `HWND`) via `MoveWindowNativeAPI`.

## Exports & Functions
- `class SelectorStrategy(Enum)`: Strategy options (`ACTIVE_FOREGROUND`, `TITLE_SUBSTRING`, `PROCESS_NAME`, `RAW_HWND`).
- `class WindowResolver`:
  - `__init__(api: MoveWindowNativeAPI)`: Initializes resolver with API client.
  - `resolve(strategy: SelectorStrategy, query: Optional[str] = None) -> Optional[int]`: Returns window handle or `None`.

## Dependencies & Data Flow
- **Imports**: `enum.Enum`, `typing.Optional`, `move_window_api.py`.
- **Invoked By**: `python/main.py`.
- **Data Flow**: Accepts CLI/API queries and returns resolved Win32 HWNDs.
