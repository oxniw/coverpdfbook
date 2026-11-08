// ==============================================================================
// File: .//Description-of-file/radial_controller.md
// Overview: Implementation of coordinator binding state, overlay window, and hooks.
// ==============================================================================

#include "radial_controller.h"
#include "mouse_hook.h"
#include "render_overlay.h"
#include "../include/config.h"

static RadialController g_controller = {};

static VOID CALLBACK AnimationTimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime) {
    (void)hwnd;
    (void)uMsg;
    (void)idEvent;
    (void)dwTime;
    radial_controller_tick();
}

RadialController* radial_controller_get_instance(void) {
    return &g_controller;
}

void radial_controller_set_sector_count(int count) {
    if (count < 2) count = 2;
    if (count > RADIAL_MAX_SECTORS) count = RADIAL_MAX_SECTORS;
    menu_state_set_sector_count(&g_controller.state_machine, count);
    anim_set_sector_count(&g_controller.anim, count);
    render_set_sector_count(count);
}

int radial_controller_get_sector_count(void) {
    return g_controller.state_machine.sector_count;
}

void radial_controller_set_sector_label(int sector_idx, const wchar_t* label) {
    render_set_sector_label(sector_idx, label);
}

void radial_controller_tick(void) {
    uint64_t now = GetTickCount64();
    int changed = anim_tick(&g_controller.anim, now);

    if (anim_is_selection_complete(&g_controller.anim)) {
        overlay_window_hide(&g_controller.overlay);
        if (g_controller.timer_id != 0) {
            KillTimer(NULL, g_controller.timer_id);
            g_controller.timer_id = 0;
        }
        return;
    }

    if (changed && g_controller.overlay.is_visible) {
        overlay_window_update_animated(
            &g_controller.overlay,
            g_controller.anim.scale,
            g_controller.anim.alpha,
            g_controller.anim.hover_weights
        );
    }
}

int radial_controller_init(void) {
    if (g_controller.is_initialized) {
        return 1;
    }

    // 1. Initialize GDI+
    if (!render_init_gdiplus()) {
        return 0;
    }

    // 2. Initialize pure state machine
    menu_state_init(&g_controller.state_machine, RADIAL_DRAG_THRESHOLD_PX, RADIAL_INNER_RADIUS_PX);

    // 3. Initialize animation manager
    anim_init(&g_controller.anim);

    // 4. Create transparent layered window
    if (!overlay_window_create(&g_controller.overlay, RADIAL_WINDOW_SIZE_PX, RADIAL_WINDOW_SIZE_PX)) {
        render_shutdown_gdiplus();
        return 0;
    }

    g_controller.action_callback = NULL;
    g_controller.callback_user_data = NULL;
    g_controller.timer_id = 0;
    g_controller.is_initialized = 1;

    return 1;
}

int radial_controller_start(void) {
    if (!g_controller.is_initialized) {
        if (!radial_controller_init()) {
            return 0;
        }
    }

    MouseHookCallbacks cbs;
    cbs.on_down = radial_controller_on_mouse_down;
    cbs.on_move = radial_controller_on_mouse_move;
    cbs.on_up   = radial_controller_on_mouse_up;

    return mouse_hook_install(cbs);
}

void radial_controller_stop(void) {
    if (g_controller.timer_id != 0) {
        KillTimer(NULL, g_controller.timer_id);
        g_controller.timer_id = 0;
    }
    mouse_hook_uninstall();
    overlay_window_hide(&g_controller.overlay);
    menu_state_reset(&g_controller.state_machine);
}

void radial_controller_set_callback(RadialActionCallback cb, void* user_data) {
    g_controller.action_callback = cb;
    g_controller.callback_user_data = user_data;
}

void radial_controller_on_mouse_down(int32_t x, int32_t y) {
    menu_state_on_down(&g_controller.state_machine, x, y);
}

void radial_controller_on_mouse_move(int32_t x, int32_t y) {
    int activated = menu_state_on_move(&g_controller.state_machine, x, y);

    if (activated) {
        uint64_t now = GetTickCount64();
        anim_start_spawn(&g_controller.anim, now);
        anim_set_target_sector(&g_controller.anim, g_controller.state_machine.active_sector);

        // Show overlay centered at the initial click point
        overlay_window_show_at(
            &g_controller.overlay,
            g_controller.state_machine.origin.x,
            g_controller.state_machine.origin.y
        );

        radial_controller_tick();

        if (g_controller.timer_id == 0) {
            g_controller.timer_id = SetTimer(NULL, 0, 15, AnimationTimerProc);
        }
    }

    if (g_controller.state_machine.state == GESTURE_STATE_ACTIVE) {
        anim_set_target_sector(&g_controller.anim, g_controller.state_machine.active_sector);
        radial_controller_tick();
    }
}

int radial_controller_on_mouse_up(int32_t x, int32_t y) {
    // Update position before release
    menu_state_on_move(&g_controller.state_machine, x, y);

    RadialSector confirmedSector = RADIAL_SECTOR_NOTHING;
    int wasGesture = menu_state_on_up(&g_controller.state_machine, &confirmedSector);

    if (wasGesture) {
        // Trigger action immediately so target window responds without delay
        if (confirmedSector != RADIAL_SECTOR_NOTHING && g_controller.action_callback) {
            g_controller.action_callback((int32_t)confirmedSector, g_controller.callback_user_data);
        }

        // Trigger visual selection confirmation animation (pop & fade-out)
        uint64_t now = GetTickCount64();
        anim_start_selection(&g_controller.anim, confirmedSector, now);
        radial_controller_tick();

        // Ensure timer is ticking through completion
        if (g_controller.timer_id == 0) {
            g_controller.timer_id = SetTimer(NULL, 0, 15, AnimationTimerProc);
        }

        return 1;
    }

    return 0; // Not a drag gesture, replayed as normal right click
}

void radial_controller_shutdown(void) {
    if (!g_controller.is_initialized) return;

    radial_controller_stop();
    overlay_window_destroy(&g_controller.overlay);
    render_shutdown_gdiplus();

    g_controller.is_initialized = 0;
}
