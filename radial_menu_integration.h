// ==============================================================================
// File: .//Description-of-file/radial_menu_integration.md
// Overview: Header for integrating the 6-sector right-click gesture radial menu
//           with the PDF bookshelf application.
// ==============================================================================

#pragma once

#include <windows.h>
#include <string>
#include <vector>

// Initializes the 6-sector gesture radial menu and links callbacks to the main window.
bool initRadialMenuIntegration(HWND hwndOverlay);

// Stops mouse hook and releases radial menu resources.
void shutdownRadialMenuIntegration(void);

// Discovers all valid PDF content folders in the workspace.
std::vector<std::wstring> discoverPdfFolders(void);
