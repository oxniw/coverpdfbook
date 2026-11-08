// ==============================================================================
// File: .//Description-of-file/create_control_panel.md
// Overview: Implementation of the Win32 floating control toolbar window.
// ==============================================================================

#include "create_control_panel.h"
#include "path_utils.h"
#include <windows.h>
#include <sstream>

namespace {
    HFONT g_hFontUi = nullptr;

    LRESULT CALLBACK ControlPanelWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
        HWND hwndOverlay = (HWND)GetWindowLongPtr(hwnd, GWLP_USERDATA);

        switch (uMsg) {
            case WM_COMMAND: {
                int id = LOWORD(wParam);
                // Forward commands to overlay window for unified execution
                if (hwndOverlay && IsWindow(hwndOverlay)) {
                    PostMessage(hwndOverlay, WM_COMMAND, wParam, lParam);
                }
                return 0;
            }
            case WM_CLOSE: {
                // Clicking [X] hides the panel; press H or click tray icon to restore
                ShowWindow(hwnd, SW_HIDE);
                return 0;
            }
            case WM_DESTROY: {
                if (g_hFontUi) {
                    DeleteObject(g_hFontUi);
                    g_hFontUi = nullptr;
                }
                return 0;
            }
        }
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
}

HWND createControlPanelWindow(HINSTANCE hInstance, HWND hwndOverlay) {
    WNDCLASSEX wc = { 0 };
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.lpfnWndProc = ControlPanelWndProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
    wc.lpszClassName = CONTROL_PANEL_CLASS;
    RegisterClassEx(&wc);

    // Create tool window positioned nicely on screen
    int screenW = GetSystemMetrics(SM_CXSCREEN);
    int panelW = 345;
    int panelH = 475;
    int panelX = screenW - panelW - 40;
    int panelY = 60;

    HWND hwndPanel = CreateWindowEx(
        WS_EX_TOPMOST | WS_EX_APPWINDOW, // Show as prominent taskbar application window
        CONTROL_PANEL_CLASS, // Registered control panel window class
        CONTROL_PANEL_TITLE, // Title display text
        WS_POPUPWINDOW | WS_CAPTION | WS_MINIMIZEBOX, // Window styles including titlebar and minimize box
        panelX, panelY, panelW, panelH, // Initial coordinates and window dimensions
        NULL, NULL, hInstance, NULL // Window creation parameters
    );

    if (!hwndPanel) return nullptr;

    SetWindowLongPtr(hwndPanel, GWLP_USERDATA, (LONG_PTR)hwndOverlay);

    // Create UI font
    g_hFontUi = CreateFont(
        -13, 0, 0, 0, FW_SEMIBOLD, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Segoe UI"
    );

    auto makeCtrl = [hwndPanel, hInstance](const wchar_t* cls, const wchar_t* txt, DWORD style, int x, int y, int w, int h, int id) -> HWND {
        HWND hCtrl = CreateWindowEx(0, cls, txt, WS_CHILD | WS_VISIBLE | style, x, y, w, h, hwndPanel, (HMENU)(INT_PTR)id, hInstance, NULL);
        if (g_hFontUi) SendMessage(hCtrl, WM_SETFONT, (WPARAM)g_hFontUi, TRUE);
        return hCtrl;
    };

    // Row controls
    makeCtrl(L"STATIC", L"Rows (M):", SS_LEFT, 18, 18, 90, 22, 0);
    makeCtrl(L"BUTTON", L"-", BS_PUSHBUTTON, 115, 15, 34, 26, IDC_BTN_ROW_DEC);
    makeCtrl(L"STATIC", L"2", SS_CENTER, 155, 18, 50, 22, IDC_TXT_ROW_VAL);
    makeCtrl(L"BUTTON", L"+", BS_PUSHBUTTON, 215, 15, 34, 26, IDC_BTN_ROW_INC);

    // Column controls
    makeCtrl(L"STATIC", L"Cols (N):", SS_LEFT, 18, 52, 90, 22, 0);
    makeCtrl(L"BUTTON", L"-", BS_PUSHBUTTON, 115, 49, 34, 26, IDC_BTN_COL_DEC);
    makeCtrl(L"STATIC", L"3", SS_CENTER, 155, 52, 50, 22, IDC_TXT_COL_VAL);
    makeCtrl(L"BUTTON", L"+", BS_PUSHBUTTON, 215, 49, 34, 26, IDC_BTN_COL_INC);

    // Size controls
    makeCtrl(L"STATIC", L"Cover Size:", SS_LEFT, 18, 86, 90, 22, 0);
    makeCtrl(L"BUTTON", L"-", BS_PUSHBUTTON, 115, 83, 34, 26, IDC_BTN_SIZE_DEC);
    makeCtrl(L"STATIC", L"180 px", SS_CENTER, 152, 86, 60, 22, IDC_TXT_SIZE_VAL);
    makeCtrl(L"BUTTON", L"+", BS_PUSHBUTTON, 215, 83, 34, 26, IDC_BTN_SIZE_INC);

    // Opacity controls
    makeCtrl(L"STATIC", L"Opacity:", SS_LEFT, 18, 120, 90, 22, 0);
    makeCtrl(L"BUTTON", L"-", BS_PUSHBUTTON, 115, 117, 34, 26, IDC_BTN_OPACITY_DEC);
    makeCtrl(L"STATIC", L"88%", SS_CENTER, 152, 120, 60, 22, IDC_TXT_OPACITY_VAL);
    makeCtrl(L"BUTTON", L"+", BS_PUSHBUTTON, 215, 117, 34, 26, IDC_BTN_OPACITY_INC);

    // Grid Alignment controls (Top-Left, Center, Top-Right, etc.)
    makeCtrl(L"STATIC", L"Align Grid:", SS_LEFT, 18, 154, 90, 22, 0);
    makeCtrl(L"BUTTON", L"<", BS_PUSHBUTTON, 115, 151, 34, 26, IDC_BTN_ALIGN_PREV);
    makeCtrl(L"STATIC", L"Center", SS_CENTER, 150, 154, 90, 22, IDC_TXT_ALIGN_VAL);
    makeCtrl(L"BUTTON", L">", BS_PUSHBUTTON, 245, 151, 34, 26, IDC_BTN_ALIGN_NEXT);

    // Pagination
    makeCtrl(L"BUTTON", L"<< Prev", BS_PUSHBUTTON, 18, 188, 85, 28, IDC_BTN_PREV_PAGE);
    makeCtrl(L"STATIC", L"Page 1 / 1", SS_CENTER, 110, 193, 105, 22, IDC_TXT_PAGE_STATUS);
    makeCtrl(L"BUTTON", L"Next >>", BS_PUSHBUTTON, 225, 188, 85, 28, IDC_BTN_NEXT_PAGE);

    // Action buttons
    makeCtrl(L"BUTTON", L"Click-Through: [ ENABLED ] (T)", BS_PUSHBUTTON, 18, 226, 292, 30, IDC_BTN_TOGGLE_CLICKTHROUGH);
    makeCtrl(L"BUTTON", L"Hide PDF Covers (V)", BS_PUSHBUTTON, 18, 264, 292, 30, IDC_BTN_TOGGLE_COVERS);
    makeCtrl(L"BUTTON", L"Rescan \"pdf/\" Folder (R)", BS_PUSHBUTTON, 18, 302, 292, 30, IDC_BTN_RELOAD);
    makeCtrl(L"BUTTON", L"Close Program (Esc)", BS_PUSHBUTTON, 18, 340, 292, 30, IDC_BTN_CLOSE_APP);

    // Status / instructions
    makeCtrl(L"STATIC", L"Status: Ready", SS_LEFT, 18, 380, 292, 50, IDC_TXT_INFO_STATUS);

    ShowWindow(hwndPanel, SW_SHOW);
    UpdateWindow(hwndPanel);

    return hwndPanel;
}

