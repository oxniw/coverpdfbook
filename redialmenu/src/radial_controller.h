// ==============================================================================
// File: .//Description-of-file/radial_controller.md
// Overview: Header for controller orchestrating state, overlay window, and hooks.
// ==============================================================================

#ifndef RADIAL_CONTROLLER_H
#define RADIAL_CONTROLLER_H

#include "../include/radial_types.h"
#include "menu_state.h"
#include "overlay_window.h"
#include "animation_manager.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    MenuStateMachine state_machine;
    OverlayWindow overlay;
    AnimationManager anim;
    RadialActionCallback action_callback;
    void* callback_user_data;
    int is_initialized;
    UINT_PTR timer_id;
} RadialController;

/**
 * @brief Initialize the controller and its subsystems.
 * @return 1 on success, 0 on failure.
 */
int radial_controller_init(void);

/**
 * @brief Set the dynamic outer sector count N (2..16).
 */
void radial_controller_set_sector_count(int count);

/**
 * @brief Get currently configured outer sector count N.
 */
int radial_controller_get_sector_count(void);

/**
 * @brief Configure custom label for a sector.
 */
void radial_controller_set_sector_label(int sector_idx, const wchar_t* label);

/**
 * @brief Advance animation frame tick.
 */
void radial_controller_tick(void);

/**
 * @brief Start mouse hook.
 */
int radial_controller_start(void);

/**
 * @brief Stop mouse hook and hide overlay.
 */
void radial_controller_stop(void);

/**
 * @brief Set the callback for confirmed actions.
 */
void radial_controller_set_callback(RadialActionCallback cb, void* user_data);

/**
 * @brief Handle mouse down event.
 */
void radial_controller_on_mouse_down(int32_t x, int32_t y);

/**
 * @brief Handle mouse move event.
 */
void radial_controller_on_mouse_move(int32_t x, int32_t y);

/**
 * @brief Handle mouse up event.
 * @return 1 if gesture was executed, 0 if was normal tap.
 */
int radial_controller_on_mouse_up(int32_t x, int32_t y);

/**
 * @brief Teardown and destroy controller resources.
 */
void radial_controller_shutdown(void);

/**
 * @brief Get global controller instance.
 */
RadialController* radial_controller_get_instance(void);

#ifdef __cplusplus
}
#endif

#endif // RADIAL_CONTROLLER_H
