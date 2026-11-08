// ==============================================================================
// File: .//Description-of-file/extract_pdf_cover.md
// Overview: Header for extracting PDF first-page covers and generating folder
//           thumbnail covers with Windows Shell and procedural fallbacks.
// ==============================================================================

#pragma once // Prevent duplicate header inclusion

#include "config.h" // Include configuration and data models
#include <string> // Include standard string library

// Pre-rasterizes first-page thumbnails for all PDFs in the given folder into .cache/.
void triggerPdfCoverBatchExtraction(const std::wstring& directoryPath); // Declare triggerPdfCoverBatchExtraction function

// Extracts or retrieves the rendered page 1 cover bitmap for a given PDF file at target dimensions.
Gdiplus::Bitmap* extractPdfCover(const std::wstring& filePath, const std::wstring& displayName, int targetWidth, int targetHeight); // Declare extractPdfCover function

// Generates an aesthetic folder thumbnail cover card with folder icon, tab, and labels.
Gdiplus::Bitmap* extractFolderThumbnail(const std::wstring& folderPath, const std::wstring& displayName, int targetWidth, int targetHeight, bool isParent); // Declare extractFolderThumbnail function

// Releases Gdiplus Bitmap if valid and clears the pointer.
void freePdfThumbnail(Gdiplus::Bitmap*& pBitmap); // Declare freePdfThumbnail function
