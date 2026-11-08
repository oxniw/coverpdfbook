# Description: main.py

## Overview
Entry-point orchestration script for the Python CLI. Parses user flags, instantiates `MoveWindowNativeAPI`, resolves the target window, and dispatches repositioning or runs the background hotkey daemon.

## Exports & Functions
- `parse_args() -> argparse.Namespace`: Parses CLI arguments.
- `resolve_target_strategy(args) -> tuple`: Maps options to `SelectorStrategy`.
- `map_alignment_mode(align_str: str) -> AlignmentMode`: Converts CLI string to enum.
- `execute_reposition(api, resolver, strategy, query, align, off_x, off_y, clamp) -> int`: Executes move and logs status.
- `main() -> None`: Orchestrates initialization and execution.

## Dependencies & Data Flow
- **Imports**: `argparse`, `logging`, `sys`, `config.py`, `move_window_api.py`, `window_finder.py`, `hotkey_listener.py`.
- **Invoked By**: Direct terminal execution (`python main.py`).
- **Data Flow**: Command line flags -> API bindings -> Repositioned window.
