// ==============================================================================
// File: .//Description-of-file/config_h.md
// Overview: Shared configuration constants and visual dimensions for radial menu.
// ==============================================================================

#ifndef CONFIG_H
#define CONFIG_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// Visual dimensions (pixels)
#define RADIAL_INNER_RADIUS_PX       45.0f   // Radius of the "Nothing" central circle
#define RADIAL_OUTER_RADIUS_PX      150.0f   // Outer radius of the radial sectors
#define RADIAL_WINDOW_SIZE_PX       400      // Total width & height (accommodates 1.2x slice expansion)
#define RADIAL_DRAG_THRESHOLD_PX     18.0f   // Minimum drag distance before menu activates

// Color palette (ARGB format)
#define COLOR_OVERLAY_BG            0x00000000 // Fully transparent window background
#define COLOR_SLICE_DEFAULT_FILL    0xD0FFFFFF // Semi-translucent white fill for slices
#define COLOR_SLICE_HOVER_FILL      0xD04CAF50 // Active slice highlight fill (vibrant green)
#define COLOR_CENTER_DEFAULT_FILL   0xE0F0F0F0 // Center circle default fill
#define COLOR_CENTER_HOVER_FILL     0xE0FFCDD2 // Center circle hover fill (subtle red/cancel)
#define COLOR_BORDER_LINE           0xFF202020 // Dark border strokes
#define COLOR_TEXT_DEFAULT          0xFF202020 // Text color for unselected slices
#define COLOR_TEXT_HOVER            0xFFFFFFFF // Text color for hovered slice

// Ring stroke width
#define RADIAL_BORDER_STROKE_WIDTH  4.0f

// Window class name
#define RADIAL_WINDOW_CLASS_NAME    "RadialMenuOverlayWindowClass"

#ifdef __cplusplus
}
#endif

#endif // CONFIG_H
