// ==============================================================================
// File: .//Description-of-file/scan_pdf_directory.md
// Overview: Header defining directory scanning to discover subfolders and PDF files.
// ==============================================================================

#pragma once // Prevent duplicate header inclusion

#include "config.h" // Include application configuration and data structures
#include <vector> // Include standard vector container
#include <string> // Include standard string library

// Scans target folder for parent directory (..), subfolders, and *.pdf files.
std::vector<PdfItem> scanPdfDirectory(const std::wstring& directoryPath); // Declare scanPdfDirectory function
