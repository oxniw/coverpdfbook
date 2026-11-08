// ==============================================================================
// File: .//Description-of-file/handle_user_input.md
// Overview: Implements user input handlers, keyboard shortcuts, mouse dragging,
//           hover detection, click-through toggling, and opening PDFs via shell execution.
// ==============================================================================

#include "handle_user_input.h"
#include <windows.h>
#include <shellapi.h>
#include <algorithm>

int hitTestGridItem(int x, int y, const GridLayoutResult& layout, const std::vector<RECT>& itemRects) {
    POINT pt = { x, y };
    for (int i = 0; i < layout.totalVisible && i < (int)itemRects.size(); ++i) {
        if (PtInRect(&itemRects[i], pt)) {
            return layout.startIndex + i;
        }
    }
    return -1;
}

void handleOverlayMouseDown(HWND hwndOverlay, int x, int y, AppSettings& settings, const GridLayoutResult& layout, const std::vector<RECT>& itemRects) {
    int hit = hitTestGridItem(x, y, layout, itemRects);
    if (hit == -1) {
        // Clicked background - initiate window dragging
        settings.isDragging = true;
        SetCapture(hwndOverlay);
        GetCursorPos(&settings.dragStartPoint);
    }
}

bool handleOverlayMouseMove(
    HWND hwndOverlay,
    int x,
    int y,
    AppSettings& settings,
    const std::vector<PdfItem>& items,
    const GridLayoutResult& layout,
    const std::vector<RECT>& itemRects
) {
    if (settings.isDragging) {
        POINT currentPos;
        GetCursorPos(&currentPos);

        int dx = currentPos.x - settings.dragStartPoint.x;
        int dy = currentPos.y - settings.dragStartPoint.y;

        if (dx != 0 || dy != 0) {
            RECT rcWnd;
            GetWindowRect(hwndOverlay, &rcWnd);
            SetWindowPos(hwndOverlay, NULL, rcWnd.left + dx, rcWnd.top + dy, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
            settings.dragStartPoint = currentPos;
        }
        return false;
    }

    int hit = hitTestGridItem(x, y, layout, itemRects);
    if (hit != settings.hoveredIndex) {
        settings.hoveredIndex = hit;
        return true; // Signals redraw needed for hover highlight
    }
    return false;
}

void handleOverlayMouseUp(HWND hwndOverlay, AppSettings& settings) {
    if (settings.isDragging) {
        settings.isDragging = false;
        ReleaseCapture();
    }
}

bool handleOverlayDoubleClick(int x, int y, const std::vector<PdfItem>& items, const GridLayoutResult& layout, const std::vector<RECT>& itemRects, std::wstring& outNewFolderPath) { // Define handleOverlayDoubleClick implementation
    int hit = hitTestGridItem(x, y, layout, itemRects); // Perform hit test against active grid item bounding boxes
    if (hit >= 0 && hit < (int)items.size()) { // Validate hit index within items range
        const PdfItem& item = items[hit]; // Retrieve clicked item reference
        if (item.type == ITEM_TYPE_FOLDER || item.type == ITEM_TYPE_PARENT) { // Check if clicked item is folder or parent navigation
            outNewFolderPath = item.filePath; // Store destination folder path for caller navigation
            return true; // Return true indicating folder navigation triggered
        } else { // Standard PDF document item clicked
            outNewFolderPath.clear(); // Clear folder path output
            openPdfFile(item.filePath); // Launch PDF file in default Windows viewer
            return true; // Return true indicating PDF execution handled
        } // Close item type condition
    } // Close bounds check
    return false; // Return false if no item was hit
}

void toggleClickThrough(HWND hwndOverlay, AppSettings& settings) {
    settings.clickThrough = !settings.clickThrough;
    LONG_PTR exStyle = GetWindowLongPtr(hwndOverlay, GWL_EXSTYLE);
    if (settings.clickThrough) {
        SetWindowLongPtr(hwndOverlay, GWL_EXSTYLE, exStyle | WS_EX_TRANSPARENT);
    } else {
        SetWindowLongPtr(hwndOverlay, GWL_EXSTYLE, exStyle & ~WS_EX_TRANSPARENT);
    }
}

void openPdfFile(const std::wstring& filePath) {
    ShellExecuteW(NULL, L"open", filePath.c_str(), NULL, NULL, SW_SHOWNORMAL);
}

bool handleAppKeyCommand(WPARAM wParam, AppSettings& settings, int totalItems, int totalPages, HWND hwndOverlay, HWND hwndPanel) {
    bool stateChanged = false;

    switch (wParam) {
        // Size adjustments (+ / -)
        case VK_ADD:
        case VK_OEM_PLUS: {
            if (settings.coverWidth < 420) {
                settings.coverWidth += 15;
                settings.coverHeight = (int)(settings.coverWidth * 1.388f);
                stateChanged = true;
            }
            break;
        }
        case VK_SUBTRACT:
        case VK_OEM_MINUS: {
            if (settings.coverWidth > 75) {
                settings.coverWidth -= 15;
                settings.coverHeight = (int)(settings.coverWidth * 1.388f);
                stateChanged = true;
            }
            break;
        }

        // Columns adjustments (N)
        case VK_OEM_6: // ']'
        case 'N': {
            if (settings.cols < 10) {
                settings.cols++;
                stateChanged = true;
            }
            break;
        }
        case VK_OEM_4: // '['
        case 'B': {
            if (settings.cols > 1) {
                settings.cols--;
                stateChanged = true;
            }
            break;
        }

        // Rows adjustments (M)
        case VK_UP: {
            if (settings.rows < 8) {
                settings.rows++;
                stateChanged = true;
            }
            break;
        }
        case VK_DOWN: {
            if (settings.rows > 1) {
                settings.rows--;
                stateChanged = true;
            }
            break;
        }

        // Pagination (Left / Right Arrow, PageUp / PageDown)
        case VK_LEFT:
        case VK_PRIOR: {
            PostMessage(hwndOverlay, WM_COMMAND, MAKEWPARAM(IDC_BTN_PREV_PAGE, 0), 0);
            break;
        }
        case VK_RIGHT:
        case VK_NEXT: {
            PostMessage(hwndOverlay, WM_COMMAND, MAKEWPARAM(IDC_BTN_NEXT_PAGE, 0), 0);
            break;
        }

        // Toggle Click-Through
        case 'T':
        case VK_F8: {
            toggleClickThrough(hwndOverlay, settings);
            stateChanged = true;
            break;
        }

        // Cycle Grid Alignment (Top-Left, Center, Top-Right, etc.)
        case 'A': {
            settings.alignment = (GridAlignment)((settings.alignment + 1) % ALIGN_COUNT);
            stateChanged = true;
            break;
        }

        // Switch PDF Folder (pdf, tgat, tpat, other, asd)
        case 'F': {
            if (!settings.availableFolders.empty()) {
                settings.currentFolderIndex = (settings.currentFolderIndex + 1) % settings.availableFolders.size();
                settings.pdfDirectory = settings.availableFolders[settings.currentFolderIndex];
                PostMessage(hwndOverlay, WM_COMMAND, MAKEWPARAM(IDC_BTN_RELOAD, 0), 0);
            }
            break;
        }

        // Toggle PDF Covers Overlay Visibility
        case 'V': {
            settings.coversVisible = !settings.coversVisible;
            if (hwndOverlay && IsWindow(hwndOverlay)) {
                ShowWindow(hwndOverlay, settings.coversVisible ? SW_SHOW : SW_HIDE);
                if (settings.coversVisible) {
                    stateChanged = true;
                }
            }
            break;
        }

        // Toggle Floating Control Panel visibility
        case 'H':
        case VK_F1: {
            if (hwndPanel && IsWindow(hwndPanel)) {
                BOOL isVis = IsWindowVisible(hwndPanel);
                ShowWindow(hwndPanel, isVis ? SW_HIDE : SW_SHOW);
            }
            break;
        }

        // Close Application
        case VK_ESCAPE: {
            PostQuitMessage(0);
            return false;
        }
    }

    return stateChanged;
}
