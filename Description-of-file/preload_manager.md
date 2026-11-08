# Description: preload_manager

## Overview
High-performance memory management and caching module that preloads thumbnails for adjacent pages (`currentPage - 1` and `currentPage + 1`) during idle time while evicting distant pages from RAM, eliminating page-turn lag spikes and guaranteeing strict memory usage under ~20MB.

## Exports & Functions
- `preloadAdjacentPages(std::vector<PdfItem>& items, const AppSettings& settings, const GridLayoutResult& layout) -> void`
  - **Description**: Computes the 3-page active window (`currentPage - 1` through `currentPage + 1`). Preloads any missing thumbnails in this range so that incoming page turn transitions find their textures already loaded in memory (0ms disk latency). Discards thumbnails for items outside the active window to avoid RAM growth.
  - **Parameters**:
    - `items`: Master collection of all discovered PDF documents and folder cards.
    - `settings`: Application state containing current page and cover width/height metrics.
    - `layout`: Computed grid metrics supplying pagination partitions and item counts.
  - **Returns**: None.

## Dependencies & Data Flow
- **Imports**: `config.h`, `extract_pdf_cover.h`, `<algorithm>`, `<vector>`.
- **Invoked By**: Called from `main.cpp` via deferred message `WM_APP_PRELOAD_ADJACENT` right after a page renders on screen.
- **Data Flow**: `settings.currentPage` -> Active 3-page index bounds computed -> Missing adjacent thumbnails loaded into RAM -> Distant page thumbnails freed.
