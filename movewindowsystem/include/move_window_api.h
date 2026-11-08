// ==============================================================================
// File: .//Description-of-file/move_window_api_h.md
// Overview: Public C ABI export header for the move-window-api library.
// ==============================================================================

#ifndef MOVE_WINDOW_API_H // Guard against multiple inclusions
#define MOVE_WINDOW_API_H // Define macro for header guard

#include "config.h" // Include shared constants and enums
#include <windows.h> // Include Windows standard handle definitions

#ifdef __cplusplus // Check if compiling under C++ compiler
extern "C" { // Enable C linkage for C++ callers
#endif // End of C++ language check

#ifdef BUILDING_MOVE_WINDOW_DLL // Check if currently compiling the shared library
#define MOVE_WINDOW_EXPORT __declspec(dllexport) // Export symbol when building DLL
#else // When importing from client application
#define MOVE_WINDOW_EXPORT __declspec(dllimport) // Import symbol when linking against DLL
#endif // End of building DLL check

// Core function to move a window by handle to the current mouse cursor location
MOVE_WINDOW_EXPORT int move_window_to_cursor( // Declare core window repositioning function
    HWND hwnd, // Target window handle
    AlignmentMode mode, // Anchor alignment mode
    int offset_x, // Custom X pixel offset
    int offset_y, // Custom Y pixel offset
    int clamp_to_screen // Boolean flag to clamp window inside screen bounds
); // End of function declaration

// Moves a window found by title substring to current cursor position
MOVE_WINDOW_EXPORT int move_window_by_title( // Declare move by title substring function
    const char* title_substring, // Case-insensitive substring to match window title
    AlignmentMode mode, // Anchor alignment mode
    int offset_x, // Custom X pixel offset
    int offset_y, // Custom Y pixel offset
    int clamp_to_screen // Boolean flag to clamp window inside screen bounds
); // End of function declaration

// Moves a window found by process image name to current cursor position
MOVE_WINDOW_EXPORT int move_window_by_process_name( // Declare move by process name function
    const char* process_name, // Process executable name (e.g., notepad.exe)
    AlignmentMode mode, // Anchor alignment mode
    int offset_x, // Custom X pixel offset
    int offset_y, // Custom Y pixel offset
    int clamp_to_screen // Boolean flag to clamp window inside screen bounds
); // End of function declaration

// Moves the currently active foreground window to current cursor position
MOVE_WINDOW_EXPORT int move_active_window_to_cursor( // Declare move active window function
    AlignmentMode mode, // Anchor alignment mode
    int offset_x, // Custom X pixel offset
    int offset_y, // Custom Y pixel offset
    int clamp_to_screen // Boolean flag to clamp window inside screen bounds
); // End of function declaration

// Locates first top-level visible window containing title substring
MOVE_WINDOW_EXPORT HWND find_window_by_title_substring( // Declare window lookup by title
    const char* title_substring // Search text
); // End of function declaration

// Locates first top-level visible window matching process name
MOVE_WINDOW_EXPORT HWND find_window_by_process_name( // Declare window lookup by process
    const char* process_name // Target executable name
); // End of function declaration

// Returns the handle of the current foreground window
MOVE_WINDOW_EXPORT HWND get_foreground_window_handle(void); // Declare foreground handle getter

// Queries current mouse cursor position
MOVE_WINDOW_EXPORT int get_cursor_coordinates(int* out_x, int* out_y); // Declare cursor coordinates getter

// Queries position and dimensions of a given window handle
MOVE_WINDOW_EXPORT int get_window_bounds( // Declare window bounds getter
    HWND hwnd, // Window handle
    int* out_x, // Pointer to receive X coordinate
    int* out_y, // Pointer to receive Y coordinate
    int* out_w, // Pointer to receive width
    int* out_h // Pointer to receive height
); // End of function declaration

#ifdef __cplusplus // Check if compiling under C++ compiler
} // Close C linkage block
#endif // End of C++ language check

#endif // Close header guard MOVE_WINDOW_API_H
