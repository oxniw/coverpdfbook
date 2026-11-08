// ==============================================================================
// File: .//Description-of-file/render_overlay.md
// Overview: High-DPI anti-aliased GDI+ rendering of donut radial menu & sectors.
// ==============================================================================

#include "render_overlay.h"
#include "../include/config.h"
#include <gdiplus.h>
#include <math.h>
#include <stdio.h>
#include <wchar.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

using namespace Gdiplus;

static ULONG_PTR g_gdiplusToken = 0;
static int g_sectorCount = RADIAL_DEFAULT_SECTOR_COUNT;
static wchar_t g_sectorLabels[RADIAL_MAX_SECTORS + 1][64] = {};

int render_init_gdiplus(void) {
    GdiplusStartupInput gdiplusStartupInput;
    Status status = GdiplusStartup(&g_gdiplusToken, &gdiplusStartupInput, NULL);
    if (status != Ok) return 0;

    // Set default labels
    wcsncpy(g_sectorLabels[0], L"Nothing", 63);
    for (int i = 1; i <= RADIAL_MAX_SECTORS; ++i) {
        _snwprintf(g_sectorLabels[i], 64, L"function%d", i);
    }

    return 1;
}

void render_set_sector_count(int count) {
    if (count < 2) count = 2;
    if (count > RADIAL_MAX_SECTORS) count = RADIAL_MAX_SECTORS;
    g_sectorCount = count;
}

void render_set_sector_label(int sector_idx, const wchar_t* label) {
    if (sector_idx >= 1 && sector_idx <= RADIAL_MAX_SECTORS && label != NULL) {
        wcsncpy(g_sectorLabels[sector_idx], label, 63);
        g_sectorLabels[sector_idx][63] = L'\0';
    }
}

void render_shutdown_gdiplus(void) {
    if (g_gdiplusToken != 0) {
        GdiplusShutdown(g_gdiplusToken);
        g_gdiplusToken = 0;
    }
}

static Color lerp_color(const Color& c1, const Color& c2, float t) {
    if (t <= 0.0f) return c1;
    if (t >= 1.0f) return c2;
    BYTE a = (BYTE)(c1.GetA() + t * (c2.GetA() - c1.GetA()));
    BYTE r = (BYTE)(c1.GetR() + t * (c2.GetR() - c1.GetR()));
    BYTE g = (BYTE)(c1.GetG() + t * (c2.GetG() - c1.GetG()));
    BYTE b = (BYTE)(c1.GetB() + t * (c2.GetB() - c1.GetB()));
    return Color(a, r, g, b);
}

