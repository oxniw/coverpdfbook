// ==============================================================================
// File: .//Description-of-file/calculate_grid_layout.md
// Overview: Header for pure layout calculations of the M*N grid and pagination.
// ==============================================================================

#pragma once

#include "config.h"
#include <vector>

// Calculates exact bounding rectangles for all visible PDF covers on a specific page.
GridLayoutResult calculateGridLayoutForPage(
    const AppSettings& settings,
    int pageIndex,
    int totalItems,
    int windowWidth,
    int windowHeight,
    std::vector<RECT>& outItemRects
);

// Calculates exact bounding rectangles for all visible PDF covers on the active settings page.
GridLayoutResult calculateGridLayout(
    const AppSettings& settings,
    int totalItems,
    int windowWidth,
    int windowHeight,
    std::vector<RECT>& outItemRects
);
