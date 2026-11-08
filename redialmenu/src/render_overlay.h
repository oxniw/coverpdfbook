// ==============================================================================
// File: .//Description-of-file/render_overlay.md
// Overview: Header for GDI+ anti-aliased radial pie menu rendering routines.
// ==============================================================================

#ifndef RENDER_OVERLAY_H
#define RENDER_OVERLAY_H

#include <windows.h>
#include "../include/radial_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize GDI+ subsystem.
 * @return 1 on success, 0 on failure.
 */
int render_init_gdiplus(void);

/**
 * @brief Shutdown GDI+ subsystem.
 */
void render_shutdown_gdiplus(void);

/**
 * @brief Configure number of outer sectors N for rendering (2..16).
 */
void render_set_sector_count(int count);

/**
 * @brief Configure custom label string for a sector (1 to N).
 */
void render_set_sector_label(int sector_idx, const wchar_t* label);

/**
 * @brief Render the radial menu into a 32-bit ARGB memory DC with animation parameters.
 *
 * @param hdcTarget Target memory DC.
 * @param width Width of the drawing area.
 * @param height Height of the drawing area.
 * @param scale Current geometric scale multiplier (e.g. 0.45 to 1.0).
 * @param hover_weights Array of 5 float weights [0.0, 1.0] for smooth hover blending.
 */
void render_draw_menu(HDC hdcTarget, int width, int height, float scale, const float* hover_weights);

#ifdef __cplusplus
}
#endif

#endif // RENDER_OVERLAY_H
