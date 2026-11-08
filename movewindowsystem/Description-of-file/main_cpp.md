# Description: main.cpp

## Overview
Entry-point application file for the C++ CLI executable (`move_window_cli.exe`). Parses command-line flags, selects window target strategies, and dispatches either a one-shot move operation or launches a background hotkey listener daemon.

## Exports & Functions
- `CliConfig parse_arguments(int argc, char* argv[])`
  - **Description**: Parses flags such as `--title`, `--process`, `--hwnd`, `--active`, `--align`, `--offset-x`, `--offset-y`, `--no-clamp`, `--daemon`.
  - **Parameters**: Command-line arguments count and array.
  - **Returns**: Populated `CliConfig` struct.
- `int execute_move(const CliConfig& config)`
  - **Description**: Resolves the target HWND through `WindowFinder` and calls `move_window_to_cursor`.
  - **Parameters**: `CliConfig`.
  - **Returns**: Status integer (0 for success, negative error code on failure).
- `int main(int argc, char* argv[])`
  - **Description**: Standard program entry point orchestrating setup and execution.

## Dependencies & Data Flow
- **Imports**: `window_finder.hpp`, `hotkey_listener.hpp`.
- **Invoked By**: Operating system loader upon execution.
- **Data Flow**: CLI arguments -> `CliConfig` -> `WindowFinder` -> `move_window_to_cursor` / `HotkeyListener`.
