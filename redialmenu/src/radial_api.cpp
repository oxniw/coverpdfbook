// ==============================================================================
// File: .//Description-of-file/radial_api_cpp.md
// Overview: Exported C-ABI entry points for radial_menu.dll consumed by Python ctypes.
// ==============================================================================

#ifndef BUILDING_RADIAL_DLL
#define BUILDING_RADIAL_DLL
#endif
#include "../include/radial_api.h"
#include "radial_controller.h"
#include <windows.h>

static volatile int g_loopRunning = 0;

int radial_init(void) {
    return radial_controller_init();
}

void radial_set_sector_count(int count) {
    radial_controller_set_sector_count(count);
}

int radial_get_sector_count(void) {
    return radial_controller_get_sector_count();
}

void radial_set_sector_label(int sector_idx, const wchar_t* label) {
    radial_controller_set_sector_label(sector_idx, label);
}

void radial_set_callback(RadialActionCallback callback, void* user_data) {
    radial_controller_set_callback(callback, user_data);
}

int radial_start_hook(void) {
    return radial_controller_start();
}

void radial_stop_hook(void) {
    radial_controller_stop();
}

int radial_poll_events(void) {
    MSG msg;
    if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
        if (msg.message == WM_QUIT) {
            return -1;
        }
        TranslateMessage(&msg);
        DispatchMessage(&msg);
        return 1;
    }
    return 0;
}

void radial_run_loop(void) {
    g_loopRunning = 1;
    MSG msg;
    while (g_loopRunning && GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

void radial_shutdown(void) {
    g_loopRunning = 0;
    PostQuitMessage(0);
    radial_controller_shutdown();
}

int radial_get_gesture_state(void) {
    RadialController* ctrl = radial_controller_get_instance();
    return (int)ctrl->state_machine.state;
}

void radial_show_at(int screen_x, int screen_y) {
    RadialController* ctrl = radial_controller_get_instance();
    overlay_window_show_at(&ctrl->overlay, screen_x, screen_y);
}

void radial_hide(void) {
    RadialController* ctrl = radial_controller_get_instance();
    overlay_window_hide(&ctrl->overlay);
}

void radial_set_highlight(int sector) {
    RadialController* ctrl = radial_controller_get_instance();
    overlay_window_update_sector(&ctrl->overlay, (RadialSector)sector);
}
