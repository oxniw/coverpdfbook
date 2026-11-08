// ==============================================================================
// File: .//Description-of-file/test_geometry.md
// Overview: Pure domain unit test for mathematical geometry and sector classification.
// ==============================================================================

#include "src/math_geometry.h"
#include "src/menu_state.h"
#include "src/animation_manager.h"
#include <stdio.h>
#include <assert.h>
#include <math.h>

int main(void) {
    printf("[TEST] Running geometry & state machine unit tests...\n");

    // Test easing curves
    assert(fabs(anim_ease_out_cubic(0.0f) - 0.0f) < 0.001f);
    assert(fabs(anim_ease_out_cubic(1.0f) - 1.0f) < 0.001f);
    assert(fabs(anim_ease_out_back(0.0f) - 0.0f) < 0.001f);
    assert(fabs(anim_ease_out_back(1.0f) - 1.0f) < 0.001f);

    // Test AnimationManager
    AnimationManager am;
    anim_init(&am);
    assert(am.scale == 1.0f);
    assert(am.is_animating == false);

    anim_start_spawn(&am, 1000);
    assert(am.is_spawning == true);
    assert(am.scale < 0.5f);

    // Advance 50ms
    int changed1 = anim_tick(&am, 1050);
    assert(changed1 == 1);
    assert(am.scale > 0.45f);

    // Advance past completion
    int changed2 = anim_tick(&am, 1200);
    assert(changed2 == 1);
    assert(am.is_spawning == false);
    assert(fabs(am.scale - 1.0f) < 0.001f);

    // Test selection confirmation animation
    anim_start_selection(&am, RADIAL_SECTOR_FUNCTION1, 2000);
    assert(am.is_selecting == true);
    assert(am.selected_sector == RADIAL_SECTOR_FUNCTION1);

    // Mid-selection pop
    int changed3 = anim_tick(&am, 2060);
    assert(changed3 == 1);
    assert(am.scale > 1.0f); // Pop expansion
    assert(am.alpha < 1.0f); // Fading out

    // End of selection
    int changed4 = anim_tick(&am, 2150);
    assert(changed4 == 1);
    assert(anim_is_selection_complete(&am) == true);
    assert(am.alpha == 0.0f);

    // Test distance
    double d = math_distance(0, 0, 3, 4);
    assert(fabs(d - 5.0) < 0.0001);

    // Test dead zone (Nothing)
    GeometryResult r0 = math_resolve_sector(100, 100, 110, 110, 45.0);
    assert(r0.sector == RADIAL_SECTOR_NOTHING);

    // Test Top-Right: function1 (dx > 0, dy < 0)
    GeometryResult r1 = math_resolve_sector(100, 100, 160, 40, 45.0);
    assert(r1.sector == RADIAL_SECTOR_FUNCTION1);

    // Test Bottom-Right: function2 (dx > 0, dy > 0)
    GeometryResult r2 = math_resolve_sector(100, 100, 160, 160, 45.0);
    assert(r2.sector == RADIAL_SECTOR_FUNCTION2);

    // Test Bottom-Left: function3 (dx < 0, dy > 0)
    GeometryResult r3 = math_resolve_sector(100, 100, 40, 160, 45.0);
    assert(r3.sector == RADIAL_SECTOR_FUNCTION3);

    // Test Top-Left: function4 (dx < 0, dy < 0)
    GeometryResult r4 = math_resolve_sector(100, 100, 40, 40, 45.0);
    assert(r4.sector == RADIAL_SECTOR_FUNCTION4);

    // Test MenuStateMachine lifecycle
    MenuStateMachine sm;
    menu_state_init(&sm, 18.0f, 45.0f);
    assert(sm.state == GESTURE_STATE_IDLE);

    // Mouse down
    menu_state_on_down(&sm, 200, 200);
    assert(sm.state == GESTURE_STATE_PENDING);

    // Small move (within drag threshold)
    int act1 = menu_state_on_move(&sm, 205, 205);
    assert(act1 == 0);
    assert(sm.state == GESTURE_STATE_PENDING);

    // Move past threshold into function2 (Bottom-Right)
    int act2 = menu_state_on_move(&sm, 270, 270);
    assert(act2 == 1);
    assert(sm.state == GESTURE_STATE_ACTIVE);
    assert(sm.active_sector == RADIAL_SECTOR_FUNCTION2);

    // Mouse up
    RadialSector confirmed = RADIAL_SECTOR_NOTHING;
    int wasGesture = menu_state_on_up(&sm, &confirmed);
    assert(wasGesture == 1);
    assert(confirmed == RADIAL_SECTOR_FUNCTION2);
    assert(sm.state == GESTURE_STATE_IDLE);

    // Test dynamic N sectors (N = 6 and N = 8)
    // For N = 6, sectors are 60 degrees each, starting from 12 o'clock (-90 deg / 270 deg)
    // Sector 1: [270, 330) (e.g. angle 300)
    // Sector 2: [330, 30) (e.g. angle 0)
    GeometryResult rn6_s1 = math_resolve_sector_n(100, 100, 150, 20, 45.0, 6);
    assert(rn6_s1.sector == (RadialSector)1);

    GeometryResult rn8 = math_resolve_sector_n(100, 100, 100, 200, 45.0, 8); // 6 o'clock (angle 90 deg)
    // At 6 o'clock: offset from 12 o'clock is 180 deg. For N=8, sweep=45 deg: 1 + floor(180/45) = 1 + 4 = 5!
    assert(rn8.sector == (RadialSector)5);

    printf("[SUCCESS] All pure domain unit tests passed successfully!\n");
    return 0;
}
