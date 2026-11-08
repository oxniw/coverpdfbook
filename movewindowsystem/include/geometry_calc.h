// ==============================================================================
// File: .//Description-of-file/geometry_calc_h.md
// Overview: Pure domain logic interface for window-to-mouse coordinate calculations and screen clamping.
// ==============================================================================

#ifndef MOVE_WINDOW_GEOMETRY_CALC_H // Guard against multiple inclusions
#define MOVE_WINDOW_GEOMETRY_CALC_H // Define macro for header guard

#include "config.h" // Include shared configuration and type definitions

#ifdef __cplusplus // Check if compiling under C++ compiler
extern "C" { // Enable C linkage for C++ callers
#endif // End of C++ language check

// Computes raw target (x, y) based on window size, mouse point, alignment mode, and custom offsets
Point2D calculate_window_target_position( // Declare target position calculation function
    Point2D mouse_pos, // Current cursor position coordinates
    int window_width, // Width of target window in pixels
    int window_height, // Height of target window in pixels
    AlignmentMode mode, // Alignment anchor mode (center, top-left, custom)
    int offset_x, // Custom horizontal pixel offset
    int offset_y // Custom vertical pixel offset
); // End of function declaration

// Clamps a rectangle inside a bounding monitor rectangle so it does not bleed off-screen
Point2D clamp_position_to_bounds( // Declare screen boundary clamping function
    Point2D desired_pos, // Computed ideal position before boundary adjustment
    int window_width, // Width of target window in pixels
    int window_height, // Height of target window in pixels
    Rect2D bounds // Bounding rectangle of the active display monitor
); // End of function declaration

#ifdef __cplusplus // Check if compiling under C++ compiler
} // Close C linkage block
#endif // End of C++ language check

#endif // Close header guard MOVE_WINDOW_GEOMETRY_CALC_H
