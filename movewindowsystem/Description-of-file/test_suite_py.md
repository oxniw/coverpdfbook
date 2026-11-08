# Description: test_suite.py

## Overview
Automated unit and integration test suite executing under Python's `unittest` framework. Validates pure domain geometry calculations (centering, top-left, custom offsets, screen clamping) and integration with the compiled native `move_window.dll`.

## Exports & Functions
- `class TestGeometryCalculations(unittest.TestCase)`:
  - `test_center_alignment()`: Verifies math for centering windows on cursor.
  - `test_top_left_alignment()`: Verifies top-left anchor math.
  - `test_custom_offset_alignment()`: Verifies delta offsets.
  - `test_boundary_clamping_inside()`: Verifies unconstrained placement inside bounds.
  - `test_boundary_clamping_overflow()`: Verifies snapping when overlapping borders.
- `class TestNativeLibraryBindings(unittest.TestCase)`:
  - `test_cursor_query()`: Verifies live cursor retrieval from Windows subsystem.
  - `test_foreground_window_query()`: Verifies active window detection.
  - `test_invalid_window_handle_reposition()`: Asserts that invalid HWNDs return `MOVE_WINDOW_ERR_NOT_FOUND`.

## Dependencies & Data Flow
- **Imports**: `unittest`, `config.py`, `geometry_calc.py`, `move_window_api.py`.
- **Invoked By**: Continuous integration or manual execution (`python test_suite.py`).
- **Data Flow**: Test fixtures -> Math verification & DLL function invocation -> Assertions.
