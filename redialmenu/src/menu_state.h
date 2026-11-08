// ==============================================================================
// File: .//Description-of-file/menu_state.md
// Overview: Header for pure state machine managing gesture lifecycle and tracking.
// ==============================================================================

#ifndef MENU_STATE_H
#define MENU_STATE_H

#include "../include/radial_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    GestureState state;
    RadialPoint origin;
    RadialPoint current;
    RadialSector active_sector;
    float drag_threshold;
    float inner_radius;
    int sector_count;
} MenuStateMachine;

/**
 * @brief Initialize state machine with configuration parameters.
 */
void menu_state_init(MenuStateMachine* sm, float drag_threshold, float inner_radius);

/**
 * @brief Set dynamic sector count N.
 */
void menu_state_set_sector_count(MenuStateMachine* sm, int count);

/**
 * @brief Handle mouse down event at (x, y). Transitions from IDLE to PENDING.
 */
void menu_state_on_down(MenuStateMachine* sm, int32_t x, int32_t y);

/**
 * @brief Handle mouse move event at (x, y). Checks threshold and updates active sector.
 * @return 1 if state transitioned to ACTIVE, 0 otherwise.
 */
int menu_state_on_move(MenuStateMachine* sm, int32_t x, int32_t y);

/**
 * @brief Handle mouse up event.
 * @param confirmed_sector Output pointer to receive the confirmed sector.
 * @return 1 if an action was confirmed (was ACTIVE), 0 if click was normal tap (was PENDING).
 */
int menu_state_on_up(MenuStateMachine* sm, RadialSector* confirmed_sector);

/**
 * @brief Reset state machine to IDLE.
 */
void menu_state_reset(MenuStateMachine* sm);

#ifdef __cplusplus
}
#endif

#endif // MENU_STATE_H
