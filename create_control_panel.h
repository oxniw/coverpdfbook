// ==============================================================================
// File: .//Description-of-file/create_control_panel.md
// Overview: Header for creating and updating the floating Win32 control toolbar window.
// ==============================================================================

#pragma once

#include "config.h"

// Creates the floating modeless control panel window containing controls for
// grid rows (M), columns (N), cover sizes, opacity, pagination, and click-through.
HWND createControlPanelWindow(HINSTANCE hInstance, HWND hwndOverlay);

// Updates the display labels on the control panel to mirror current settings.
void updateControlPanelUI(HWND hwndPanel, const AppSettings& settings, int totalItems, int totalPages);
