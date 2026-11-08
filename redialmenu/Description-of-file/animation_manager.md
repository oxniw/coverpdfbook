# Description: animation_manager.c / animation_manager.h

## Overview
Pure domain animation manager containing parametric easing curves, pop-in spring transitions, and per-sector hover interpolation. Completely decoupled from Win32 and GDI+.

## Exports & Functions
- `anim_ease_out_cubic(t: float) -> float`
  - **Description**: Smooth cubic ease-out curve $1 - (1 - t)^3$.
- `anim_ease_out_back(t: float) -> float`
  - **Description**: Spring overshoot curve with cubic pull-back.
- `anim_init(am: AnimationManager*) -> void`
  - **Description**: Initializes manager properties to identity.
- `anim_start_spawn(am: AnimationManager*, now_ms: uint64_t) -> void`
  - **Description**: Initiates the pop-in spring animation from scale 0.45 to 1.0.
- `anim_start_selection(am: AnimationManager*, sector: RadialSector, now_ms: uint64_t) -> void`
  - **Description**: Initiates the visual selection confirmation animation (pulse burst pop and smooth alpha fade-out over 120ms).
- `anim_is_selection_complete(am: AnimationManager*) -> bool`
  - **Description**: Checks whether the selection animation has concluded so the overlay window can safely hide.
- `anim_set_target_sector(am: AnimationManager*, sector: RadialSector) -> void`
  - **Description**: Sets active target sector to trigger smooth crossfade transitions.
- `anim_tick(am: AnimationManager*, now_ms: uint64_t) -> int`
  - **Description**: Advances animation by elapsed time delta.
  - **Returns**: `1` if visual properties changed and redraw is required, `0` if settled.

## Dependencies & Data Flow
- **Imports**: `radial_types.h`, `<math.h>`, `<stdbool.h>`.
- **Invoked By**: `radial_controller.cpp`.
- **Data Flow**: Millisecond timestamps -> easing calculations -> scale, alpha, and per-sector weights -> `overlay_window_update_animated`.
