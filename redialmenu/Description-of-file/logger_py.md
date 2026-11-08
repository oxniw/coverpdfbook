# Description: logger.py

## Overview
Production-grade structured logging module strictly conforming to the `prevent-print` skill. Replaces standard console `print()` calls with timestamped, leveled logs (`INFO`, `WARNING`, `ERROR`, `DEBUG`) routed to standard error.

## Exports & Functions
- `setup_logger(name: str = "RadialMenu") -> logging.Logger`
  - **Description**: Configures and returns a thread-safe `logging.Logger` with standard date-time formatting.
  - **Parameters**:
    - `name`: Logger channel identifier.
  - **Returns**: Configured `logging.Logger` instance.

## Dependencies & Data Flow
- **Imports**: `logging`, `sys`.
- **Invoked By**: `radial_bridge.py`, `action_handlers.py`, `main.py`.
- **Data Flow**: Diagnostic messages -> formatted timestamped records -> standard error stream.
