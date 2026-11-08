// ==============================================================================
// File: .//Description-of-file/main.md
// Overview: Application entry point orchestrating lifecycle, windows, input loops, and teardown.
// ==============================================================================

#include "config.h"
#include "scan_pdf_directory.h"
#include "extract_pdf_cover.h"
#include "calculate_grid_layout.h"
#include "render_overlay_canvas.h"
#include "page_animation_manager.h"
#include "create_control_panel.h"
#include "handle_user_input.h"
#include "radial_menu_integration.h"
#include "path_utils.h"
#include "move_window_integration.h"
#include "preload_manager.h"

#include <windows.h>
#include <shellapi.h>
#include <gdiplus.h>
#include <vector>
#include <algorithm>

#define WM_APP_TRAY (WM_APP + 101)

namespace {
    AppSettings g_settings;
    std::vector<PdfItem> g_items;
    std::vector<RECT> g_itemRects;
    GridLayoutResult g_layout;
    PageAnimationState g_animState;
    HWND g_hwndOverlay = nullptr;
    HWND g_hwndPanel = nullptr;
    ULONG_PTR g_gdiplusToken = 0;
    NOTIFYICONDATAW g_nid = { sizeof(NOTIFYICONDATAW) };

    void refreshDisplay() {
        if (!g_hwndOverlay || !IsWindow(g_hwndOverlay)) return;

        RECT rc;
        GetClientRect(g_hwndOverlay, &rc);
        int winW = rc.right - rc.left;
        int winH = rc.bottom - rc.top;

        // Recalculate layout
        g_layout = calculateGridLayout(g_settings, (int)g_items.size(), winW, winH, g_itemRects);

        // Clamp current page in bounds
        if (g_settings.currentPage >= g_layout.totalPages) {
            g_settings.currentPage = (g_layout.totalPages > 0) ? (g_layout.totalPages - 1) : 0;
            g_layout = calculateGridLayout(g_settings, (int)g_items.size(), winW, winH, g_itemRects);
        }

        // Render layered window with page transition animation state
        renderOverlayCanvas(g_hwndOverlay, g_settings, g_items, g_layout, g_itemRects, &g_animState);

        // Update control panel stats
        updateControlPanelUI(g_hwndPanel, g_settings, (int)g_items.size(), g_layout.totalPages); // Update floating toolbar statistics

        // Post non-blocking message to quietly preload adjacent pages while user is idle
        PostMessage(g_hwndOverlay, WM_APP_PRELOAD_ADJACENT, 0, 0); // Asynchronously preload next & previous page
    }

    void triggerPageTransition(int newPage, AnimationDirection dir) {
        if (g_layout.totalPages <= 1) return;
        if (newPage < 0 || newPage >= g_layout.totalPages) return;

        if (g_animState.isAnimating) {
            g_settings.currentPage = g_animState.toPage;
        }

        if (newPage == g_settings.currentPage) return;

        int fromPage = g_settings.currentPage;
        startPageAnimation(g_animState, fromPage, newPage, dir);
        if (g_hwndOverlay && IsWindow(g_hwndOverlay)) {
            SetTimer(g_hwndOverlay, TIMER_ANIMATION_ID, 16, NULL);
        }
        refreshDisplay();
    }

    void reloadPdfFolder() {
        // Stop any active animation
        if (g_hwndOverlay && IsWindow(g_hwndOverlay)) {
            KillTimer(g_hwndOverlay, TIMER_ANIMATION_ID);
        }
        g_animState.isAnimating = false;
        invalidatePageCache();

        // Free old thumbnails
        for (auto& item : g_items) {
            freePdfThumbnail(item.thumbnail);
        }
        g_items.clear();

        // Trigger non-blocking background batch extraction of page 1 covers
        triggerPdfCoverBatchExtraction(g_settings.pdfDirectory, g_hwndOverlay); // Asynchronously extract covers without freezing UI

        // Scan folder again
        g_items = scanPdfDirectory(g_settings.pdfDirectory);
        g_settings.currentPage = 0;
        g_settings.hoveredIndex = -1;

        refreshDisplay();
    }

