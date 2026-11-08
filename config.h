// ==============================================================================
// File: .//Description-of-file/config.md
// Overview: Shared application structures, defaults, constants, and control identifiers.
// ==============================================================================

#pragma once

#ifndef UNICODE
#define UNICODE
#endif
#ifndef _UNICODE
#define _UNICODE
#endif

#include <windows.h>
#include <wtypes.h>
#include <propidl.h>
#include <gdiplus.h>
#include <string>
#include <vector>

// Window class names and titles
#define OVERLAY_WINDOW_CLASS   L"PdfBookOverlayWindowClass"
#define OVERLAY_WINDOW_TITLE   L"PDF Book Covers Overlay"
#define CONTROL_PANEL_CLASS    L"PdfBookControlPanelClass"
#define CONTROL_PANEL_TITLE    L"Overlay Controls (M*N Grid & Size)"

// Control Identifier Constants for Floating Control Panel
enum ControlIDs {
    IDC_BTN_ROW_DEC = 1001,
    IDC_BTN_ROW_INC,
    IDC_TXT_ROW_VAL,
    IDC_BTN_COL_DEC,
    IDC_BTN_COL_INC,
    IDC_TXT_COL_VAL,
    IDC_BTN_SIZE_DEC,
    IDC_BTN_SIZE_INC,
    IDC_TXT_SIZE_VAL,
    IDC_BTN_OPACITY_DEC,
    IDC_BTN_OPACITY_INC,
    IDC_TXT_OPACITY_VAL,
    IDC_BTN_ALIGN_PREV,
    IDC_BTN_ALIGN_NEXT,
    IDC_TXT_ALIGN_VAL,
    IDC_BTN_PREV_PAGE,
    IDC_BTN_NEXT_PAGE,
    IDC_TXT_PAGE_STATUS,
    IDC_BTN_TOGGLE_CLICKTHROUGH,
    IDC_BTN_TOGGLE_COVERS,
    IDC_BTN_SWITCH_FOLDER,
    IDC_TXT_FOLDER_VAL,
    IDC_BTN_RELOAD,
    IDC_BTN_HIDE_PANEL,
    IDC_BTN_CLOSE_APP,
    IDC_BTN_TOGGLE_HELP,
    IDC_TXT_INFO_STATUS,
    IDC_CMD_OPEN_SETTINGS_AT_CURSOR
};

// Grid Alignment / Start Position on Desktop Screen
enum GridAlignment {
    ALIGN_TOP_LEFT = 0,
    ALIGN_TOP_CENTER,
    ALIGN_TOP_RIGHT,
    ALIGN_MIDDLE_LEFT,
    ALIGN_CENTER,
    ALIGN_MIDDLE_RIGHT,
    ALIGN_BOTTOM_LEFT,
    ALIGN_BOTTOM_CENTER,
    ALIGN_BOTTOM_RIGHT,
    ALIGN_COUNT
};

inline const wchar_t* getAlignmentName(GridAlignment align) {
    switch (align) {
        case ALIGN_TOP_LEFT:      return L"Top-Left";
        case ALIGN_TOP_CENTER:    return L"Top-Center";
        case ALIGN_TOP_RIGHT:     return L"Top-Right";
        case ALIGN_MIDDLE_LEFT:   return L"Mid-Left";
        case ALIGN_CENTER:        return L"Center";
        case ALIGN_MIDDLE_RIGHT:  return L"Mid-Right";
        case ALIGN_BOTTOM_LEFT:   return L"Bottom-Left";
        case ALIGN_BOTTOM_CENTER: return L"Bottom-Center";
        case ALIGN_BOTTOM_RIGHT:  return L"Bottom-Right";
        default:                  return L"Center";
    }
}

// Global Hotkey Identifiers
enum HotkeyIDs {
    HK_TOGGLE_CLICKTHROUGH = 2001,
    HK_TOGGLE_PANEL,
    HK_TOGGLE_COVERS,
    HK_RELOAD_PDFS
};

// Timer Identifiers
enum TimerIDs {
    TIMER_ANIMATION_ID = 3001
};

// Animation Direction & State
enum AnimationDirection {
    ANIM_DIR_NONE = 0,
    ANIM_DIR_NEXT, // Slides to next page (enters from right)
    ANIM_DIR_PREV  // Slides to previous page (enters from left)
};

struct PageAnimationState {
    bool isAnimating = false;
    int fromPage = 0;
    int toPage = 0;
    AnimationDirection direction = ANIM_DIR_NONE;
    DWORD startTimeMs = 0;
    DWORD durationMs = 280; // 280ms duration for snappy, fluid transition
    float progress = 0.0f;
    float easedProgress = 0.0f;
};

// Represents the classification of an item in the bookshelf
enum ItemType {
    ITEM_TYPE_PDF = 0, // PDF document file
    ITEM_TYPE_FOLDER,  // Subdirectory folder
    ITEM_TYPE_PARENT   // Parent directory navigation entry (..)
};

// Represents a single PDF file or folder and its cached cover bitmap
struct PdfItem {
    ItemType type = ITEM_TYPE_PDF; // Item type: PDF document, folder, or parent navigation
    std::wstring filePath;         // Absolute canonical path for filesystem operations
    std::wstring relativePath;     // Relative path relative to current browsing directory
    std::wstring fileName;         // Raw filename or directory name
    std::wstring displayName;      // Formatted display title for cover pill badge
    Gdiplus::Bitmap* thumbnail = nullptr; // Cached cover or folder thumbnail bitmap
    RECT screenRect = {0, 0, 0, 0}; // Bounding rectangle on desktop screen
};

// Application dynamic runtime settings
struct AppSettings {
    int rows = 3;                  // M: Number of rows in grid (1..10)
    int cols = 10;                  // N: Number of columns in grid (1..12)
    int coverWidth = 160;          // Width of each PDF cover card in pixels
    int coverHeight = 250;         // Computed based on aspect ratio (~1.388)
    int spacing = 24;              // Gap between grid cells
    BYTE overlayAlpha = 225;       // Window opacity (50..255)
    bool clickThrough = true;      // Pass clicks directly through to underlying windows (uninteractable)
    bool coversVisible = true;     // Visibility of the PDF covers overlay (true=shown, false=hidden)
    GridAlignment alignment = ALIGN_CENTER; // Anchor position: Top-Left, Center, Top-Right, etc.
    int marginX = 40;              // Screen margin X in pixels
    int marginY = 40;              // Screen margin Y in pixels
    int currentPage = 0;           // 0-based index of current page
    int hoveredIndex = -1;         // Index of currently hovered PDF item in full list (-1 for none)
    std::wstring pdfDirectory = L"pdf"; // Active directory path to scan for PDFs
    std::vector<std::wstring> availableFolders; // Discovered PDF folders (pdf, tgat, tpat, other, asd)
    int currentFolderIndex = 0;    // Index into availableFolders
    bool isDragging = false;       // Window movement state
    POINT dragStartPoint = {0, 0}; // Starting cursor point for dragging overlay
};

// Result metrics from calculating grid layout
struct GridLayoutResult {
    int totalItems = 0;
    int itemsPerPage = 0;
    int totalPages = 1;
    int startIndex = 0;
    int endIndex = 0;
    int totalVisible = 0;
    RECT gridBounds = {0, 0, 0, 0};
};
