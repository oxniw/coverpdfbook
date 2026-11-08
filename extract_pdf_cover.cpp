// ==============================================================================
// File: .//Description-of-file/extract_pdf_cover.md
// Overview: Implements extraction of PDF covers using pypdfium2 rasterizer,
//           IShellItemImageFactory COM interface, and procedural fallback generator.
// ==============================================================================

#include "extract_pdf_cover.h"
#include <windows.h>
#include <shobjidl.h>
#include <gdiplus.h>
#include <vector>
#include <algorithm>

namespace {
    std::wstring getFileNameFromPath(const std::wstring& path) {
        size_t lastSlash = path.find_last_of(L"\\/");
        if (lastSlash == std::wstring::npos) return path;
        return path.substr(lastSlash + 1);
    }

    bool fileExists(const std::wstring& path) {
        DWORD attrib = GetFileAttributesW(path.c_str());
        return (attrib != INVALID_FILE_ATTRIBUTES && !(attrib & FILE_ATTRIBUTE_DIRECTORY));
    }

    // Generate an aesthetic fallback cover bitmap using GDI+
    Gdiplus::Bitmap* generateFallbackCover(const std::wstring& displayName, int width, int height) {
        if (width < 32) width = 32;
        if (height < 32) height = 32;

        Gdiplus::Bitmap* bmp = new Gdiplus::Bitmap(width, height, PixelFormat32bppARGB);
        Gdiplus::Graphics g(bmp);
        g.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
        g.SetTextRenderingHint(Gdiplus::TextRenderingHintClearTypeGridFit);

        size_t hash = 5381;
        for (wchar_t c : displayName) {
            hash = ((hash << 5) + hash) + c;
        }

        struct ColorTheme {
            Gdiplus::Color top;
            Gdiplus::Color bottom;
            Gdiplus::Color accent;
        };

        const ColorTheme themes[] = {
            { Gdiplus::Color(255, 26, 54, 93),   Gdiplus::Color(255, 15, 23, 42),  Gdiplus::Color(255, 96, 165, 250) },
            { Gdiplus::Color(255, 19, 78, 74),   Gdiplus::Color(255, 6, 44, 43),   Gdiplus::Color(255, 52, 211, 153) },
            { Gdiplus::Color(255, 112, 26, 51),  Gdiplus::Color(255, 64, 12, 28),  Gdiplus::Color(255, 244, 114, 182) },
            { Gdiplus::Color(255, 88, 28, 135),  Gdiplus::Color(255, 46, 16, 101), Gdiplus::Color(255, 192, 132, 252) },
            { Gdiplus::Color(255, 124, 45, 18),  Gdiplus::Color(255, 67, 20, 7),   Gdiplus::Color(255, 251, 146, 60) },
            { Gdiplus::Color(255, 30, 41, 59),   Gdiplus::Color(255, 15, 23, 42),  Gdiplus::Color(255, 148, 163, 184) }
        };

        const ColorTheme& theme = themes[hash % (sizeof(themes) / sizeof(themes[0]))];

        Gdiplus::Rect fullRect(0, 0, width, height);
        Gdiplus::LinearGradientBrush bgBrush(
            fullRect,
            theme.top,
            theme.bottom,
            Gdiplus::LinearGradientModeForwardDiagonal
        );
        g.FillRectangle(&bgBrush, fullRect);

        int spineWidth = (width > 80) ? 14 : 8;
        Gdiplus::LinearGradientBrush spineBrush(
            Gdiplus::Rect(0, 0, spineWidth, height),
            Gdiplus::Color(140, 0, 0, 0),
            Gdiplus::Color(0, 0, 0, 0),
            Gdiplus::LinearGradientModeHorizontal
        );
        g.FillRectangle(&spineBrush, 0, 0, spineWidth, height);

        Gdiplus::Pen spinePen(Gdiplus::Color(60, 255, 255, 255), 1.5f);
        g.DrawLine(&spinePen, spineWidth, 0, spineWidth, height);

        Gdiplus::Pen accentPen(theme.accent, 2.0f);
        int topMargin = height / 6;
        g.DrawLine(&accentPen, spineWidth + 12, topMargin, width - 16, topMargin);

        float titleFontSize = (width > 160) ? 11.5f : ((width > 120) ? 9.5f : 8.0f);
        Gdiplus::Font titleFont(L"Segoe UI Semibold", titleFontSize, Gdiplus::FontStyleBold);
        Gdiplus::SolidBrush textShadow(Gdiplus::Color(160, 0, 0, 0));
        Gdiplus::SolidBrush textBrush(Gdiplus::Color(255, 245, 245, 250));

        Gdiplus::RectF titleRect(
            (Gdiplus::REAL)(spineWidth + 12),
            (Gdiplus::REAL)(topMargin + 14),
            (Gdiplus::REAL)(width - spineWidth - 24),
            (Gdiplus::REAL)(height - topMargin - 40)
        );

        Gdiplus::StringFormat titleFormat;
        titleFormat.SetAlignment(Gdiplus::StringAlignmentNear);
        titleFormat.SetLineAlignment(Gdiplus::StringAlignmentNear);
        titleFormat.SetTrimming(Gdiplus::StringTrimmingEllipsisWord);

        Gdiplus::RectF shadowRect = titleRect;
        shadowRect.X += 1.0f;
        shadowRect.Y += 1.0f;
        g.DrawString(displayName.c_str(), -1, &titleFont, shadowRect, &titleFormat, &textShadow);
        g.DrawString(displayName.c_str(), -1, &titleFont, titleRect, &titleFormat, &textBrush);

        Gdiplus::Pen borderPen(Gdiplus::Color(100, 255, 255, 255), 1.0f);
        g.DrawRectangle(&borderPen, 0, 0, width - 1, height - 1);

        return bmp;
    }
}

