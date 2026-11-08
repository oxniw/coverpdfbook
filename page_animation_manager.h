// ==============================================================================
// File: .//Description-of-file/page_animation_manager.md
// Overview: Header for managing page transition animations and easing curves.
// ==============================================================================

#pragma once

#include "config.h"

// Pure ease-out cubic curve calculation: f(t) = 1 - (1 - t)^3
float calculateEaseOutCubic(float t);

// Starts a page transition animation between fromPage and toPage
void startPageAnimation(PageAnimationState& anim, int fromPage, int toPage, AnimationDirection dir);

// Updates the animation progress based on elapsed time.
// Returns true if animation is still running, false if completed.
bool updatePageAnimation(PageAnimationState& anim);
