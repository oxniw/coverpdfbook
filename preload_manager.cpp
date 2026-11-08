// ==============================================================================
// File: .//Description-of-file/preload_manager.md
// Overview: Implements intelligent 3-page memory window preloading and cache eviction.
// ==============================================================================

#include "preload_manager.h" // Include preload manager header
#include "extract_pdf_cover.h" // Include thumbnail extraction functions
#include <algorithm> // Include standard min and max algorithms

void preloadAdjacentPages(
    std::vector<PdfItem>& items, // Master list of items
    const AppSettings& settings, // Current application settings
    const GridLayoutResult& layout // Computed grid layout metrics
) { // Define preloadAdjacentPages implementation
    if (items.empty() || layout.itemsPerPage <= 0) return; // Guard against empty items or invalid layout

    int prevPage = settings.currentPage - 1; // Compute previous page index
    int currPage = settings.currentPage; // Current active page index
    int nextPage = settings.currentPage + 1; // Compute next page index

    int windowMinIndex = (prevPage >= 0) ? (prevPage * layout.itemsPerPage) : 0; // Calculate lower bound of 3-page window
    int windowMaxIndex = (nextPage < layout.totalPages) ? ((nextPage + 1) * layout.itemsPerPage) : ((currPage + 1) * layout.itemsPerPage); // Calculate upper bound
    if (windowMaxIndex > (int)items.size()) windowMaxIndex = (int)items.size(); // Clamp upper bound to total items count

    // 1. Preload thumbnails inside the 3-page active window
    for (int i = windowMinIndex; i < windowMaxIndex; ++i) { // Loop through items in 3-page active window
        PdfItem& item = items[i]; // Get reference to item
        if (!item.thumbnail || item.thumbnail->GetWidth() != (UINT)settings.coverWidth) { // Check if thumbnail missing or resized
            if (item.thumbnail) freePdfThumbnail(item.thumbnail); // Free mismatched thumbnail if present
            if (item.type == ITEM_TYPE_FOLDER || item.type == ITEM_TYPE_PARENT) { // Check if folder or parent item
                item.thumbnail = extractFolderThumbnail(item.filePath, item.displayName, settings.coverWidth, settings.coverHeight, item.type == ITEM_TYPE_PARENT); // Generate folder thumbnail
            } else { // Standard PDF document item
                item.thumbnail = extractPdfCover(item.filePath, item.displayName, settings.coverWidth, settings.coverHeight); // Extract PDF cover thumbnail
            } // Close item type branch
        } // Close thumbnail check
    } // Close preload loop

    // 2. Evict distant pages outside the 3-page window to keep RAM footprint minimal (~15MB-25MB)
    for (int i = 0; i < windowMinIndex; ++i) { // Loop through items before 3-page window
        if (items[i].thumbnail) { // Check if thumbnail is loaded in RAM
            freePdfThumbnail(items[i].thumbnail); // Evict bitmap from memory
        } // Close if statement
    } // Close lower eviction loop

    for (int i = windowMaxIndex; i < (int)items.size(); ++i) { // Loop through items after 3-page window
        if (items[i].thumbnail) { // Check if thumbnail is loaded in RAM
            freePdfThumbnail(items[i].thumbnail); // Evict bitmap from memory
        } // Close if statement
    } // Close upper eviction loop
} // Close preloadAdjacentPages function