namespace {
    struct BatchNotifyContext { // Context structure for background worker thread
        HANDLE hProcess; // Handle to the Python background process
        HWND hwndNotify; // Window handle to receive completion notification
    }; // End context structure

    static DWORD WINAPI BatchWorkerThreadProc(LPVOID lpParam) { // Thread function waiting on Python extraction
        BatchNotifyContext* ctx = (BatchNotifyContext*)lpParam; // Cast parameter to context pointer
        WaitForSingleObject(ctx->hProcess, 60000); // Wait in background thread for process completion
        CloseHandle(ctx->hProcess); // Close background process handle
        if (ctx->hwndNotify && IsWindow(ctx->hwndNotify)) { // Verify notification window is valid
            PostMessage(ctx->hwndNotify, WM_APP_CACHE_UPDATED, 0, 0); // Send cache updated message to main overlay
        } // Close if statement
        delete ctx; // Clean up dynamically allocated context
        return 0; // Exit thread successfully
    } // End thread procedure
} // Close anonymous namespace

void triggerPdfCoverBatchExtraction(const std::wstring& directoryPath, HWND hwndNotify) {
    CreateDirectoryW(L".cache", NULL); // Ensure .cache directory exists
    std::wstring cmd = L"python extract_pdf_cover.py \"" + directoryPath + L"\""; // Build python command string

    STARTUPINFOW si = { sizeof(STARTUPINFOW) }; // Initialize startup info structure
    si.dwFlags = STARTF_USESHOWWINDOW; // Specify window visibility flag
    si.wShowWindow = SW_HIDE; // Run Python process completely hidden in background
    PROCESS_INFORMATION pi = { 0 }; // Initialize process information structure

    std::vector<wchar_t> cmdBuf(cmd.begin(), cmd.end()); // Copy command string to mutable buffer
    cmdBuf.push_back(L'\0'); // Null-terminate command buffer

    if (CreateProcessW(NULL, cmdBuf.data(), NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi)) { // Launch Python process asynchronously
        CloseHandle(pi.hThread); // Close thread handle immediately
        BatchNotifyContext* ctx = new BatchNotifyContext{ pi.hProcess, hwndNotify }; // Allocate thread context
        HANDLE hThread = CreateThread(NULL, 0, BatchWorkerThreadProc, ctx, 0, NULL); // Spawn background monitor thread
        if (hThread) { // Check if thread created
            CloseHandle(hThread); // Close thread handle so it cleans itself up
        } else { // Fallback if thread creation fails
            CloseHandle(pi.hProcess); // Close process handle
            delete ctx; // Delete context
        } // Close thread check
    } // Close CreateProcess check
}

