// ==============================================================================
// File: .//Description-of-file/render_overlay_canvas.md
// Overview: Header for drawing transparent anti-aliased GDI+ canvas and presenting
//           it via Win32 UpdateLayeredWindow with smooth page transition animations.
// ==============================================================================

#pragma once

#include "config.h"
#include "calculate_grid_layout.h"
#include <vector>

// Renders the transparent desktop overlay with support for smooth 60 FPS
// slide-and-fade page transition animations.
void renderOverlayCanvas(
    HWND hwndOverlay,
    const AppSettings& settings,
    std::vector<PdfItem>& items,
    const GridLayoutResult& layout,
    const std::vector<RECT>& itemRects,
    const PageAnimationState* pAnim = nullptr
);

// Invalidates and releases cached page snapshot bitmaps (called on folder/layout reload).
void invalidatePageCache();

// Releases persistent DIB section and device contexts on application shutdown.
void cleanupRenderResources();
