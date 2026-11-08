# Description: main.cpp

## Overview
Standalone native C++ application entry point. Demonstrates direct usage of the radial menu system without requiring a Python runtime.

## Exports & Functions
- `main(void) -> int`
  - **Description**: Initializes the radial menu engine, registers native action handlers, starts the mouse hook, and runs the Windows message loop until process termination.
- `on_action_triggered(int32_t sector, void* user_data) -> void`
  - **Description**: Internal callback handling confirmed sector selections.

## Dependencies & Data Flow
- **Imports**: `include/radial_api.h`, `<windows.h>`, `<stdio.h>`.
- **Invoked By**: OS process launcher (`radial_menu_cpp.exe`).
- **Data Flow**: OS startup -> engine setup -> event loop -> debug output / execution.
