// ==============================================================================
// File: .//Description-of-file/scan_pdf_directory.md
// Overview: Implementation of directory scanning discovering subfolders, parent
//           navigation entries, and PDF documents using relative and absolute paths.
// ==============================================================================

#include "scan_pdf_directory.h" // Include directory scanning header
#include "path_utils.h" // Include path manipulation utilities
#include <windows.h> // Include core Windows API
#include <algorithm> // Include standard sorting algorithms

namespace { // Begin anonymous namespace for local helper functions
    std::wstring formatDisplayName(const std::wstring& filename) { // Define formatDisplayName helper function
        size_t dotPos = filename.find_last_of(L"."); // Find position of extension dot
        std::wstring base = (dotPos == std::wstring::npos) ? filename : filename.substr(0, dotPos); // Strip extension if present
        for (auto& ch : base) { // Iterate over each character in base name
            if (ch == L'_') ch = L' '; // Replace underscores with spaces for readability
        } // Close character loop
        return base; // Return formatted base display name
    } // Close formatDisplayName function
} // Close anonymous namespace

std::vector<PdfItem> scanPdfDirectory(const std::wstring& directoryPath) { // Define scanPdfDirectory implementation
    std::vector<PdfItem> items; // Initialize vector to hold all discovered items
    std::wstring normDir = normalizePath(directoryPath); // Normalize directory path to backslashes
    if (normDir.empty()) return items; // Return empty vector if directory path is empty

    // 1. Check if parent directory exists and add ".." navigation entry
    std::wstring parentDir = getParentDirectory(normDir); // Query parent directory path
    if (!parentDir.empty()) { // Check if valid parent directory exists
        PdfItem parentItem; // Create parent navigation item
        parentItem.type = ITEM_TYPE_PARENT; // Assign parent item classification
        parentItem.fileName = L".."; // Set file name representation
        parentItem.displayName = L".. (Up)"; // Set display label with up indicator
        parentItem.filePath = parentDir; // Store target parent directory path
        parentItem.relativePath = L".."; // Set relative navigation path
        parentItem.thumbnail = nullptr; // Initialize thumbnail pointer to null
        items.push_back(parentItem); // Push parent navigation entry to front of items list
    } // Close if statement

    std::vector<PdfItem> folderItems; // Initialize vector for subdirectories
    std::vector<PdfItem> pdfItems; // Initialize vector for PDF documents

    // 2. Discover subdirectories in the target folder
    std::wstring folderPattern = normDir + L"\\*"; // Build wildcard search pattern for all entries
    WIN32_FIND_DATAW findData; // Allocate find data structure for Windows filesystem search
    HANDLE hFind = FindFirstFileW(folderPattern.c_str(), &findData); // Begin search in target directory
    if (hFind != INVALID_HANDLE_VALUE) { // Check if directory search handle is valid
        do { // Loop through all directory entries
            if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) { // Check if current entry is a directory
                if (wcscmp(findData.cFileName, L".") != 0 && wcscmp(findData.cFileName, L"..") != 0) { // Skip current (.) and parent (..) dot folders
                    if (findData.cFileName[0] != L'$' && wcscmp(findData.cFileName, L".git") != 0 && wcscmp(findData.cFileName, L".cache") != 0 && wcscmp(findData.cFileName, L"redialmenu") != 0 && wcscmp(findData.cFileName, L"movewindowsystem") != 0 && wcscmp(findData.cFileName, L"Description-of-file") != 0) { // Skip system and internal folders
                        PdfItem folderItem; // Create folder item instance
                        folderItem.type = ITEM_TYPE_FOLDER; // Assign folder classification
                        folderItem.fileName = findData.cFileName; // Store folder name
                        folderItem.displayName = findData.cFileName; // Store folder display name
                        folderItem.filePath = resolvePath(normDir, findData.cFileName); // Compute absolute canonical folder path
                        folderItem.relativePath = toRelativePath(normDir, folderItem.filePath); // Compute relative path from current directory
                        folderItem.thumbnail = nullptr; // Initialize thumbnail to null
                        folderItems.push_back(folderItem); // Add folder to subdirectories list
                    } // Close if statement
                } // Close if statement
            } // Close if statement
        } while (FindNextFileW(hFind, &findData)); // Continue until all entries are checked
        FindClose(hFind); // Close filesystem search handle
    } // Close if statement

    // 3. Discover PDF documents in the target folder
    std::wstring pdfPattern = normDir + L"\\*.pdf"; // Build search pattern specifically for PDF documents
    hFind = FindFirstFileW(pdfPattern.c_str(), &findData); // Begin search for PDF files
    if (hFind != INVALID_HANDLE_VALUE) { // Check if PDF search handle is valid
        do { // Loop through all PDF file matches
            if (!(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) { // Ensure matched entry is a regular file
                PdfItem pdfItem; // Create PDF item instance
                pdfItem.type = ITEM_TYPE_PDF; // Assign PDF classification
                pdfItem.fileName = findData.cFileName; // Store PDF file name
                pdfItem.displayName = formatDisplayName(findData.cFileName); // Format human-readable display name
                pdfItem.filePath = resolvePath(normDir, findData.cFileName); // Compute absolute canonical file path
                pdfItem.relativePath = toRelativePath(normDir, pdfItem.filePath); // Compute relative path from current directory
                pdfItem.thumbnail = nullptr; // Initialize thumbnail to null
                pdfItems.push_back(pdfItem); // Add PDF to documents list
            } // Close if statement
        } while (FindNextFileW(hFind, &findData)); // Continue until all PDF files are collected
        FindClose(hFind); // Close PDF search handle
    } // Close if statement

    // 4. Sort folders alphabetically
    std::sort(folderItems.begin(), folderItems.end(), [](const PdfItem& a, const PdfItem& b) { // Sort folder items alphabetically
        return _wcsicmp(a.fileName.c_str(), b.fileName.c_str()) < 0; // Compare folder names case-insensitively
    }); // Close folder sort

    // 5. Sort PDF files alphabetically
    std::sort(pdfItems.begin(), pdfItems.end(), [](const PdfItem& a, const PdfItem& b) { // Sort PDF items alphabetically
        return _wcsicmp(a.fileName.c_str(), b.fileName.c_str()) < 0; // Compare PDF names case-insensitively
    }); // Close PDF sort

    // 6. Concatenate: [Parent ..] -> [Folders] -> [PDFs]
    for (auto& f : folderItems) { // Loop through sorted folders
        items.push_back(f); // Append folder to master list
    } // Close folder loop
    for (auto& p : pdfItems) { // Loop through sorted PDFs
        items.push_back(p); // Append PDF to master list
    } // Close PDF loop

    return items; // Return consolidated list of items
} // Close scanPdfDirectory function
