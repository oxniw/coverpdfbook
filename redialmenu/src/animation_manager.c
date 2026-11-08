// ==============================================================================
// File: .//Description-of-file/animation_manager.md
// Overview: Pure domain implementation of easing curves, hover transitions, and pop-in spring.
// ==============================================================================

#include "animation_manager.h"
#include <math.h>

#define SPAWN_DURATION_MS 140.0f
#define SELECT_DURATION_MS 120.0f
#define HOVER_LERP_SPEED 14.0f // units per second

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

float anim_ease_out_cubic(float t) {
    if (t < 0.0f) return 0.0f;
    if (t > 1.0f) return 1.0f;
    float inv = 1.0f - t;
    return 1.0f - (inv * inv * inv);
}

float anim_ease_out_back(float t) {
    if (t < 0.0f) return 0.0f;
    if (t > 1.0f) return 1.0f;
    const float c1 = 1.70158f;
    const float c3 = c1 + 1.0f;
    float inv = t - 1.0f;
    return 1.0f + c3 * (inv * inv * inv) + c1 * (inv * inv);
}

void anim_init(AnimationManager* am) {
    if (!am) return;
    am->scale = 1.0f;
    am->alpha = 1.0f;
    for (int i = 0; i < ANIM_SECTOR_COUNT; ++i) {
        am->hover_weights[i] = 0.0f;
    }
    am->target_sector = RADIAL_SECTOR_NOTHING;
    am->selected_sector = RADIAL_SECTOR_NOTHING;
    am->spawn_start_time_ms = 0;
    am->select_start_time_ms = 0;
    am->last_tick_time_ms = 0;
    am->select_progress = 0.0f;
    am->select_pop = 0.0f;
    am->sector_count = RADIAL_DEFAULT_SECTOR_COUNT;
    am->is_spawning = false;
    am->is_selecting = false;
    am->is_animating = false;
}

void anim_set_sector_count(AnimationManager* am, int count) {
    if (!am) return;
    if (count < 2) count = 2;
    if (count > RADIAL_MAX_SECTORS) count = RADIAL_MAX_SECTORS;
    am->sector_count = count;
}

void anim_start_spawn(AnimationManager* am, uint64_t now_ms) {
    if (!am) return;
    am->spawn_start_time_ms = now_ms;
    am->last_tick_time_ms = now_ms;
    am->is_spawning = true;
    am->is_selecting = false;
    am->is_animating = true;
    am->scale = 0.45f;
    am->alpha = 0.0f;
    am->select_progress = 0.0f;
    am->select_pop = 0.0f;
}

void anim_start_selection(AnimationManager* am, RadialSector sector, uint64_t now_ms) {
    if (!am) return;
    am->selected_sector = sector;
    am->select_start_time_ms = now_ms;
    am->last_tick_time_ms = now_ms;
    am->is_spawning = false;
    am->is_selecting = true;
    am->is_animating = true;
    am->select_progress = 0.0f;
    am->select_pop = 0.0f;

    // Lock selected sector hover weight
    for (int i = 0; i < ANIM_SECTOR_COUNT; ++i) {
        am->hover_weights[i] = ((int)sector == i) ? 1.0f : 0.0f;
    }
}

bool anim_is_selection_complete(AnimationManager* am) {
    if (!am) return true;
    return (!am->is_selecting && am->select_progress >= 1.0f);
}

void anim_set_target_sector(AnimationManager* am, RadialSector sector) {
    if (!am) return;
    if (am->is_selecting) return; // ignore hover changes during selection confirmation
    if (am->target_sector != sector) {
        am->target_sector = sector;
        am->is_animating = true;
    }
}

int anim_tick(AnimationManager* am, uint64_t now_ms) {
    if (!am) return 0;
    if (!am->is_animating) return 0;

    float dt = 0.016f; // default 60fps delta fallback
    if (am->last_tick_time_ms > 0 && now_ms > am->last_tick_time_ms) {
        dt = (float)(now_ms - am->last_tick_time_ms) / 1000.0f;
        if (dt > 0.05f) dt = 0.05f; // clamp delta
    }
    am->last_tick_time_ms = now_ms;

    int changed = 0;

    // 1. Process selection dismiss / confirmation pop animation
    if (am->is_selecting) {
        float elapsed = (float)(now_ms - am->select_start_time_ms);
        float p = elapsed / SELECT_DURATION_MS;

        if (p >= 1.0f) {
            am->is_selecting = false;
            am->alpha = 0.0f;
            am->select_progress = 1.0f;
            am->select_pop = 0.0f;
            am->is_animating = false;
        } else {
            am->select_progress = p;
            am->select_pop = sinf(p * (float)M_PI);
            if (am->selected_sector != RADIAL_SECTOR_NOTHING) {
                // Expanding burst + smooth fade
                am->scale = 1.0f + 0.07f * am->select_pop;
                am->alpha = 1.0f - anim_ease_out_cubic(p);
            } else {
                // Subtle shrink + fade for cancel
                am->scale = 1.0f - 0.20f * anim_ease_out_cubic(p);
                am->alpha = 1.0f - anim_ease_out_cubic(p);
            }
        }
        return 1;
    }

    // 2. Process spawn animation
    if (am->is_spawning) {
        float elapsed = (float)(now_ms - am->spawn_start_time_ms);
        float progress = elapsed / SPAWN_DURATION_MS;

        if (progress >= 1.0f) {
            am->scale = 1.0f;
            am->alpha = 1.0f;
            am->is_spawning = false;
        } else {
            am->scale = 0.45f + 0.55f * anim_ease_out_back(progress);
            am->alpha = anim_ease_out_cubic(progress);
        }
        changed = 1;
    }

    // 3. Process smooth hover transition per sector
    for (int i = 0; i < ANIM_SECTOR_COUNT; ++i) {
        float target = ((int)am->target_sector == i) ? 1.0f : 0.0f;
        float current = am->hover_weights[i];
        float delta = target - current;

        if (fabsf(delta) > 0.005f) {
            float step = delta * HOVER_LERP_SPEED * dt;
            if (fabsf(step) > fabsf(delta)) {
                am->hover_weights[i] = target;
            } else {
                am->hover_weights[i] += step;
            }
            changed = 1;
        } else {
            am->hover_weights[i] = target;
        }
    }

    // Check if still animating
    bool stillAnimating = am->is_spawning || am->is_selecting;
    for (int i = 0; i < ANIM_SECTOR_COUNT; ++i) {
        float target = ((int)am->target_sector == i) ? 1.0f : 0.0f;
        if (fabsf(am->hover_weights[i] - target) > 0.001f) {
            stillAnimating = true;
            break;
        }
    }
    am->is_animating = stillAnimating;

    return changed;
}