Gdiplus::Bitmap* extractPdfCover(const std::wstring& filePath, const std::wstring& displayName, int targetWidth, int targetHeight) {
    if (targetWidth <= 0) targetWidth = 180; // Ensure minimum valid width
    if (targetHeight <= 0) targetHeight = 250; // Ensure minimum valid height

    std::wstring filename = getFileNameFromPath(filePath); // Extract filename from path
    std::wstring cachePath = L".cache\\" + filename + L".png"; // Construct cache file path

    // 1. Try loading cached rendered page thumbnail without blocking UI
    if (fileExists(cachePath)) { // Check if pre-rendered PNG exists on disk
        Gdiplus::Bitmap* cachedBmp = Gdiplus::Bitmap::FromFile(cachePath.c_str());
        if (cachedBmp != nullptr && cachedBmp->GetLastStatus() == Gdiplus::Ok) {
            Gdiplus::Bitmap* finalBmp = new Gdiplus::Bitmap(targetWidth, targetHeight, PixelFormat32bppARGB);
            Gdiplus::Graphics g(finalBmp);
            g.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
            g.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);

            g.Clear(Gdiplus::Color(0, 0, 0, 0));

            float origW = (float)cachedBmp->GetWidth();
            float origH = (float)cachedBmp->GetHeight();
            float scale = std::min((float)targetWidth / origW, (float)targetHeight / origH);

            float drawW = origW * scale;
            float drawH = origH * scale;
            float drawX = ((float)targetWidth - drawW) / 2.0f;
            float drawY = ((float)targetHeight - drawH) / 2.0f;

            // Draw crisp rendered page
            g.DrawImage(cachedBmp, drawX, drawY, drawW, drawH);

            // Subtle page border
            Gdiplus::Pen borderPen(Gdiplus::Color(70, 0, 0, 0), 1.0f);
            g.DrawRectangle(&borderPen, drawX, drawY, drawW - 1, drawH - 1);

            delete cachedBmp;
            return finalBmp;
        }
        if (cachedBmp) delete cachedBmp;
    }

    // 2. Fallback to IShellItemImageFactory COM interface
    IShellItemImageFactory* pFactory = nullptr;
    HRESULT hr = SHCreateItemFromParsingName(filePath.c_str(), nullptr, IID_PPV_ARGS(&pFactory));
    
    if (SUCCEEDED(hr) && pFactory != nullptr) {
        SIZE size = { targetWidth * 2, targetHeight * 2 };
        HBITMAP hbmp = nullptr;
        hr = pFactory->GetImage(size, SIIGBF_THUMBNAILONLY, &hbmp);
        if (FAILED(hr)) {
            hr = pFactory->GetImage(size, SIIGBF_BIGGERSIZEOK, &hbmp);
        }
        pFactory->Release();

        if (SUCCEEDED(hr) && hbmp != nullptr) {
            Gdiplus::Bitmap* originalBmp = Gdiplus::Bitmap::FromHBITMAP(hbmp, nullptr);
            DeleteObject(hbmp);

            if (originalBmp != nullptr && originalBmp->GetLastStatus() == Gdiplus::Ok) {
                Gdiplus::Bitmap* finalBmp = new Gdiplus::Bitmap(targetWidth, targetHeight, PixelFormat32bppARGB);
                Gdiplus::Graphics g(finalBmp);
                g.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
                g.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
                g.Clear(Gdiplus::Color(0, 0, 0, 0));

                float origW = (float)originalBmp->GetWidth();
                float origH = (float)originalBmp->GetHeight();
                float scale = std::min((float)targetWidth / origW, (float)targetHeight / origH);

                float drawW = origW * scale;
                float drawH = origH * scale;
                float drawX = ((float)targetWidth - drawW) / 2.0f;
                float drawY = ((float)targetHeight - drawH) / 2.0f;

                g.DrawImage(originalBmp, drawX, drawY, drawW, drawH);

                delete originalBmp;
                return finalBmp;
            }
            if (originalBmp) delete originalBmp;
        }
    }

    // 3. Fallback to procedural book cover generator
    return generateFallbackCover(displayName, targetWidth, targetHeight); // Return generated fallback book cover
}