void updateControlPanelUI(HWND hwndPanel, const AppSettings& settings, int totalItems, int totalPages) {
    if (!hwndPanel || !IsWindow(hwndPanel)) return;

    // Update Rows
    std::wstring rowStr = std::to_wstring(settings.rows);
    SetDlgItemText(hwndPanel, IDC_TXT_ROW_VAL, rowStr.c_str());

    // Update Cols
    std::wstring colStr = std::to_wstring(settings.cols);
    SetDlgItemText(hwndPanel, IDC_TXT_COL_VAL, colStr.c_str());

    // Update Size
    std::wstring sizeStr = std::to_wstring(settings.coverWidth) + L" px";
    SetDlgItemText(hwndPanel, IDC_TXT_SIZE_VAL, sizeStr.c_str());

    // Update Opacity
    int pct = (int)((settings.overlayAlpha * 100) / 255);
    std::wstring opStr = std::to_wstring(pct) + L"%";
    SetDlgItemText(hwndPanel, IDC_TXT_OPACITY_VAL, opStr.c_str());

    // Update Grid Alignment text (e.g. "Top-Left", "Center", "Top-Right")
    SetDlgItemText(hwndPanel, IDC_TXT_ALIGN_VAL, getAlignmentName(settings.alignment));

    // Update Page
    std::wstringstream pageSs;
    pageSs << L"Page " << (settings.currentPage + 1) << L" / " << totalPages;
    SetDlgItemText(hwndPanel, IDC_TXT_PAGE_STATUS, pageSs.str().c_str());

    // Update Click-through button text
    std::wstring ctBtn = settings.clickThrough ? L"Click-Through: [ ENABLED ] (T)" : L"Click-Through: [ DISABLED ] (T)";
    SetDlgItemText(hwndPanel, IDC_BTN_TOGGLE_CLICKTHROUGH, ctBtn.c_str());

    // Update Hide/Show PDF Covers button text
    std::wstring coversBtn = settings.coversVisible ? L"Hide PDF Covers (V)" : L"Show PDF Covers (V)";
    SetDlgItemText(hwndPanel, IDC_BTN_TOGGLE_COVERS, coversBtn.c_str());

    // Update info status with current relative browsing path
    std::wstring relDir = toRelativePath(getDesktopDirectoryPath(), settings.pdfDirectory); // Calculate path relative to Desktop
    if (relDir.empty() || relDir == L".") relDir = L"Desktop"; // Format friendly label for Desktop root
    std::wstringstream infoSs; // Create string stream for status text
    infoSs << L"Folder: " << relDir << L"\n" // Format active relative directory
           << L"Items: " << totalItems << L" (Double-click folder to open)\n" // Display item count and navigation hint
           << (settings.coversVisible ? L"[Covers Shown] " : L"[Covers Hidden] ") // Display covers visibility status
           << (settings.clickThrough ? L"[Click-Through]" : L"[Interactive]") // Display click-through mode status
           << L"\n[Radial Menu] Hold Right-Click & Drag!"; // Display gesture hint
    SetDlgItemText(hwndPanel, IDC_TXT_INFO_STATUS, infoSs.str().c_str()); // Set formatted status text in control panel
}
