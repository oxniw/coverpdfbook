// ==============================================================================
// File: .//Description-of-file/overlay_window.md
// Overview: Header for Win32 layered transparent window creation and presentation.
// ==============================================================================

#ifndef OVERLAY_WINDOW_H
#define OVERLAY_WINDOW_H

#include <windows.h>
#include "../include/radial_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    HWND hwnd;
    HDC mem_dc;
    HBITMAP mem_bitmap;
    HBITMAP old_bitmap;
    void* bits;
    int width;
    int height;
    int is_visible;
    RadialSector current_sector;
} OverlayWindow;

/**
 * @brief Create the transparent layered overlay window.
 * @param win Pointer to OverlayWindow instance.
 * @param width Window pixel width.
 * @param height Window pixel height.
 * @return 1 on success, 0 on failure.
 */
int overlay_window_create(OverlayWindow* win, int width, int height);

/**
 * @brief Show and position the overlay window centered at (center_x, center_y).
 */
void overlay_window_show_at(OverlayWindow* win, int center_x, int center_y);

/**
 * @brief Hide the overlay window.
 */
void overlay_window_hide(OverlayWindow* win);

/**
 * @brief Update animated visual properties (scale, alpha, and sector hover weights).
 */
void overlay_window_update_animated(OverlayWindow* win, float scale, float alpha, const float* hover_weights);

/**
 * @brief Update the highlighted sector and re-render the layered window.
 */
void overlay_window_update_sector(OverlayWindow* win, RadialSector sector);

/**
 * @brief Destroy window and free graphic memory DC resources.
 */
void overlay_window_destroy(OverlayWindow* win);

#ifdef __cplusplus
}
#endif

#endif // OVERLAY_WINDOW_H
