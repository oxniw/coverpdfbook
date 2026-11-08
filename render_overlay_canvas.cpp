// ==============================================================================
// File: .//Description-of-file/render_overlay_canvas.md
// Overview: Implements double-buffered GDI+ rendering of the transparent overlay
//           with pure transparent background, glowing hover cards, and smooth page slide animations.
// ==============================================================================

#include "render_overlay_canvas.h"
#include "extract_pdf_cover.h"
#include <windows.h>
#include <gdiplus.h>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cmath>

namespace {
    // Persistent offscreen DIB backbuffer resources (Zero-Copy allocation)
    HDC g_memDC = NULL;
    HBITMAP g_hDIB = NULL;
    HBITMAP g_hOldBmp = NULL;
    void* g_pBits = nullptr;
    int g_bufWidth = 0;
    int g_bufHeight = 0;

    // Page snapshot cache for zero-lag page transition animations
    Gdiplus::Bitmap* g_pageFromBitmap = nullptr;
    Gdiplus::Bitmap* g_pageToBitmap = nullptr;
    int g_cachedFromPage = -1;
    int g_cachedToPage = -1;

    void ensureDIBSection(int width, int height) {
        if (g_memDC && g_hDIB && g_bufWidth == width && g_bufHeight == height) {
            return;
        }

        if (g_memDC) {
            if (g_hOldBmp) SelectObject(g_memDC, g_hOldBmp);
            if (g_hDIB) DeleteObject(g_hDIB);
            DeleteDC(g_memDC);
            g_memDC = NULL;
            g_hDIB = NULL;
            g_pBits = nullptr;
        }

        g_bufWidth = width;
        g_bufHeight = height;

        HDC screenDC = GetDC(NULL);
        g_memDC = CreateCompatibleDC(screenDC);

        BITMAPINFO bmi = { 0 };
        bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bmi.bmiHeader.biWidth = width;
        bmi.bmiHeader.biHeight = -height; // Top-down DIB so scanline 0 is top
        bmi.bmiHeader.biPlanes = 1;
        bmi.bmiHeader.biBitCount = 32;
        bmi.bmiHeader.biCompression = BI_RGB;

        g_hDIB = CreateDIBSection(screenDC, &bmi, DIB_RGB_COLORS, &g_pBits, NULL, 0);
        g_hOldBmp = (HBITMAP)SelectObject(g_memDC, g_hDIB);
        ReleaseDC(NULL, screenDC);
    }

    // Helper to draw rounded rectangle paths
    void addRoundedRect(Gdiplus::GraphicsPath& path, Gdiplus::RectF rect, float radius) {
        float diameter = radius * 2.0f;
        float right = rect.X + rect.Width;
        float bottom = rect.Y + rect.Height;
        path.AddArc(rect.X, rect.Y, diameter, diameter, 180.0f, 90.0f);
        path.AddArc(right - diameter, rect.Y, diameter, diameter, 270.0f, 90.0f);
        path.AddArc(right - diameter, bottom - diameter, diameter, diameter, 0.0f, 90.0f);
        path.AddArc(rect.X, bottom - diameter, diameter, diameter, 90.0f, 90.0f);
        path.CloseFigure();
    }

