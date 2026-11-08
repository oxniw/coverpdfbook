# Description: window_finder.hpp

## Overview
C++ abstraction header providing the `WindowFinder` class and `WindowTargetSpec` structure. Encapsulates multiple window discovery strategies (Title, Process, HWND, Foreground) behind a unified modern C++ interface returning `std::optional<HWND>`.

## Exports & Functions
- `enum class SelectorType`:
  - `ActiveForeground`: Query foreground window.
  - `TitleSubstring`: Query by title text.
  - `ProcessName`: Query by executable name.
  - `RawHandle`: Query by numeric HWND.
- `struct WindowTargetSpec`: Data holder for selector type, query string, and HWND.
- `class WindowFinder`:
  - `static std::optional<HWND> resolve(const WindowTargetSpec& spec)`: Resolves target window.
  - `static std::string get_window_title(HWND hwnd)`: Returns window title string.

## Dependencies & Data Flow
- **Imports**: `move_window_api.h`, `<string>`, `<optional>`.
- **Invoked By**: `main.cpp`, `window_finder.cpp`.
- **Data Flow**: Consumes configuration options from CLI or controller, returns resolved `HWND`.