Gdiplus::Bitmap* extractFolderThumbnail(const std::wstring& folderPath, const std::wstring& displayName, int targetWidth, int targetHeight, bool isParent) {
    if (targetWidth <= 0) targetWidth = 160; // Ensure minimum valid width
    if (targetHeight <= 0) targetHeight = 250; // Ensure minimum valid height

    Gdiplus::Bitmap* bmp = new Gdiplus::Bitmap(targetWidth, targetHeight, PixelFormat32bppARGB); // Create 32bpp bitmap for folder card
    Gdiplus::Graphics g(bmp); // Create GDI+ graphics context
    g.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality); // Enable high quality smoothing
    g.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic); // Enable bicubic interpolation
    g.SetTextRenderingHint(Gdiplus::TextRenderingHintClearTypeGridFit); // Enable ClearType text rendering

    // 1. Clear transparent background
    g.Clear(Gdiplus::Color(0, 0, 0, 0)); // Clear with full transparency

    // 2. Outer card body background
    Gdiplus::RectF cardRect(0.0f, 0.0f, (Gdiplus::REAL)targetWidth, (Gdiplus::REAL)targetHeight); // Define card rectangle boundary
    Gdiplus::GraphicsPath cardPath; // Create graphics path for rounded card
    float radius = 10.0f; // Define corner rounding radius
    float diameter = radius * 2.0f; // Calculate corner diameter
    cardPath.AddArc(0.0f, 0.0f, diameter, diameter, 180.0f, 90.0f); // Add top-left corner
    cardPath.AddArc((Gdiplus::REAL)targetWidth - diameter, 0.0f, diameter, diameter, 270.0f, 90.0f); // Add top-right corner
    cardPath.AddArc((Gdiplus::REAL)targetWidth - diameter, (Gdiplus::REAL)targetHeight - diameter, diameter, diameter, 0.0f, 90.0f); // Add bottom-right corner
    cardPath.AddArc(0.0f, (Gdiplus::REAL)targetHeight - diameter, diameter, diameter, 90.0f, 90.0f); // Add bottom-left corner
    cardPath.CloseFigure(); // Close rounded card path

    // Color gradient for folder card
    Gdiplus::Color topColor = isParent ? Gdiplus::Color(255, 30, 41, 59) : Gdiplus::Color(255, 15, 23, 42); // Determine top gradient color
    Gdiplus::Color bottomColor = isParent ? Gdiplus::Color(255, 15, 23, 42) : Gdiplus::Color(255, 2, 6, 23); // Determine bottom gradient color
    Gdiplus::LinearGradientBrush bgBrush(cardRect, topColor, bottomColor, Gdiplus::LinearGradientModeVertical); // Create gradient brush
    g.FillPath(&bgBrush, &cardPath); // Fill card background

    // Subtle border
    Gdiplus::Pen borderPen(isParent ? Gdiplus::Color(100, 148, 163, 184) : Gdiplus::Color(100, 245, 158, 11), 1.5f); // Create colored border pen
    g.DrawPath(&borderPen, &cardPath); // Draw card outline

    // 3. Folder tab contour at the top
    Gdiplus::GraphicsPath tabPath; // Create path for top folder tab
    tabPath.AddRectangle(Gdiplus::RectF(10.0f, 10.0f, targetWidth * 0.45f, 16.0f)); // Add tab header rect
    Gdiplus::SolidBrush tabBrush(isParent ? Gdiplus::Color(255, 51, 65, 85) : Gdiplus::Color(255, 217, 119, 6)); // Create tab fill brush
    g.FillPath(&tabBrush, &tabPath); // Fill folder tab contour

    // 4. Center icon: query Windows Shell folder icon or draw stylized folder vector
    SHFILEINFOW sfi = { 0 }; // Initialize shell file info structure
    HICON hIcon = nullptr; // Initialize icon handle
    if (!isParent) { // Query shell for actual folder icon if normal directory
        if (SHGetFileInfoW(folderPath.c_str(), FILE_ATTRIBUTE_DIRECTORY, &sfi, sizeof(sfi), SHGFI_ICON | SHGFI_LARGEICON | SHGFI_USEFILEATTRIBUTES)) { // Query system folder icon
            hIcon = sfi.hIcon; // Assign retrieved icon handle
        } // Close if statement
    } // Close if statement

    if (hIcon != nullptr) { // If shell icon is available
        Gdiplus::Bitmap* shellIconBmp = Gdiplus::Bitmap::FromHICON(hIcon); // Wrap icon handle into GDI+ bitmap
        if (shellIconBmp) { // Ensure bitmap was created
            int iconSize = std::min(targetWidth - 40, 64); // Determine icon drawing dimensions
            int iconX = (targetWidth - iconSize) / 2; // Center icon horizontally
            int iconY = (targetHeight - iconSize) / 2 - 10; // Center icon vertically above text
            g.DrawImage(shellIconBmp, iconX, iconY, iconSize, iconSize); // Draw shell folder icon
            delete shellIconBmp; // Delete temporary wrapper bitmap
        } // Close if statement
        DestroyIcon(hIcon); // Destroy shell icon handle
    } else { // Fallback to vector folder glyph or parent symbol
        if (isParent) { // Render parent navigation symbol
            Gdiplus::Font arrowFont(L"Segoe UI", 32.0f, Gdiplus::FontStyleBold); // Create large font for up arrow
            Gdiplus::SolidBrush arrowBrush(Gdiplus::Color(255, 56, 189, 248)); // Create cyan brush for parent symbol
            Gdiplus::RectF arrowRect(0, (Gdiplus::REAL)(targetHeight / 2 - 45), (Gdiplus::REAL)targetWidth, 50.0f); // Define symbol bounding rectangle
            Gdiplus::StringFormat sf; // Create string formatting options
            sf.SetAlignment(Gdiplus::StringAlignmentCenter); // Center align horizontally
            sf.SetLineAlignment(Gdiplus::StringAlignmentCenter); // Center align vertically
            g.DrawString(L"\x2191", -1, &arrowFont, arrowRect, &sf, &arrowBrush); // Draw up arrow symbol
        } else { // Render styled vector folder shape
            int folderW = targetWidth - 50; // Calculate folder graphic width
            int folderH = (int)(folderW * 0.7f); // Calculate folder graphic height
            int folderX = (targetWidth - folderW) / 2; // Center folder graphic horizontally
            int folderY = (targetHeight - folderH) / 2 - 10; // Center folder graphic vertically
            Gdiplus::SolidBrush fBodyBrush(Gdiplus::Color(255, 245, 158, 11)); // Amber folder body brush
            g.FillRectangle(&fBodyBrush, folderX, folderY + 12, folderW, folderH - 12); // Fill folder body
            Gdiplus::SolidBrush fTabBrush(Gdiplus::Color(255, 217, 119, 6)); // Darker amber folder tab brush
            g.FillRectangle(&fTabBrush, folderX, folderY, folderW / 2, 14); // Fill folder tab
        } // Close else statement
    } // Close if statement

    // 5. Folder Name label inside card
    Gdiplus::Font labelFont(L"Segoe UI Semibold", 9.0f, Gdiplus::FontStyleRegular); // Create font for folder name
    Gdiplus::SolidBrush labelBrush(isParent ? Gdiplus::Color(255, 148, 163, 184) : Gdiplus::Color(255, 254, 243, 199)); // Create label brush
    Gdiplus::RectF labelRect(8.0f, (Gdiplus::REAL)(targetHeight - 55), (Gdiplus::REAL)(targetWidth - 16), 40.0f); // Define text bounding rectangle
    Gdiplus::StringFormat textFormat; // Create string format instance
    textFormat.SetAlignment(Gdiplus::StringAlignmentCenter); // Align text centered horizontally
    textFormat.SetLineAlignment(Gdiplus::StringAlignmentCenter); // Align text centered vertically
    textFormat.SetTrimming(Gdiplus::StringTrimmingEllipsisWord); // Truncate long names with ellipsis
    g.DrawString(displayName.c_str(), -1, &labelFont, labelRect, &textFormat, &labelBrush); // Draw folder name text

    return bmp; // Return generated folder thumbnail bitmap
}

void freePdfThumbnail(Gdiplus::Bitmap*& pBitmap) {
    if (pBitmap != nullptr) {
        delete pBitmap;
        pBitmap = nullptr;
    }
}