    void drawGridPage(
        Gdiplus::Graphics& g,
        std::vector<PdfItem>& items,
        const AppSettings& settings,
        const GridLayoutResult& layout,
        const std::vector<RECT>& itemRects,
        float offsetX,
        float alphaMultiplier
    ) {
        if (alphaMultiplier <= 0.01f) return;
        BYTE alphaByte = (BYTE)(255 * std::clamp(alphaMultiplier, 0.0f, 1.0f));

        // Prepare GDI+ ImageAttributes for smooth fade if alphaMultiplier < 1.0f
        Gdiplus::ColorMatrix cm = {
            1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 0.0f, alphaMultiplier, 0.0f,
            0.0f, 0.0f, 0.0f, 0.0f, 1.0f
        };
        Gdiplus::ImageAttributes imgAttr;
        imgAttr.SetColorMatrix(&cm, Gdiplus::ColorMatrixFlagsDefault, Gdiplus::ColorAdjustTypeBitmap);

        for (int i = 0; i < layout.totalVisible && i < (int)itemRects.size(); ++i) {
            int globalIndex = layout.startIndex + i;
            if (globalIndex >= (int)items.size()) break;

            PdfItem& item = items[globalIndex];
            const RECT& r = itemRects[i];
            bool isHovered = (settings.hoveredIndex == globalIndex);

            // Lazy extract/render thumbnail if not yet cached or if size changed
            if (!item.thumbnail || item.thumbnail->GetWidth() != (UINT)settings.coverWidth) { // Check if thumbnail missing or resized
                if (item.thumbnail) freePdfThumbnail(item.thumbnail); // Free previous thumbnail bitmap if present
                if (item.type == ITEM_TYPE_FOLDER || item.type == ITEM_TYPE_PARENT) { // Check if item is folder or parent
                    item.thumbnail = extractFolderThumbnail(item.filePath, item.displayName, settings.coverWidth, settings.coverHeight, item.type == ITEM_TYPE_PARENT); // Generate folder thumbnail card
                } else { // Standard PDF document item
                    item.thumbnail = extractPdfCover(item.filePath, item.displayName, settings.coverWidth, settings.coverHeight); // Extract PDF cover thumbnail
                } // Close item type condition
            } // Close thumbnail extraction condition

            int cardX = r.left + (int)offsetX;
            int cardY = r.top;
            int cardW = settings.coverWidth;
            int cardH = settings.coverHeight;

            // Fast, optimized drop shadow underneath each floating book cover
            for (int s = 3; s >= 1; --s) {
                Gdiplus::RectF shadowRect(
                    (Gdiplus::REAL)(cardX - s + 1),
                    (Gdiplus::REAL)(cardY - s + 3),
                    (Gdiplus::REAL)(cardW + s * 2),
                    (Gdiplus::REAL)(cardH + s * 2)
                );
                Gdiplus::GraphicsPath shadowPath;
                addRoundedRect(shadowPath, shadowRect, 8.0f);
                BYTE shadowAlpha = (BYTE)((30 * (4 - s)) * alphaMultiplier);
                Gdiplus::SolidBrush shadowBrush(Gdiplus::Color(shadowAlpha, 0, 0, 0));
                g.FillPath(&shadowBrush, &shadowPath);
            }

            // Draw PDF first-page cover thumbnail
            if (item.thumbnail) {
                Gdiplus::Rect destRect(cardX, cardY, cardW, cardH);
                g.DrawImage(item.thumbnail, destRect, 0, 0, item.thumbnail->GetWidth(), item.thumbnail->GetHeight(), Gdiplus::UnitPixel, &imgAttr);
            }

            // Glowing border on hover
            if (isHovered) {
                Gdiplus::Pen hoverPen(Gdiplus::Color(alphaByte, 56, 189, 248), 3.0f);
                g.DrawRectangle(&hoverPen, cardX - 1, cardY - 1, cardW + 2, cardH + 2);
            } else {
                Gdiplus::Pen cardBorder(Gdiplus::Color((BYTE)(140 * alphaMultiplier), 255, 255, 255), 1.0f);
                g.DrawRectangle(&cardBorder, cardX, cardY, cardW, cardH);
            }

            // Title pill badge below cover
            int titlePillH = 26;
            int titlePillY = cardY + cardH + 6;
            Gdiplus::RectF titlePillRect((Gdiplus::REAL)cardX, (Gdiplus::REAL)titlePillY, (Gdiplus::REAL)cardW, (Gdiplus::REAL)titlePillH);
            Gdiplus::GraphicsPath titlePillPath;
            addRoundedRect(titlePillPath, titlePillRect, 6.0f);

            Gdiplus::SolidBrush titlePillBg(Gdiplus::Color((BYTE)(200 * alphaMultiplier), 15, 23, 42));
            g.FillPath(&titlePillBg, &titlePillPath);

            Gdiplus::Font itemTitleFont(L"Segoe UI Semibold", 8.5f, Gdiplus::FontStyleRegular);
            Gdiplus::SolidBrush itemTitleBrush(isHovered ? Gdiplus::Color(alphaByte, 56, 189, 248) : Gdiplus::Color(alphaByte, 241, 245, 249));

            Gdiplus::StringFormat titleFormat;
            titleFormat.SetAlignment(Gdiplus::StringAlignmentCenter);
            titleFormat.SetLineAlignment(Gdiplus::StringAlignmentCenter);
            titleFormat.SetTrimming(Gdiplus::StringTrimmingEllipsisWord);

            std::wstring pillText = item.displayName; // Initialize pill text with item display name
            if (item.type == ITEM_TYPE_FOLDER) { // Check if item represents a directory
                pillText = L"[Folder] " + item.displayName; // Prefix folder tag for clarity
            } else if (item.type == ITEM_TYPE_PARENT) { // Check if item represents parent directory navigation
                pillText = L"^ .. (Parent)"; // Prefix parent directory label
            } // Close if statement
            g.DrawString(pillText.c_str(), -1, &itemTitleFont, titlePillRect, &titleFormat, &itemTitleBrush); // Draw formatted title string inside pill badge
        }
    }
}

