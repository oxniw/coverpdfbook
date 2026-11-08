# ==============================================================================
# File: .//Description-of-file/geometry_calc_py.md
# Overview: Pure domain calculation module for window placement coordinates and boundary clamping in Python.
# ==============================================================================

from config import AlignmentMode, Point2D, Rect2D # Import configuration types

def calculate_window_target_position( # Define target coordinate calculation function
    mouse_pos: Point2D, # Cursor coordinate position
    window_width: int, # Window width in pixels
    window_height: int, # Window height in pixels
    mode: AlignmentMode, # Alignment anchor mode
    offset_x: int = 0, # Custom horizontal offset
    offset_y: int = 0 # Custom vertical offset
) -> Point2D: # Return calculated Point2D
    if mode == AlignmentMode.CENTER: # Center alignment branch
        return Point2D( # Return center-calculated point
            x=mouse_pos.x - (window_width // 2), # Compute centered horizontal X coordinate
            y=mouse_pos.y - (window_height // 2) # Compute centered vertical Y coordinate
        ) # Close point instantiation
    elif mode == AlignmentMode.TOP_LEFT: # Top-left alignment branch
        return Point2D(x=mouse_pos.x, y=mouse_pos.y) # Return top-left snapped coordinate
    elif mode == AlignmentMode.CUSTOM_OFFSET: # Custom offset alignment branch
        return Point2D( # Return custom-offset coordinate
            x=mouse_pos.x + offset_x, # Apply custom X offset
            y=mouse_pos.y + offset_y # Apply custom Y offset
        ) # Close point instantiation
    return Point2D( # Default fallback calculation
        x=mouse_pos.x - (window_width // 2), # Fallback centered X
        y=mouse_pos.y - (window_height // 2) # Fallback centered Y
    ) # Close point instantiation

def clamp_position_to_bounds( # Define boundary clamping function
    desired_pos: Point2D, # Target position before clamping
    window_width: int, # Window width in pixels
    window_height: int, # Window height in pixels
    bounds: Rect2D # Display monitor bounds
) -> Point2D: # Return boundary-clamped Point2D
    max_x = bounds.x + bounds.width - window_width # Calculate maximum allowable X coordinate
    max_y = bounds.y + bounds.height - window_height # Calculate maximum allowable Y coordinate

    clamped_x = desired_pos.x # Initialize clamped X coordinate
    if clamped_x < bounds.x: # Check left edge overflow
        clamped_x = bounds.x # Clamp to left edge
    elif clamped_x > max_x: # Check right edge overflow
        clamped_x = max(bounds.x, max_x) # Clamp to right boundary or min bounds if window is larger than screen

    clamped_y = desired_pos.y # Initialize clamped Y coordinate
    if clamped_y < bounds.y: # Check top edge overflow
        clamped_y = bounds.y # Clamp to top edge
    elif clamped_y > max_y: # Check bottom edge overflow
        clamped_y = max(bounds.y, max_y) # Clamp to bottom boundary or min bounds if window is larger than screen

    return Point2D(x=clamped_x, y=clamped_y) # Return final clamped point
