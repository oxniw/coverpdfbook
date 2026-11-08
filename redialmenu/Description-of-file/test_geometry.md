# Description: test_geometry.c

## Overview
Comprehensive test suite verifying Euclidean distance formulas, angle degree mappings, quadrant sector boundaries, dead-zone thresholds, and state machine transitions.

## Exports & Functions
- `main(void) -> int`
  - **Description**: Executes assertions for distance, quadrants 1-4, center "Nothing" circle, and state machine event progression.
  - **Returns**: `0` if all assertions pass.

## Dependencies & Data Flow
- **Imports**: `math_geometry.h`, `menu_state.h`, `<stdio.h>`, `<assert.h>`, `<math.h>`.
- **Invoked By**: Build / test runners (`tests/test_geometry.exe`).
- **Data Flow**: Test fixtures -> domain math & state logic -> assertions.