void invalidatePageCache() {
    if (g_pageFromBitmap) {
        delete g_pageFromBitmap;
        g_pageFromBitmap = nullptr;
    }
    if (g_pageToBitmap) {
        delete g_pageToBitmap;
        g_pageToBitmap = nullptr;
    }
    g_cachedFromPage = -1;
    g_cachedToPage = -1;
}

void cleanupRenderResources() {
    invalidatePageCache();
    if (g_memDC) {
        if (g_hOldBmp) SelectObject(g_memDC, g_hOldBmp);
        if (g_hDIB) DeleteObject(g_hDIB);
        DeleteDC(g_memDC);
        g_memDC = NULL;
        g_hDIB = NULL;
        g_pBits = nullptr;
        g_bufWidth = 0;
        g_bufHeight = 0;
    }
}

void renderOverlayCanvas(
    HWND hwndOverlay,
    const AppSettings& settings,
    std::vector<PdfItem>& items,
    const GridLayoutResult& layout,
    const std::vector<RECT>& itemRects,
    const PageAnimationState* pAnim
) {
    if (!hwndOverlay || !IsWindow(hwndOverlay)) return;

    RECT clientRect;
    GetClientRect(hwndOverlay, &clientRect);
    int width = clientRect.right - clientRect.left;
    int height = clientRect.bottom - clientRect.top;
    if (width <= 0 || height <= 0) return;

    ensureDIBSection(width, height);
    if (!g_pBits || !g_memDC) return;

    // Wrap the persistent DIBSection's pixel memory directly - ZERO heap allocations!
    Gdiplus::Bitmap backBuffer(width, height, width * 4, PixelFormat32bppPARGB, (BYTE*)g_pBits);
    Gdiplus::Graphics g(&backBuffer);

    // Completely transparent background
    g.Clear(Gdiplus::Color(0, 0, 0, 0));

    if (items.empty()) {
        invalidatePageCache();
        Gdiplus::SolidBrush whiteBrush(Gdiplus::Color(255, 248, 250, 252));
        Gdiplus::Font emptyFont(L"Segoe UI", 13.0f, Gdiplus::FontStyleBold);
        Gdiplus::RectF emptyRect(0, (Gdiplus::REAL)(height / 2 - 30), (Gdiplus::REAL)width, 60.0f);
        Gdiplus::StringFormat emptyFormat;
        emptyFormat.SetAlignment(Gdiplus::StringAlignmentCenter);
        emptyFormat.SetLineAlignment(Gdiplus::StringAlignmentCenter);
        g.DrawString(L"No PDF files found in this folder.\nPlace .pdf files there and press [R] to reload.", -1, &emptyFont, emptyRect, &emptyFormat, &whiteBrush);
    } else if (pAnim != nullptr && pAnim->isAnimating) {
        // GPU/SurfaceFlinger-style Texture Caching for zero-lag page transition animations
        // 1. Snapshot outgoing page ONCE at 1:1 pixel resolution
        if (g_cachedFromPage != pAnim->fromPage || !g_pageFromBitmap) {
            if (g_pageFromBitmap) { delete g_pageFromBitmap; g_pageFromBitmap = nullptr; }
            g_pageFromBitmap = new Gdiplus::Bitmap(width, height, PixelFormat32bppPARGB);
            Gdiplus::Graphics gFrom(g_pageFromBitmap);
            gFrom.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
            gFrom.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
            gFrom.SetTextRenderingHint(Gdiplus::TextRenderingHintClearTypeGridFit);
            gFrom.Clear(Gdiplus::Color(0, 0, 0, 0));

            std::vector<RECT> fromRects;
            GridLayoutResult fromLayout = calculateGridLayoutForPage(settings, pAnim->fromPage, (int)items.size(), width, height, fromRects);
            drawGridPage(gFrom, items, settings, fromLayout, fromRects, 0.0f, 1.0f);
            gFrom.Flush(Gdiplus::FlushIntentionSync);
            g_cachedFromPage = pAnim->fromPage;
        }

        // 2. Snapshot incoming page ONCE at 1:1 pixel resolution
        if (g_cachedToPage != pAnim->toPage || !g_pageToBitmap) {
            if (g_pageToBitmap) { delete g_pageToBitmap; g_pageToBitmap = nullptr; }
            g_pageToBitmap = new Gdiplus::Bitmap(width, height, PixelFormat32bppPARGB);
            Gdiplus::Graphics gTo(g_pageToBitmap);
            gTo.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
            gTo.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
            gTo.SetTextRenderingHint(Gdiplus::TextRenderingHintClearTypeGridFit);
            gTo.Clear(Gdiplus::Color(0, 0, 0, 0));

            std::vector<RECT> toRects;
            GridLayoutResult toLayout = calculateGridLayoutForPage(settings, pAnim->toPage, (int)items.size(), width, height, toRects);
            drawGridPage(gTo, items, settings, toLayout, toRects, 0.0f, 1.0f);
            gTo.Flush(Gdiplus::FlushIntentionSync);
            g_cachedToPage = pAnim->toPage;
        }

        float slideDist = (float)width * 0.40f;
        float dirSign = (pAnim->direction == ANIM_DIR_NEXT) ? 1.0f : -1.0f;

        float offsetOut = -pAnim->easedProgress * slideDist * dirSign;
        float alphaOut = 1.0f - pAnim->easedProgress;

        float offsetIn = (1.0f - pAnim->easedProgress) * slideDist * dirSign;
        float alphaIn = pAnim->easedProgress;

        // Blit cached textures directly with NearestNeighbor interpolation: zero bicubic calculations per frame!
        g.SetInterpolationMode(Gdiplus::InterpolationModeNearestNeighbor);

        // Blit Outgoing Page
        if (alphaOut > 0.01f && g_pageFromBitmap) {
            Gdiplus::ColorMatrix cmOut = {
                1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
                0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 0.0f, alphaOut, 0.0f,
                0.0f, 0.0f, 0.0f, 0.0f, 1.0f
            };
            Gdiplus::ImageAttributes attrOut;
            attrOut.SetColorMatrix(&cmOut, Gdiplus::ColorMatrixFlagsDefault, Gdiplus::ColorAdjustTypeBitmap);
            int dstX = (int)std::round(offsetOut);
            g.DrawImage(g_pageFromBitmap, Gdiplus::Rect(dstX, 0, width, height), 0, 0, width, height, Gdiplus::UnitPixel, &attrOut);
        }

        // Blit Incoming Page
        if (alphaIn > 0.01f && g_pageToBitmap) {
            Gdiplus::ColorMatrix cmIn = {
                1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
                0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 0.0f, alphaIn, 0.0f,
                0.0f, 0.0f, 0.0f, 0.0f, 1.0f
            };
            Gdiplus::ImageAttributes attrIn;
            attrIn.SetColorMatrix(&cmIn, Gdiplus::ColorMatrixFlagsDefault, Gdiplus::ColorAdjustTypeBitmap);
            int dstX = (int)std::round(offsetIn);
            g.DrawImage(g_pageToBitmap, Gdiplus::Rect(dstX, 0, width, height), 0, 0, width, height, Gdiplus::UnitPixel, &attrIn);
        }
    } else {
        // Animation finished or static view: clear snapshot cache to free RAM
        invalidatePageCache();

        g.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
        g.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
        g.SetTextRenderingHint(Gdiplus::TextRenderingHintClearTypeGridFit);
        drawGridPage(g, items, settings, layout, itemRects, 0.0f, 1.0f);
    }

    g.Flush(Gdiplus::FlushIntentionSync);

    // Blit directly to screen via Win32 UpdateLayeredWindow from persistent g_memDC - Zero GetHBITMAP copies!
    HDC screenDC = GetDC(NULL);
    POINT ptSrc = { 0, 0 };
    SIZE sizeWnd = { width, height };

    POINT ptDst;
    RECT wndRect;
    GetWindowRect(hwndOverlay, &wndRect);
    ptDst.x = wndRect.left;
    ptDst.y = wndRect.top;

    BLENDFUNCTION blend = { AC_SRC_OVER, 0, settings.overlayAlpha, AC_SRC_ALPHA };
    UpdateLayeredWindow(hwndOverlay, screenDC, &ptDst, &sizeWnd, g_memDC, &ptSrc, 0, &blend, ULW_ALPHA);
    ReleaseDC(NULL, screenDC);
}
