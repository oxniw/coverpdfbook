// ==============================================================================
// File: .//Description-of-file/handle_user_input.md
// Overview: Header for handling mouse interactions, window dragging, tooltips,
//           keyboard shortcuts, and PDF execution.
// ==============================================================================

#pragma once

#include "config.h"
#include <vector>

// Checks which PDF item contains the point (x, y) on the active page.
// Returns -1 if no item is hit.
int hitTestGridItem(int x, int y, const GridLayoutResult& layout, const std::vector<RECT>& itemRects);

// Handles mouse down events for drag initiation and item selection.
void handleOverlayMouseDown(HWND hwndOverlay, int x, int y, AppSettings& settings, const GridLayoutResult& layout, const std::vector<RECT>& itemRects);

// Handles mouse movement for smooth window dragging and hover highlights.
bool handleOverlayMouseMove(HWND hwndOverlay, int x, int y, AppSettings& settings, const std::vector<PdfItem>& items, const GridLayoutResult& layout, const std::vector<RECT>& itemRects);

// Handles mouse button release to finalize dragging.
void handleOverlayMouseUp(HWND hwndOverlay, AppSettings& settings);

// Handles double click to execute PDF document or navigate into folder.
// If a folder or parent entry was clicked, outNewFolderPath is populated.
bool handleOverlayDoubleClick(int x, int y, const std::vector<PdfItem>& items, const GridLayoutResult& layout, const std::vector<RECT>& itemRects, std::wstring& outNewFolderPath); // Declare handleOverlayDoubleClick function

// Handles keyboard shortcuts for resizing, layout adjustments, and navigation.
bool handleAppKeyCommand(WPARAM wParam, AppSettings& settings, int totalItems, int totalPages, HWND hwndOverlay, HWND hwndPanel);

// Toggles mouse click-through transparency mode.
void toggleClickThrough(HWND hwndOverlay, AppSettings& settings);

// Launches the PDF using Windows default viewer.
void openPdfFile(const std::wstring& filePath);
