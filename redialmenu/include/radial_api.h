// ==============================================================================
// File: .//Description-of-file/radial_api_h.md
// Overview: C-ABI exported DLL API for radial menu engine consumed by Python ctypes.
// ==============================================================================

#ifndef RADIAL_API_H
#define RADIAL_API_H

#include "radial_types.h"

#if defined(_WIN32) || defined(__CYGWIN__)
    #ifdef BUILDING_RADIAL_DLL
        #define RADIAL_EXPORT __declspec(dllexport)
    #else
        #define RADIAL_EXPORT __declspec(dllimport)
    #endif
#else
    #define RADIAL_EXPORT
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize the radial menu overlay engine and register window classes.
 * @return 1 on success, 0 on failure.
 */
RADIAL_EXPORT int radial_init(void);

/**
 * @brief Configure number of outer sectors N (e.g. 2, 3, 4, 6, 8, up to 16).
 * @param count Number of sectors (clamped to 2..16).
 */
RADIAL_EXPORT void radial_set_sector_count(int count);

/**
 * @brief Get currently configured outer sector count N.
 */
RADIAL_EXPORT int radial_get_sector_count(void);

/**
 * @brief Set custom label for sector (1-indexed).
 * @param sector_index 1 to N.
 * @param label Wide string (e.g. L"Copy", L"Paste").
 */
RADIAL_EXPORT void radial_set_sector_label(int sector_index, const wchar_t* label);

/**
 * @brief Register callback function to be called when a sector action is confirmed.
 * @param callback Function pointer to receive sector ID.
 * @param user_data Opaque pointer passed back to the callback.
 */
RADIAL_EXPORT void radial_set_callback(RadialActionCallback callback, void* user_data);

/**
 * @brief Start the low-level mouse hook to capture right-click drag gestures.
 * @return 1 if hook installed successfully, 0 otherwise.
 */
RADIAL_EXPORT int radial_start_hook(void);

/**
 * @brief Stop and unhook the mouse hook.
 */
RADIAL_EXPORT void radial_stop_hook(void);

/**
 * @brief Run a single iteration of the Windows message pump (non-blocking).
 * @return 1 if message processed, 0 if queue empty, -1 on WM_QUIT.
 */
RADIAL_EXPORT int radial_poll_events(void);

/**
 * @brief Run blocking Windows message loop until shutdown is requested.
 */
RADIAL_EXPORT void radial_run_loop(void);

/**
 * @brief Request shutdown of the message loop and cleanup resources.
 */
RADIAL_EXPORT void radial_shutdown(void);

/**
 * @brief Check current state of gesture interaction.
 * @return Current GestureState (0=IDLE, 1=PENDING, 2=ACTIVE).
 */
RADIAL_EXPORT int radial_get_gesture_state(void);

/**
 * @brief Manually show radial menu at specified screen coordinates.
 * @param screen_x Center X position on screen.
 * @param screen_y Center Y position on screen.
 */
RADIAL_EXPORT void radial_show_at(int screen_x, int screen_y);

/**
 * @brief Manually hide radial menu overlay window.
 */
RADIAL_EXPORT void radial_hide(void);

/**
 * @brief Manually set the currently highlighted sector.
 * @param sector Sector index (0 to 4).
 */
RADIAL_EXPORT void radial_set_highlight(int sector);

#ifdef __cplusplus
}
#endif

#endif // RADIAL_API_H
