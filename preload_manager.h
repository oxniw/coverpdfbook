// ==============================================================================
// File: .//Description-of-file/preload_manager.md
// Overview: Header for background pre-loading of adjacent page thumbnails and memory pruning.
// ==============================================================================

#pragma once // Prevent duplicate header inclusion

#include "config.h" // Include application configuration and data models
#include <vector> // Include standard vector container

// Pre-loads thumbnails for adjacent pages (currentPage - 1 and currentPage + 1)
// and evicts distant pages from RAM to maintain a strict, low memory footprint.
void preloadAdjacentPages(
    std::vector<PdfItem>& items, // Master list of items
    const AppSettings& settings, // Current application settings
    const GridLayoutResult& layout // Computed grid layout metrics
); // Declare preloadAdjacentPages function
