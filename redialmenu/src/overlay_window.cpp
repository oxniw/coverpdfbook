// ==============================================================================
// File: .//Description-of-file/overlay_window.md
// Overview: Win32 layered transparent window implementation with UpdateLayeredWindow.
// ==============================================================================

#include "overlay_window.h"
#include "render_overlay.h"
#include "../include/config.h"

static LRESULT CALLBACK OverlayWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_DESTROY:
            return 0;
        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
}

int overlay_window_create(OverlayWindow* win, int width, int height) {
    if (!win) return 0;

    win->width = width;
    win->height = height;
    win->is_visible = 0;
    win->current_sector = RADIAL_SECTOR_NOTHING;

    HINSTANCE hInstance = GetModuleHandle(NULL);

    WNDCLASSEX wc = {};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = OverlayWndProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.lpszClassName = TEXT(RADIAL_WINDOW_CLASS_NAME);

    RegisterClassEx(&wc);

    // Create layered, tool-window (no taskbar button), topmost, transparent to mouse
    win->hwnd = CreateWindowEx(
        WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE,
        TEXT(RADIAL_WINDOW_CLASS_NAME),
        TEXT("RadialMenuOverlay"),
        WS_POPUP,
        0, 0, width, height,
        NULL, NULL, hInstance, NULL
    );

    if (!win->hwnd) {
        return 0;
    }

    // Create 32-bit ARGB DIB Section for alpha channel blending
    HDC screenDC = GetDC(NULL);
    win->mem_dc = CreateCompatibleDC(screenDC);

    BITMAPINFO bmi = {};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = width;
    bmi.bmiHeader.biHeight = -height; // top-down
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    win->mem_bitmap = CreateDIBSection(win->mem_dc, &bmi, DIB_RGB_COLORS, &win->bits, NULL, 0);
    win->old_bitmap = (HBITMAP)SelectObject(win->mem_dc, win->mem_bitmap);
    ReleaseDC(NULL, screenDC);

    return 1;
}

static void update_layered_contents_ex(OverlayWindow* win, float scale, float alpha, const float* hover_weights) {
    if (!win || !win->hwnd || !win->mem_dc) return;

    // Draw using GDI+
    render_draw_menu(win->mem_dc, win->width, win->height, scale, hover_weights);

    POINT ptSrc = { 0, 0 };
    SIZE sizeWnd = { win->width, win->height };
    BLENDFUNCTION blend = {};
    blend.BlendOp = AC_SRC_OVER;

    // Clamp alpha
    if (alpha < 0.0f) alpha = 0.0f;
    if (alpha > 1.0f) alpha = 1.0f;
    blend.SourceConstantAlpha = (BYTE)(255.0f * alpha);
    blend.AlphaFormat = AC_SRC_ALPHA;

    HDC screenDC = GetDC(NULL);
    UpdateLayeredWindow(
        win->hwnd,
        screenDC,
        NULL,
        &sizeWnd,
        win->mem_dc,
        &ptSrc,
        0,
        &blend,
        ULW_ALPHA
    );
    ReleaseDC(NULL, screenDC);
}

void overlay_window_update_animated(OverlayWindow* win, float scale, float alpha, const float* hover_weights) {
    if (!win || !win->hwnd) return;
    if (win->is_visible) {
        update_layered_contents_ex(win, scale, alpha, hover_weights);
    }
}

void overlay_window_show_at(OverlayWindow* win, int center_x, int center_y) {
    if (!win || !win->hwnd) return;

    int posX = center_x - (win->width / 2);
    int posY = center_y - (win->height / 2);

    SetWindowPos(
        win->hwnd,
        HWND_TOPMOST,
        posX, posY,
        win->width, win->height,
        SWP_NOACTIVATE | SWP_SHOWWINDOW
    );

    win->is_visible = 1;
}

void overlay_window_hide(OverlayWindow* win) {
    if (!win || !win->hwnd) return;

    ShowWindow(win->hwnd, SW_HIDE);
    win->is_visible = 0;
}

void overlay_window_update_sector(OverlayWindow* win, RadialSector sector) {
    if (!win || !win->hwnd) return;

    win->current_sector = sector;
    if (win->is_visible) {
        float weights[5] = { 0 };
        int idx = (int)sector;
        if (idx >= 0 && idx < 5) weights[idx] = 1.0f;
        update_layered_contents_ex(win, 1.0f, 1.0f, weights);
    }
}

void overlay_window_destroy(OverlayWindow* win) {
    if (!win) return;

    if (win->mem_dc) {
        if (win->old_bitmap) {
            SelectObject(win->mem_dc, win->old_bitmap);
        }
        DeleteDC(win->mem_dc);
        win->mem_dc = NULL;
    }

    if (win->mem_bitmap) {
        DeleteObject(win->mem_bitmap);
        win->mem_bitmap = NULL;
    }

    if (win->hwnd) {
        DestroyWindow(win->hwnd);
        win->hwnd = NULL;
    }

    win->is_visible = 0;
}
