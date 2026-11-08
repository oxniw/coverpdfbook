// ==============================================================================
// File: .//Description-of-file/geometry_calc_c.md
// Overview: Pure domain implementation for window-to-mouse coordinate calculations and screen clamping.
// ==============================================================================

#include "geometry_calc.h" // Include corresponding header declarations

Point2D calculate_window_target_position( // Define target position calculation function
    Point2D mouse_pos, // Current cursor position coordinates
    int window_width, // Width of target window in pixels
    int window_height, // Height of target window in pixels
    AlignmentMode mode, // Alignment anchor mode
    int offset_x, // Custom horizontal pixel offset
    int offset_y // Custom vertical pixel offset
) { // Begin function body
    Point2D result; // Create return point structure
    result.x = mouse_pos.x; // Default horizontal position to cursor X
    result.y = mouse_pos.y; // Default vertical position to cursor Y

    switch (mode) { // Branch logic based on specified alignment mode
        case ALIGN_CENTER: // Handle center alignment mode
            result.x = mouse_pos.x - (window_width / 2); // Center window horizontally on cursor
            result.y = mouse_pos.y - (window_height / 2); // Center window vertically on cursor
            break; // Exit switch statement

        case ALIGN_TOP_LEFT: // Handle top-left alignment mode
            result.x = mouse_pos.x; // Align window left edge with cursor X
            result.y = mouse_pos.y; // Align window top edge with cursor Y
            break; // Exit switch statement

        case ALIGN_CUSTOM_OFFSET: // Handle custom offset mode
            result.x = mouse_pos.x + offset_x; // Apply horizontal offset relative to cursor
            result.y = mouse_pos.y + offset_y; // Apply vertical offset relative to cursor
            break; // Exit switch statement

        default: // Handle unrecognized mode fallback
            result.x = mouse_pos.x - (window_width / 2); // Default fallback to center alignment X
            result.y = mouse_pos.y - (window_height / 2); // Default fallback to center alignment Y
            break; // Exit switch statement
    } // End switch statement

    return result; // Return computed target position
} // End function body

Point2D clamp_position_to_bounds( // Define screen boundary clamping function
    Point2D desired_pos, // Computed ideal position before boundary adjustment
    int window_width, // Width of target window in pixels
    int window_height, // Height of target window in pixels
    Rect2D bounds // Bounding rectangle of the active display monitor
) { // Begin function body
    Point2D clamped = desired_pos; // Initialize clamped result with desired position
    int max_x = bounds.x + bounds.width - window_width; // Compute rightmost allowable X coordinate
    int max_y = bounds.y + bounds.height - window_height; // Compute bottommost allowable Y coordinate

    if (clamped.x < bounds.x) { // Check if window exceeds left boundary
        clamped.x = bounds.x; // Snap window to left screen boundary
    } else if (clamped.x > max_x) { // Check if window exceeds right boundary
        clamped.x = (max_x >= bounds.x) ? max_x : bounds.x; // Snap window to right boundary or min X if oversized
    } // End horizontal clamp check

    if (clamped.y < bounds.y) { // Check if window exceeds top boundary
        clamped.y = bounds.y; // Snap window to top screen boundary
    } else if (clamped.y > max_y) { // Check if window exceeds bottom boundary
        clamped.y = (max_y >= bounds.y) ? max_y : bounds.y; // Snap window to bottom boundary or min Y if oversized
    } // End vertical clamp check

    return clamped; // Return boundary-adjusted point
} // End function body
