// ==============================================================================
// File: .//Description-of-file/animation_manager.md
// Overview: Header for pure animation controller, easing curves, and sector interpolation.
// ==============================================================================

#ifndef ANIMATION_MANAGER_H
#define ANIMATION_MANAGER_H

#include "../include/radial_types.h"
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#define ANIM_SECTOR_COUNT (RADIAL_MAX_SECTORS + 1)

typedef struct {
    float scale;                    // Current outer/inner radius scale multiplier
    float alpha;                    // Current window master opacity [0.0, 1.0]
    float hover_weights[ANIM_SECTOR_COUNT]; // Smooth interpolation weight [0.0, 1.0] per sector
    RadialSector target_sector;     // Current hovered sector
    RadialSector selected_sector;   // Sector selected upon mouse-up
    uint64_t spawn_start_time_ms;   // Timestamp when spawn began
    uint64_t select_start_time_ms;  // Timestamp when selection animation began
    uint64_t last_tick_time_ms;     // Timestamp of last frame tick
    float select_progress;          // 0.0 to 1.0 during selection dismiss
    float select_pop;               // Pulse burst factor for selected sector
    int sector_count;               // Configured outer sector count N
    bool is_spawning;               // True while pop-in animation is active
    bool is_selecting;              // True while selection confirmation pop is active
    bool is_animating;              // True if any transition is actively progressing
} AnimationManager;

/**
 * @brief Initialize animation manager state.
 */
void anim_init(AnimationManager* am);

/**
 * @brief Set outer sector count N for animation.
 */
void anim_set_sector_count(AnimationManager* am, int count);

/**
 * @brief Trigger the pop-in / expand spawn animation.
 */
void anim_start_spawn(AnimationManager* am, uint64_t now_ms);

/**
 * @brief Trigger selection confirmation pop & fade-out animation.
 */
void anim_start_selection(AnimationManager* am, RadialSector sector, uint64_t now_ms);

/**
 * @brief Set the target sector to animate smooth hover transitions.
 */
void anim_set_target_sector(AnimationManager* am, RadialSector sector);

/**
 * @brief Advance animation by current timestamp.
 * @return 1 if visuals changed and redraw is required, 0 if steady state.
 */
int anim_tick(AnimationManager* am, uint64_t now_ms);

/**
 * @brief Returns true if selection animation has completed and window should hide.
 */
bool anim_is_selection_complete(AnimationManager* am);

/**
 * @brief Cubic ease-out curve.
 */
float anim_ease_out_cubic(float t);

/**
 * @brief Back ease-out curve with slight spring overshoot.
 */
float anim_ease_out_back(float t);

#ifdef __cplusplus
}
#endif

#endif // ANIMATION_MANAGER_H
