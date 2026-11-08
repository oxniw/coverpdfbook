// ==============================================================================
// File: .//Description-of-file/page_animation_manager.md
// Overview: Pure domain implementation of page transition animation and easing curves.
// ==============================================================================

#include "page_animation_manager.h"
#include <algorithm>

float calculateEaseOutCubic(float t) {
    if (t <= 0.0f) return 0.0f;
    if (t >= 1.0f) return 1.0f;
    float inv = 1.0f - t;
    return 1.0f - (inv * inv * inv);
}

void startPageAnimation(PageAnimationState& anim, int fromPage, int toPage, AnimationDirection dir) {
    anim.fromPage = fromPage;
    anim.toPage = toPage;
    anim.direction = dir;
    anim.progress = 0.0f;
    anim.easedProgress = 0.0f;
    anim.startTimeMs = GetTickCount();
    anim.isAnimating = true;
}

bool updatePageAnimation(PageAnimationState& anim) {
    if (!anim.isAnimating) return false;

    DWORD now = GetTickCount();
    DWORD elapsed = now - anim.startTimeMs;

    if (elapsed >= anim.durationMs) {
        anim.progress = 1.0f;
        anim.easedProgress = 1.0f;
        anim.isAnimating = false;
        return false;
    }

    anim.progress = (float)elapsed / (float)anim.durationMs;
    anim.easedProgress = calculateEaseOutCubic(anim.progress);
    return true;
}
