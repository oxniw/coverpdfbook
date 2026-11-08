# Description: main.py

## Overview
Primary Python application entry point. Coordinates configuration, initializes the C++ bridge, wires action dispatchers, listens for termination signals (Ctrl+C), and drives the responsive event pump.

## Exports & Functions
- `main() -> int`
  - **Description**: Bootstraps the application, connects signals, initializes mouse hooks, and runs the polling loop until exit.
  - **Returns**: `0` on clean exit, `1` on error.

## Dependencies & Data Flow
- **Imports**: `signal`, `sys`, `time`, `radial_bridge.RadialMenuBridge`, `action_handlers.dispatch_action`, `logger.setup_logger`.
- **Invoked By**: Direct command-line execution (`python main.py`).
- **Data Flow**: OS startup -> bridge initialization -> hook installation -> continuous polling -> graceful shutdown on exit signal.
