# Description: extract_pdf_cover

## Overview
Extracts high-resolution first-page PDF covers using the `pypdfium2` rasterizer engine (`extract_pdf_cover.py`), backed by `IShellItemImageFactory` and an aesthetic procedural GDI+ vector book-cover generator fallback.

## Exports & Functions
- `triggerPdfCoverBatchExtraction(const std::wstring& directoryPath) -> void`
  - **Description**: Spawns a hidden background worker (`extract_pdf_cover.py`) to rasterize page 1 of all PDF files in the target directory into `.cache/<filename>.png` at 2.0x retina scale.
  - **Parameters**:
    - `directoryPath`: The path to the folder containing PDF documents.
  - **Returns**: None.
- `extractPdfCover(const std::wstring& filePath, const std::wstring& displayName, int targetWidth, int targetHeight) -> Gdiplus::Bitmap*`
  - **Description**: Checks for the pre-rasterized first-page thumbnail in `.cache/`, loads it directly, and fits it to `targetWidth` x `targetHeight` preserving exact aspect ratio with high quality bicubic interpolation. Falls back to Shell COM thumbnails and procedural covers if needed.
  - **Parameters**:
    - `filePath`: Absolute path to the PDF document.
    - `displayName`: Clean human-readable document title.
    - `targetWidth`: Desired thumbnail width in pixels.
    - `targetHeight`: Desired thumbnail height in pixels.
  - **Returns**: A pointer to an allocated 32bpp ARGB `Gdiplus::Bitmap` containing the rendered cover.
- `extractFolderThumbnail(const std::wstring& folderPath, const std::wstring& displayName, int targetWidth, int targetHeight, bool isParent) -> Gdiplus::Bitmap*`
  - **Description**: Generates an aesthetic folder cover card styled with dark gradient, folder tabs, Windows Shell system folder icon, and distinctive parent navigation indicator (`^`).
  - **Parameters**:
    - `folderPath`: Absolute or relative path to the folder.
    - `displayName`: Folder display name.
    - `targetWidth`: Desired thumbnail width in pixels.
    - `targetHeight`: Desired thumbnail height in pixels.
    - `isParent`: `true` if this card represents parent directory navigation (`..`).
  - **Returns**: A pointer to an allocated 32bpp ARGB `Gdiplus::Bitmap` containing the folder cover.
- `freePdfThumbnail(Gdiplus::Bitmap*& pBitmap) -> void`
  - **Description**: Safely deallocates a cached GDI+ bitmap and nullifies its reference.

## Dependencies & Data Flow
- **Imports**: `config.h`, `<windows.h>`, `<shobjidl.h>`, `<gdiplus.h>`, `<vector>`, `<algorithm>`.
- **Invoked By**: Called from `main.cpp` (`reloadPdfFolder` and startup) and lazily by `renderOverlayCanvas`.
- **Data Flow**: PDF files in `pdf/` -> `extract_pdf_cover.py` rasterizes page 1 -> Cached in `.cache/*.png` -> Loaded by `extractPdfCover` into `Gdiplus::Bitmap*`.