    LRESULT CALLBACK OverlayWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
        switch (uMsg) {
            case WM_LBUTTONDOWN: {
                int x = (short)LOWORD(lParam);
                int y = (short)HIWORD(lParam);
                handleOverlayMouseDown(hwnd, x, y, g_settings, g_layout, g_itemRects);
                return 0;
            }
            case WM_MOUSEMOVE: {
                int x = (short)LOWORD(lParam);
                int y = (short)HIWORD(lParam);
                if (handleOverlayMouseMove(hwnd, x, y, g_settings, g_items, g_layout, g_itemRects)) {
                    refreshDisplay();
                }
                return 0;
            }
            case WM_LBUTTONUP: {
                handleOverlayMouseUp(hwnd, g_settings);
                return 0;
            }
            case WM_LBUTTONDBLCLK: {
                int x = (short)LOWORD(lParam); // Extract cursor X coordinate from message parameter
                int y = (short)HIWORD(lParam); // Extract cursor Y coordinate from message parameter
                std::wstring newFolder; // Buffer to receive folder path if directory was double-clicked
                if (handleOverlayDoubleClick(x, y, g_items, g_layout, g_itemRects, newFolder)) { // Process double click event
                    if (!newFolder.empty()) { // Check if double click resulted in folder navigation request
                        g_settings.pdfDirectory = normalizePath(newFolder); // Update active browsing directory path
                        reloadPdfFolder(); // Reload directory contents and redraw bookshelf
                    } // Close if statement
                } // Close if statement
                return 0; // Return handled message status
            }
            case WM_TIMER: {
                if (wParam == TIMER_ANIMATION_ID) {
                    if (updatePageAnimation(g_animState)) {
                        refreshDisplay();
                    } else {
                        KillTimer(hwnd, TIMER_ANIMATION_ID);
                        g_settings.currentPage = g_animState.toPage;
                        refreshDisplay();
                    }
                    return 0;
                }
                break;
            }
            case WM_MOUSEWHEEL: {
                short zDelta = (short)HIWORD(wParam);
                if (zDelta > 0) {
                    triggerPageTransition(g_settings.currentPage - 1, ANIM_DIR_PREV);
                } else if (zDelta < 0) {
                    triggerPageTransition(g_settings.currentPage + 1, ANIM_DIR_NEXT);
                }
                return 0;
            }
            case WM_KEYDOWN: {
                if (wParam == 'R' || wParam == VK_F5) {
                    reloadPdfFolder();
                } else if (handleAppKeyCommand(wParam, g_settings, (int)g_items.size(), g_layout.totalPages, hwnd, g_hwndPanel)) {
                    refreshDisplay();
                }
                return 0;
            }
            case WM_HOTKEY: {
                if (wParam == HK_TOGGLE_CLICKTHROUGH) {
                    toggleClickThrough(hwnd, g_settings);
                    refreshDisplay();
                } else if (wParam == HK_TOGGLE_PANEL) {
                    if (g_hwndPanel && IsWindow(g_hwndPanel)) {
                        BOOL isVis = IsWindowVisible(g_hwndPanel);
                        ShowWindow(g_hwndPanel, isVis ? SW_HIDE : SW_SHOW);
                    }
                } else if (wParam == HK_TOGGLE_COVERS) {
                    g_settings.coversVisible = !g_settings.coversVisible;
                    if (g_hwndOverlay && IsWindow(g_hwndOverlay)) {
                        ShowWindow(g_hwndOverlay, g_settings.coversVisible ? SW_SHOW : SW_HIDE);
                        if (g_settings.coversVisible) refreshDisplay();
                    }
                    updateControlPanelUI(g_hwndPanel, g_settings, (int)g_items.size(), g_layout.totalPages);
                } else if (wParam == HK_RELOAD_PDFS) {
                    reloadPdfFolder();
                }
                return 0;
            }
            case WM_COMMAND: {
                int id = LOWORD(wParam);
                switch (id) {
                    case IDC_BTN_ROW_INC:
                        if (g_animState.isAnimating) {
                            KillTimer(hwnd, TIMER_ANIMATION_ID);
                            g_animState.isAnimating = false;
                            g_settings.currentPage = g_animState.toPage;
                        }
                        if (g_settings.rows < 8) g_settings.rows++;
                        break;
                    case IDC_BTN_ROW_DEC:
                        if (g_animState.isAnimating) {
                            KillTimer(hwnd, TIMER_ANIMATION_ID);
                            g_animState.isAnimating = false;
                            g_settings.currentPage = g_animState.toPage;
                        }
                        if (g_settings.rows > 1) g_settings.rows--;
                        break;
                    case IDC_BTN_COL_INC:
                        if (g_animState.isAnimating) {
                            KillTimer(hwnd, TIMER_ANIMATION_ID);
                            g_animState.isAnimating = false;
                            g_settings.currentPage = g_animState.toPage;
                        }
                        if (g_settings.cols < 10) g_settings.cols++;
                        break;
                    case IDC_BTN_COL_DEC:
                        if (g_animState.isAnimating) {
                            KillTimer(hwnd, TIMER_ANIMATION_ID);
                            g_animState.isAnimating = false;
                            g_settings.currentPage = g_animState.toPage;
                        }
                        if (g_settings.cols > 1) g_settings.cols--;
                        break;
                    case IDC_BTN_SIZE_INC:
                        if (g_animState.isAnimating) {
                            KillTimer(hwnd, TIMER_ANIMATION_ID);
                            g_animState.isAnimating = false;
                            g_settings.currentPage = g_animState.toPage;
                        }
                        if (g_settings.coverWidth < 420) {
                            g_settings.coverWidth += 20;
                            g_settings.coverHeight = (int)(g_settings.coverWidth * 1.388f);
                        }
                        break;
                    case IDC_BTN_SIZE_DEC:
                        if (g_animState.isAnimating) {
                            KillTimer(hwnd, TIMER_ANIMATION_ID);
                            g_animState.isAnimating = false;
                            g_settings.currentPage = g_animState.toPage;
                        }
                        if (g_settings.coverWidth > 75) {
                            g_settings.coverWidth -= 20;
                            g_settings.coverHeight = (int)(g_settings.coverWidth * 1.388f);
                        }
                        break;
                    case IDC_BTN_OPACITY_INC:
                        if (g_settings.overlayAlpha <= 235) g_settings.overlayAlpha += 20;
                        else g_settings.overlayAlpha = 255;
                        break;
                    case IDC_BTN_OPACITY_DEC:
                        if (g_settings.overlayAlpha >= 70) g_settings.overlayAlpha -= 20;
                        break;
                    case IDC_BTN_ALIGN_PREV:
                        if (g_animState.isAnimating) {
                            KillTimer(hwnd, TIMER_ANIMATION_ID);
                            g_animState.isAnimating = false;
                            g_settings.currentPage = g_animState.toPage;
                        }
                        if (g_settings.alignment < 0) g_settings.alignment = (GridAlignment)(ALIGN_COUNT - 1);
                        break;
                    case IDC_BTN_ALIGN_NEXT:
                        if (g_animState.isAnimating) {
                            KillTimer(hwnd, TIMER_ANIMATION_ID);
                            g_animState.isAnimating = false;
                            g_settings.currentPage = g_animState.toPage;
                        }
                        g_settings.alignment = (GridAlignment)((g_settings.alignment + 1) % ALIGN_COUNT);
                        break;
                    case IDC_BTN_PREV_PAGE:
                        triggerPageTransition(g_settings.currentPage - 1, ANIM_DIR_PREV);
                        return 0;
                    case IDC_BTN_NEXT_PAGE:
                        triggerPageTransition(g_settings.currentPage + 1, ANIM_DIR_NEXT);
                        return 0;
                    case IDC_BTN_TOGGLE_CLICKTHROUGH:
                        toggleClickThrough(hwnd, g_settings);
                        break;
                    case IDC_BTN_TOGGLE_COVERS:
                        g_settings.coversVisible = !g_settings.coversVisible;
                        if (g_hwndOverlay && IsWindow(g_hwndOverlay)) {
                            ShowWindow(g_hwndOverlay, g_settings.coversVisible ? SW_SHOW : SW_HIDE);
                            if (g_settings.coversVisible) refreshDisplay();
                        }
                        updateControlPanelUI(g_hwndPanel, g_settings, (int)g_items.size(), g_layout.totalPages);
                        return 0;
                    case IDC_BTN_SWITCH_FOLDER:
                        if (!g_settings.availableFolders.empty()) {
                            g_settings.currentFolderIndex = (g_settings.currentFolderIndex + 1) % g_settings.availableFolders.size();
                            g_settings.pdfDirectory = g_settings.availableFolders[g_settings.currentFolderIndex];
                            reloadPdfFolder();
                        }
                        return 0;
                    case IDC_BTN_RELOAD:
                        reloadPdfFolder();
                        return 0;
                    case IDC_BTN_HIDE_PANEL:
                        if (g_hwndPanel && IsWindow(g_hwndPanel)) {
                            ShowWindow(g_hwndPanel, SW_HIDE);
                        }
                        return 0;
                    case IDC_CMD_OPEN_SETTINGS_AT_CURSOR:
                        if (g_hwndPanel && IsWindow(g_hwndPanel)) { // Ensure settings panel window exists
                            BOOL isVis = IsWindowVisible(g_hwndPanel); // Query current visibility state of panel
                            BOOL isMin = IsIconic(g_hwndPanel); // Check if panel is currently minimized to taskbar
                            if (isVis && !isMin) { // Check if settings window is currently active and visible
                                ShowWindow(g_hwndPanel, SW_HIDE); // Hide settings panel window
                            } else { // Settings window is currently hidden or minimized
                                ShowWindow(g_hwndPanel, SW_SHOWNORMAL); // Restore and display settings panel
                                moveWindowToCursor(g_hwndPanel); // Move setting window directly to mouse cursor position
                                SetForegroundWindow(g_hwndPanel); // Bring setting window into active focus
                            } // Close visibility toggle condition
                        } // Close if statement
                        return 0; // Return handled message status
                    case IDC_BTN_CLOSE_APP:
                        PostQuitMessage(0);
                        return 0;
                }
                invalidatePageCache();
                refreshDisplay();
                return 0;
            }
            case WM_APP_PRELOAD_ADJACENT: {
                preloadAdjacentPages(g_items, g_settings, g_layout); // Quietly preload adjacent pages into memory while idle
                return 0; // Return handled message status
            }
            case WM_APP_CACHE_UPDATED: {
                // Free stale thumbnails on visible page and refresh now that retina PNGs are available
                for (int i = g_layout.startIndex; i < g_layout.endIndex && i < (int)g_items.size(); ++i) { // Loop visible items
                    if (g_items[i].type == ITEM_TYPE_PDF) { // Check if item is PDF document
                        freePdfThumbnail(g_items[i].thumbnail); // Invalidate placeholder thumbnail
                    } // Close if statement
                } // Close item loop
                refreshDisplay(); // Redraw overlay with freshly rasterized retina covers
                return 0; // Return handled message status
            }
            case WM_APP_TRAY: {
                if (lParam == WM_LBUTTONUP || lParam == WM_RBUTTONUP) {
                    if (g_hwndPanel && IsWindow(g_hwndPanel)) {
                        BOOL isVis = IsWindowVisible(g_hwndPanel);
                        ShowWindow(g_hwndPanel, isVis ? SW_HIDE : SW_SHOW);
                        if (!isVis) {
                            SetForegroundWindow(g_hwndPanel);
                        }
                    }
                }
                return 0;
            }
            case WM_DESTROY: {
                PostQuitMessage(0);
                return 0;
            }
        }
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR, int nCmdShow) {
    // 1. Initialize COM Apartment for Shell thumbnail interfaces
    CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);

    // 2. Initialize GDI+ subsystem
    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    Gdiplus::GdiplusStartup(&g_gdiplusToken, &gdiplusStartupInput, NULL);

    // 3. Register Overlay Window Class
    WNDCLASSEX wc = { 0 };
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_DBLCLKS;
    wc.lpfnWndProc = OverlayWndProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.lpszClassName = OVERLAY_WINDOW_CLASS;
    RegisterClassEx(&wc);

    // 4. Set default browsing path to Windows Desktop and perform initial scan
    g_settings.pdfDirectory = getDesktopDirectoryPath(); // Initialize default path to Windows Desktop
    g_settings.availableFolders = discoverPdfFolders(); // Discover relative PDF folders in app root
    g_items = scanPdfDirectory(g_settings.pdfDirectory); // Scan desktop directory for folders and PDFs

    // 5. Create full desktop screen uninteractable transparent overlay window
    int screenW = GetSystemMetrics(SM_CXSCREEN); // Query screen pixel width
    int screenH = GetSystemMetrics(SM_CYSCREEN); // Query screen pixel height

    g_hwndOverlay = CreateWindowEx( // Create layered overlay window
        WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_TRANSPARENT, // Window styles for click-through topmost layer
        OVERLAY_WINDOW_CLASS, // Registered overlay window class
        OVERLAY_WINDOW_TITLE, // Overlay window caption
        WS_POPUP, // Popup window style
        0, 0, screenW, screenH, // Span full desktop display dimensions
        NULL, NULL, hInstance, NULL // Window creation parameters
    ); // End CreateWindowEx call

    if (!g_hwndOverlay) { // Verify overlay window creation
        Gdiplus::GdiplusShutdown(g_gdiplusToken); // Shutdown GDI+ subsystem
        CoUninitialize(); // Uninitialize COM library
        return 1; // Return failure code
    } // Close if statement

    // Trigger non-blocking background batch extraction with completion notification to overlay
    triggerPdfCoverBatchExtraction(g_settings.pdfDirectory, g_hwndOverlay); // Asynchronously pre-rasterize covers without UI stall

    // Initialize 6-sector right-click gesture radial menu
    initRadialMenuIntegration(g_hwndOverlay);

    // 6. Create floating Control Panel
    g_hwndPanel = createControlPanelWindow(hInstance, g_hwndOverlay);

    // 7. Register Global Hotkeys (Ctrl+Alt+T, Ctrl+Alt+H, Ctrl+Alt+V, Ctrl+Alt+R)
    RegisterHotKey(g_hwndOverlay, HK_TOGGLE_CLICKTHROUGH, MOD_CONTROL | MOD_ALT, 'T');
    RegisterHotKey(g_hwndOverlay, HK_TOGGLE_PANEL, MOD_CONTROL | MOD_ALT, 'H');
    RegisterHotKey(g_hwndOverlay, HK_TOGGLE_COVERS, MOD_CONTROL | MOD_ALT, 'V');
    RegisterHotKey(g_hwndOverlay, HK_RELOAD_PDFS, MOD_CONTROL | MOD_ALT, 'R');

    // 8. Register System Tray Icon for easy access when panel is hidden
    g_nid.hWnd = g_hwndOverlay;
    g_nid.uID = 1001;
    g_nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    g_nid.uCallbackMessage = WM_APP_TRAY;
    g_nid.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wcscpy_s(g_nid.szTip, L"PDF Bookshelf (Click to show/hide controls)");
    Shell_NotifyIconW(NIM_ADD, &g_nid);

    // 9. Show overlay and trigger initial layout & paint
    ShowWindow(g_hwndOverlay, SW_SHOW);
    refreshDisplay();

    // 10. Standard Windows Message Loop
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // 11. Clean up resources
    shutdownRadialMenuIntegration();
    Shell_NotifyIconW(NIM_DELETE, &g_nid);
    UnregisterHotKey(g_hwndOverlay, HK_TOGGLE_CLICKTHROUGH);
    UnregisterHotKey(g_hwndOverlay, HK_TOGGLE_PANEL);
    UnregisterHotKey(g_hwndOverlay, HK_TOGGLE_COVERS);
    UnregisterHotKey(g_hwndOverlay, HK_RELOAD_PDFS);

    cleanupRenderResources();

    for (auto& item : g_items) {
        freePdfThumbnail(item.thumbnail);
    }
    g_items.clear();

    Gdiplus::GdiplusShutdown(g_gdiplusToken);
    CoUninitialize();

    return (int)msg.wParam;
}

// Console/standard entry point bridge if compiled as standard console app
int main() {
    return wWinMain(GetModuleHandle(NULL), NULL, GetCommandLineW(), SW_SHOWNORMAL);
}
