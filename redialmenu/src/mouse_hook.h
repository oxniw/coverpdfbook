// ==============================================================================
// File: .//Description-of-file/mouse_hook.md
// Overview: Header for Windows low-level mouse hook (WH_MOUSE_LL) management.
// ==============================================================================

#ifndef MOUSE_HOOK_H
#define MOUSE_HOOK_H

#include <windows.h>
#include "../include/radial_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*HookMouseDownHandler)(int32_t x, int32_t y);
typedef void (*HookMouseMoveHandler)(int32_t x, int32_t y);
typedef int  (*HookMouseUpHandler)(int32_t x, int32_t y); // returns 1 if gesture handled, 0 if was tap

typedef struct {
    HookMouseDownHandler on_down;
    HookMouseMoveHandler on_move;
    HookMouseUpHandler   on_up;
} MouseHookCallbacks;

/**
 * @brief Initialize and install the global low-level mouse hook.
 * @param callbacks Event handlers invoked upon mouse events.
 * @return 1 on success, 0 on failure.
 */
int mouse_hook_install(MouseHookCallbacks callbacks);

/**
 * @brief Uninstall and remove the global mouse hook.
 */
void mouse_hook_uninstall(void);

/**
 * @brief Synthesize a normal right-click (down + up) when gesture is not triggered.
 * @param x Screen X coordinate.
 * @param y Screen Y coordinate.
 */
void mouse_hook_replay_right_click(int32_t x, int32_t y);

#ifdef __cplusplus
}
#endif

#endif // MOUSE_HOOK_H
