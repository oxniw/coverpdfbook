# Description: calculate_grid_layout

## Overview
Pure domain algorithm responsible for computing the exact $M \times N$ cell layout, aspect-ratio-preserved bounding boxes, dynamic anchor positioning (Top-Left, Middle/Center, Top-Right, Bottom-Left, etc.), and multi-page pagination partitions.

## Exports & Functions
- `calculateGridLayoutForPage(const AppSettings& settings, int targetPage, int totalItems, int windowWidth, int windowHeight, std::vector<RECT>& outItemRects) -> GridLayoutResult`
  - **Description**: Calculates grid layout and item bounding boxes for any specified target page index. Used by transition animation renderers to compute simultaneous outgoing and incoming page geometry.
- `calculateGridLayout(const AppSettings& settings, int totalItems, int windowWidth, int windowHeight, std::vector<RECT>& outItemRects) -> GridLayoutResult`
  - **Description**: Calculates page counts, items-per-page, active page subsets, and screen-space `RECT` coordinates for every visible PDF cover positioned according to `settings.alignment` (Top-Left, Center, Top-Right, etc.).
  - **Parameters**:
    - `settings`: The current application state containing row count ($M$), column count ($N$), cover size, alignment mode, and current page.
    - `totalItems`: The total count of discovered PDF items.
    - `windowWidth`: Current client width of the overlay window (full screen width).
    - `windowHeight`: Current client height of the overlay window (full screen height).
    - `outItemRects`: Output vector populated with the computed `RECT` boundaries for visible items on the active page.
  - **Returns**: A `GridLayoutResult` containing pagination metrics and overall grid bounding box.

## Dependencies & Data Flow
- **Imports**: `config.h`, `<vector>`, `<algorithm>`.
- **Invoked By**: Called from `main.cpp` (`refreshDisplay`) before each render pass and input hit-test.
- **Data Flow**: `AppSettings` + desktop screen dimensions -> Geometric math calculations -> `outItemRects` + `GridLayoutResult`.
