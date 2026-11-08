// ==============================================================================
// File: .//Description-of-file/menu_state.md
// Overview: Pure C implementation of the radial menu gesture state machine.
// ==============================================================================

#include "menu_state.h"
#include "math_geometry.h"

void menu_state_init(MenuStateMachine* sm, float drag_threshold, float inner_radius) {
    if (!sm) return;
    sm->state = GESTURE_STATE_IDLE;
    sm->origin.x = 0;
    sm->origin.y = 0;
    sm->current.x = 0;
    sm->current.y = 0;
    sm->active_sector = RADIAL_SECTOR_NOTHING;
    sm->drag_threshold = drag_threshold;
    sm->inner_radius = inner_radius;
    sm->sector_count = RADIAL_DEFAULT_SECTOR_COUNT;
}

void menu_state_set_sector_count(MenuStateMachine* sm, int count) {
    if (!sm) return;
    if (count < 2) count = 2;
    if (count > RADIAL_MAX_SECTORS) count = RADIAL_MAX_SECTORS;
    sm->sector_count = count;
}

void menu_state_on_down(MenuStateMachine* sm, int32_t x, int32_t y) {
    if (!sm) return;
    sm->state = GESTURE_STATE_PENDING;
    sm->origin.x = x;
    sm->origin.y = y;
    sm->current.x = x;
    sm->current.y = y;
    sm->active_sector = RADIAL_SECTOR_NOTHING;
}

int menu_state_on_move(MenuStateMachine* sm, int32_t x, int32_t y) {
    if (!sm) return 0;
    if (sm->state == GESTURE_STATE_IDLE) return 0;

    sm->current.x = x;
    sm->current.y = y;

    double dist = math_distance(sm->origin.x, sm->origin.y, x, y);

    int just_activated = 0;
    if (sm->state == GESTURE_STATE_PENDING) {
        if (dist >= sm->drag_threshold) {
            sm->state = GESTURE_STATE_ACTIVE;
            just_activated = 1;
        }
    }

    if (sm->state == GESTURE_STATE_ACTIVE) {
        GeometryResult geom = math_resolve_sector_n(
            sm->origin.x, sm->origin.y,
            sm->current.x, sm->current.y,
            sm->inner_radius,
            sm->sector_count
        );
        sm->active_sector = geom.sector;
    }

    return just_activated;
}

int menu_state_on_up(MenuStateMachine* sm, RadialSector* confirmed_sector) {
    if (!sm) return 0;

    int was_active = (sm->state == GESTURE_STATE_ACTIVE);
    if (confirmed_sector) {
        *confirmed_sector = sm->active_sector;
    }

    menu_state_reset(sm);
    return was_active;
}

void menu_state_reset(MenuStateMachine* sm) {
    if (!sm) return;
    sm->state = GESTURE_STATE_IDLE;
    sm->active_sector = RADIAL_SECTOR_NOTHING;
}