void render_draw_menu(HDC hdcTarget, int width, int height, float scale, const float* hover_weights) {
    Graphics graphics(hdcTarget);
    graphics.SetSmoothingMode(SmoothingModeAntiAlias);
    graphics.SetTextRenderingHint(TextRenderingHintAntiAliasGridFit);

    // Clear background to fully transparent
    graphics.Clear(Color(0, 0, 0, 0));

    if (scale <= 0.05f) {
        return;
    }

    float cx = width / 2.0f;
    float cy = height / 2.0f;
    float r_out = RADIAL_OUTER_RADIUS_PX * scale;
    float r_in = RADIAL_INNER_RADIUS_PX * scale;

    // Colors
    Color defaultFill(240, 255, 255, 255);
    Color hoverFill(245, 76, 175, 80);        // Material Green highlight
    Color strokeColor(255, 30, 30, 30);      // High-contrast border
    Color centerDefaultFill(245, 248, 249, 250);
    Color centerHoverFill(245, 239, 83, 80);  // Cancel red highlight
    Color textNormalColor(255, 30, 30, 30);
    Color textHoverColor(255, 255, 255, 255);

    Pen borderPen(strokeColor, RADIAL_BORDER_STROKE_WIDTH);
    Pen dividerPen(strokeColor, RADIAL_BORDER_STROKE_WIDTH);

    int N = g_sectorCount;
    float sweepAngle = 360.0f / (float)N;

    // Draw the N outer pie slices with dynamic 1.2x size scaling when selected/hovered
    for (int i = 0; i < N; ++i) {
        int secIdx = i + 1; // 1-indexed for sectors 1..N
        float startAngle = 270.0f + i * sweepAngle;
        float startRad = startAngle * (float)(M_PI / 180.0);
        float endRad = (startAngle + sweepAngle) * (float)(M_PI / 180.0);

        float w = (hover_weights != NULL && secIdx >= 0 && secIdx <= RADIAL_MAX_SECTORS) ? hover_weights[secIdx] : 0.0f;

        // Active slice smoothly expands outward up to 1.2x size
        float sliceSizeFactor = 1.0f + 0.20f * w;
        float r_out_i = r_out * sliceSizeFactor;

        RectF sliceBounds(cx - r_out_i, cy - r_out_i, r_out_i * 2.0f, r_out_i * 2.0f);
        Color sliceColor = lerp_color(defaultFill, hoverFill, w);
        SolidBrush fillBrush(sliceColor);
        graphics.FillPie(&fillBrush, sliceBounds, startAngle, sweepAngle);

        // Draw outer arc
        graphics.DrawArc(&borderPen, sliceBounds, startAngle, sweepAngle);

        // Draw radial bounding divider lines extending to r_out_i
        float x1_start = cx + r_in * cosf(startRad);
        float y1_start = cy + r_in * sinf(startRad);
        float x1_end   = cx + r_out_i * cosf(startRad);
        float y1_end   = cy + r_out_i * sinf(startRad);
        graphics.DrawLine(&dividerPen, x1_start, y1_start, x1_end, y1_end);

        float x2_start = cx + r_in * cosf(endRad);
        float y2_start = cy + r_in * sinf(endRad);
        float x2_end   = cx + r_out_i * cosf(endRad);
        float y2_end   = cy + r_out_i * sinf(endRad);
        graphics.DrawLine(&dividerPen, x2_start, y2_start, x2_end, y2_end);
    }

    // Inner circle fill & outline with smooth 1.2x cancel blend
    float centerWeight = (hover_weights != NULL) ? hover_weights[0] : 0.0f;
    float centerSizeFactor = 1.0f + 0.20f * centerWeight;
    float r_in_eff = r_in * centerSizeFactor;
    RectF innerBounds(cx - r_in_eff, cy - r_in_eff, r_in_eff * 2.0f, r_in_eff * 2.0f);

    Color centerColor = lerp_color(centerDefaultFill, centerHoverFill, centerWeight);
    SolidBrush centerBrush(centerColor);
    graphics.FillEllipse(&centerBrush, innerBounds);
    graphics.DrawEllipse(&borderPen, innerBounds);

    // Setup typography (font size scales gracefully with sector density and 1.2x boost)
    float densityFactor = (N > 4) ? (4.0f / (float)N) : 1.0f;
    if (densityFactor < 0.65f) densityFactor = 0.65f;
    float baseLabelFontSize = 13.0f * (scale < 0.8f ? 0.8f : scale) * densityFactor;
    float baseCenterFontSize = 12.0f * (scale < 0.8f ? 0.8f : scale) * centerSizeFactor;
    FontFamily fontFamily(L"Segoe UI");
    Font centerFont(&fontFamily, baseCenterFontSize, FontStyleBold, UnitPixel);
    StringFormat format;
    format.SetAlignment(StringAlignmentCenter);
    format.SetLineAlignment(StringAlignmentCenter);

    // Draw slice labels with smoothly blended text colors & 1.2x text scale
    for (int i = 0; i < N; ++i) {
        int secIdx = i + 1;
        float startAngle = 270.0f + i * sweepAngle;
        float midAngle = startAngle + sweepAngle * 0.5f;
        float textAngle = midAngle * (float)(M_PI / 180.0);

        float w = (hover_weights != NULL && secIdx >= 0 && secIdx <= RADIAL_MAX_SECTORS) ? hover_weights[secIdx] : 0.0f;
        float sliceSizeFactor = 1.0f + 0.20f * w;
        float r_out_i = r_out * sliceSizeFactor;

        Color textColor = lerp_color(textNormalColor, textHoverColor, w);
        SolidBrush textBrush(textColor);

        float textRadius = (r_in + r_out_i) * 0.52f;
        float tx = cx + textRadius * cosf(textAngle);
        float ty = cy + textRadius * sinf(textAngle);

        float currentFontSize = baseLabelFontSize * (1.0f + 0.20f * w);
        Font labelFont(&fontFamily, currentFontSize, FontStyleBold, UnitPixel);

        RectF textRect(tx - 50.0f * scale * sliceSizeFactor,
                       ty - 16.0f * scale * sliceSizeFactor,
                       100.0f * scale * sliceSizeFactor,
                       32.0f * scale * sliceSizeFactor);
        const wchar_t* labelStr = (g_sectorLabels[secIdx][0] != L'\0') ? g_sectorLabels[secIdx] : L"";
        graphics.DrawString(labelStr, -1, &labelFont, textRect, &format, &textBrush);
    }

    // Draw center label "Nothing"
    Color centerTextColor = lerp_color(textNormalColor, textHoverColor, centerWeight);
    SolidBrush centerTextBrush(centerTextColor);
    RectF centerRect(cx - r_in_eff, cy - r_in_eff, r_in_eff * 2.0f, r_in_eff * 2.0f);
    graphics.DrawString(L"Nothing", -1, &centerFont, centerRect, &format, &centerTextBrush);
}
