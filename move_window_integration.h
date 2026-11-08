// ==============================================================================
// File: .//Description-of-file/move_window_integration.md
// Overview: Header for integrating movewindowsystem library to reposition windows to cursor.
// ==============================================================================

#pragma once // Prevent duplicate header inclusion

#include <windows.h> // Include core Windows API definitions

// Repositions the given window so that its center aligns with the current mouse cursor,
// with automatic screen boundary clamping.
bool moveWindowToCursor(HWND hwnd); // Declare moveWindowToCursor function
