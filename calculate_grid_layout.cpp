// ==============================================================================
// File: .//Description-of-file/calculate_grid_layout.md
// Overview: Pure domain calculation for M*N grid positioning, card bounding boxes,
//           and multi-page pagination.
// ==============================================================================

#include "calculate_grid_layout.h"
#include <algorithm>

GridLayoutResult calculateGridLayoutForPage(
    const AppSettings& settings,
    int pageIndex,
    int totalItems,
    int windowWidth,
    int windowHeight,
    std::vector<RECT>& outItemRects
) {
    GridLayoutResult result;
    outItemRects.clear();

    result.totalItems = totalItems;
    int rows = (settings.rows > 0) ? settings.rows : 1;
    int cols = (settings.cols > 0) ? settings.cols : 1;
    result.itemsPerPage = rows * cols;

    if (totalItems == 0) {
        result.totalPages = 1;
        result.startIndex = 0;
        result.endIndex = 0;
        result.totalVisible = 0;
        return result;
    }

    result.totalPages = (totalItems + result.itemsPerPage - 1) / result.itemsPerPage;
    if (result.totalPages < 1) result.totalPages = 1;

    int page = std::clamp(pageIndex, 0, result.totalPages - 1);
    result.startIndex = page * result.itemsPerPage;
    result.endIndex = std::min(result.startIndex + result.itemsPerPage, totalItems);
    result.totalVisible = result.endIndex - result.startIndex;

    outItemRects.resize(result.totalVisible);

    // Compute cell dimensions
    int cardWidth = settings.coverWidth;
    int titleHeight = 38;
    int cardHeight = settings.coverHeight + titleHeight;
    int spacing = settings.spacing;

    int gridWidth = cols * cardWidth + (cols - 1) * spacing;
    int gridHeight = rows * cardHeight + (rows - 1) * spacing;

    int startX = 0;
    int startY = 0;
    int mX = settings.marginX;
    int mY = settings.marginY;

    // Horizontal alignment calculation
    switch (settings.alignment) {
        case ALIGN_TOP_LEFT:
        case ALIGN_MIDDLE_LEFT:
        case ALIGN_BOTTOM_LEFT:
            startX = mX;
            break;
        case ALIGN_TOP_RIGHT:
        case ALIGN_MIDDLE_RIGHT:
        case ALIGN_BOTTOM_RIGHT:
            startX = windowWidth - gridWidth - mX;
            break;
        case ALIGN_TOP_CENTER:
        case ALIGN_CENTER:
        case ALIGN_BOTTOM_CENTER:
        default:
            startX = (windowWidth - gridWidth) / 2;
            break;
    }

    // Vertical alignment calculation
    switch (settings.alignment) {
        case ALIGN_TOP_LEFT:
        case ALIGN_TOP_CENTER:
        case ALIGN_TOP_RIGHT:
            startY = mY;
            break;
        case ALIGN_BOTTOM_LEFT:
        case ALIGN_BOTTOM_CENTER:
        case ALIGN_BOTTOM_RIGHT:
            startY = windowHeight - gridHeight - mY;
            break;
        case ALIGN_MIDDLE_LEFT:
        case ALIGN_CENTER:
        case ALIGN_MIDDLE_RIGHT:
        default:
            startY = (windowHeight - gridHeight) / 2;
            break;
    }

    if (startX < 10) startX = 10;
    if (startY < 10) startY = 10;

    result.gridBounds.left = startX;
    result.gridBounds.top = startY;
    result.gridBounds.right = startX + gridWidth;
    result.gridBounds.bottom = startY + gridHeight;

    for (int i = 0; i < result.totalVisible; ++i) {
        int r = i / cols;
        int c = i % cols;

        int itemLeft = startX + c * (cardWidth + spacing);
        int itemTop = startY + r * (cardHeight + spacing);

        RECT rect;
        rect.left = itemLeft;
        rect.top = itemTop;
        rect.right = itemLeft + cardWidth;
        rect.bottom = itemTop + cardHeight;

        outItemRects[i] = rect;
    }

    return result;
}

GridLayoutResult calculateGridLayout(
    const AppSettings& settings,
    int totalItems,
    int windowWidth,
    int windowHeight,
    std::vector<RECT>& outItemRects
) {
    return calculateGridLayoutForPage(settings, settings.currentPage, totalItems, windowWidth, windowHeight, outItemRects);
}
