# Description: page_animation_manager

## Overview
Pure domain animation manager providing easing curves and lifecycle state tracking for smooth 60 FPS slide-and-fade page transition animations between PDF bookshelf pages.

## Exports & Functions
- `calculateEaseOutCubic(float t) -> float`
  - **Description**: Computes a smooth ease-out cubic curve given a normalized time $t \in [0, 1]$ using the formula $f(t) = 1 - (1 - t)^3$.
  - **Parameters**:
    - `t`: Normalized progression factor from 0.0 (start) to 1.0 (end).
  - **Returns**: Eased progress factor in $[0, 1]$.

- `startPageAnimation(PageAnimationState& anim, int fromPage, int toPage, AnimationDirection dir) -> void`
  - **Description**: Initializes an animation lifecycle sequence from the current page to the target page, setting start timestamps, direction flags, and initial zero progress.
  - **Parameters**:
    - `anim`: State container reference modified with transition parameters.
    - `fromPage`: Index of the outgoing page.
    - `toPage`: Index of the incoming page.
    - `dir`: Horizontal direction (`ANIM_DIR_NEXT` or `ANIM_DIR_PREV`).
  - **Returns**: None.

- `updatePageAnimation(PageAnimationState& anim) -> bool`
  - **Description**: Advances animation elapsed time based on system tick count and recalculates eased progress.
  - **Parameters**:
    - `anim`: Mutable state container reference holding animation status.
  - **Returns**: `true` if animation is actively in progress; `false` once elapsed time reaches or exceeds transition duration.

## Dependencies & Data Flow
- **Imports**: `config.h`, `<algorithm>`.
- **Invoked By**: Called from `main.cpp` (`triggerPageTransition` to begin, `WM_TIMER` to advance frame by frame) and passed into `render_overlay_canvas.cpp`.
- **Data Flow**: Timer ticks -> `updatePageAnimation` -> `easedProgress` -> `renderOverlayCanvas` offscreen alpha blend and slide offset calculation.
