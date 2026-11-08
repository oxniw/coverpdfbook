# Description: render_overlay_canvas

## Overview
Double-buffered anti-aliased GDI+ rendering engine that draws the clean, clutter-free desktop overlay: 100% transparent background with zero headers or footers, rendering only the floating PDF book covers and their title badges with soft drop shadows.

## Exports & Functions
- `renderOverlayCanvas(HWND hwndOverlay, const AppSettings& settings, std::vector<PdfItem>& items, const GridLayoutResult& layout, const std::vector<RECT>& itemRects, const PageAnimationState* pAnim = nullptr) -> void`
  - **Description**: Clears the canvas with full alpha transparency, renders each PDF's true first-page cover with soft drop shadows and glowing borders, renders smooth horizontal slide-and-fade page transitions when animating using zero-allocation persistent DIB section and texture snapshot caching, and presents it with per-pixel alpha blending across the entire desktop monitor.
  - **Parameters**:
    - `hwndOverlay`: Handle to the fullscreen layered desktop overlay window.
    - `settings`: Application state determining opacity, cover dimensions, and hover indices.
    - `items`: Master vector of PDF items.
    - `layout`: Computed grid layout and pagination result.
    - `itemRects`: Bounding coordinates for each card on the active page.
    - `pAnim`: Optional pointer to `PageAnimationState` supplying easing progress, slide direction, and source/target page indices.
  - **Returns**: None.
- `invalidatePageCache() -> void`
  - **Description**: Discards and frees cached page snapshot bitmaps, ensuring stale textures are purged when folder or grid geometry changes.
- `cleanupRenderResources() -> void`
  - **Description**: Destroys the persistent offscreen 32bpp DIB section, memory DC, and page caches during application shutdown.

## Dependencies & Data Flow
- **Imports**: `render_overlay_canvas.h`, `extract_pdf_cover.h`, `<windows.h>`, `<gdiplus.h>`, `<sstream>`, `<iomanip>`, `<algorithm>`.
- **Invoked By**: Called from `main.cpp` (`refreshDisplay`) whenever layout, zoom, hover index, opacity, or page changes.
- **Data Flow**: `items` + `settings` + `layout` -> Pure transparent canvas drawing -> Win32 `UpdateLayeredWindow`.
