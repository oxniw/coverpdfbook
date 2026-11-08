# Description: scan_pdf_directory

## Overview
Discovers and catalogues subdirectories, parent directory (`..`) navigation entries, and `.pdf` documents in a target directory using native Win32 filesystem discovery APIs (`FindFirstFileW` / `FindNextFileW`) and relative path utilities.

## Exports & Functions
- `scanPdfDirectory(const std::wstring& directoryPath) -> std::vector<PdfItem>`
  - **Description**: Traverses the specified folder, discovers parent navigation (`..`) if applicable, discovers subdirectories, extracts all `.pdf` files, computes their relative and absolute canonical paths, creates clean display titles, and sorts folders first then PDFs alphabetically.
  - **Parameters**:
    - `directoryPath`: The relative or absolute path of the directory to scan (defaults to Windows Desktop).
  - **Returns**: A vector of initialized `PdfItem` objects (type `ITEM_TYPE_PARENT`, `ITEM_TYPE_FOLDER`, or `ITEM_TYPE_PDF`) ready for layout and thumbnail rendering.

## Dependencies & Data Flow
- **Imports**: `config.h`, `path_utils.h`, `<windows.h>`, `<algorithm>`, `<vector>`, `<string>`.
- **Invoked By**: Called from `main.cpp` during initial startup and whenever the user double-clicks a folder or triggers a folder reload via `[R]`.
- **Data Flow**: Target directory string -> Win32 file & folder discovery -> `std::vector<PdfItem>` passed to layout calculation and canvas renderer.
