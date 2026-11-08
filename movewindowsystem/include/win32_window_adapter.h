// ==============================================================================
// File: .//Description-of-file/win32_window_adapter_h.md
// Overview: Win32 OS adapter layer isolating Windows user32 APIs for cursor, monitor, and window manipulation.
// ==============================================================================

#ifndef MOVE_WINDOW_WIN32_ADAPTER_H // Guard against multiple inclusions
#define MOVE_WINDOW_WIN32_ADAPTER_H // Define macro for header guard

#include "config.h" // Include configuration and shared types
#include <windows.h> // Include Windows Win32 API definitions

#ifdef __cplusplus // Check if compiling under C++ compiler
extern "C" { // Enable C linkage for C++ callers
#endif // End of C++ language check

// Retrieves current global cursor position
int adapter_get_cursor_position(Point2D* out_pos); // Retrieve cursor screen coordinates

// Retrieves bounding rectangle and dimension of a given window handle
int adapter_get_window_geometry(HWND hwnd, Rect2D* out_geometry); // Retrieve window position and size

// Retrieves bounding rectangle of the monitor containing the specified point
int adapter_get_monitor_geometry(Point2D pt, Rect2D* out_monitor_bounds); // Retrieve monitor display rect

// Sets window position without altering its Z-order or size
int adapter_set_window_position(HWND hwnd, Point2D pos); // Update window on-screen position

// Validates whether an HWND is a valid and visible top-level window
int adapter_is_window_valid(HWND hwnd); // Check window validity and visibility

#ifdef __cplusplus // Check if compiling under C++ compiler
} // Close C linkage block
#endif // End of C++ language check

#endif // Close header guard MOVE_WINDOW_WIN32_ADAPTER_H
