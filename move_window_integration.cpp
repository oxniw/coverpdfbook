// ==============================================================================
// File: .//Description-of-file/move_window_integration.md
// Overview: Implementation of the movewindowsystem C-ABI integration bridge
//           repositioning windows centered at current mouse cursor coordinates.
// ==============================================================================

#include "move_window_integration.h" // Include move window integration header

// Declare external C ABI export function from move_window.dll
extern "C" { // Begin C linkage declaration block
    int move_window_to_cursor(HWND hwnd, int mode, int offset_x, int offset_y, int clamp_to_screen); // External C ABI function prototype
} // End C linkage declaration block

bool moveWindowToCursor(HWND hwnd) { // Define moveWindowToCursor implementation
    if (!hwnd || !IsWindow(hwnd)) return false; // Verify that window handle is valid
    // Mode 0 = ALIGN_CENTER, offset_x = 0, offset_y = 0, clamp_to_screen = 1
    int result = move_window_to_cursor(hwnd, 0, 0, 0, 1); // Move window centered to cursor with screen edge clamping
    return (result == 0); // Return true if operation completed successfully
} // Close moveWindowToCursor function
