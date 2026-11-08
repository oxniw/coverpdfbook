// ==============================================================================
// File: .//Description-of-file/mouse_hook.md
// Overview: Low-level mouse hook implementation (WH_MOUSE_LL) with right-click pass-through.
// ==============================================================================

#include "mouse_hook.h"

static HHOOK g_mouseHook = NULL;
static MouseHookCallbacks g_hookCallbacks = {};
static volatile LONG g_isReplaying = 0;

void mouse_hook_replay_right_click(int32_t x, int32_t y) {
    (void)x;
    (void)y;
    InterlockedExchange(&g_isReplaying, 1);

    INPUT inputs[2] = {};

    // Right Down
    inputs[0].type = INPUT_MOUSE;
    inputs[0].mi.dwFlags = MOUSEEVENTF_RIGHTDOWN;

    // Right Up
    inputs[1].type = INPUT_MOUSE;
    inputs[1].mi.dwFlags = MOUSEEVENTF_RIGHTUP;

    SendInput(2, inputs, sizeof(INPUT));

    InterlockedExchange(&g_isReplaying, 0);
}

static LRESULT CALLBACK LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode >= 0) {
        MSLLHOOKSTRUCT* pMouse = (MSLLHOOKSTRUCT*)lParam;

        // Ignore synthetic/injected events to prevent recursion loops
        if ((pMouse->flags & LLMHF_INJECTED) || g_isReplaying) {
            return CallNextHookEx(g_mouseHook, nCode, wParam, lParam);
        }

        switch (wParam) {
            case WM_RBUTTONDOWN: {
                if (g_hookCallbacks.on_down) {
                    g_hookCallbacks.on_down(pMouse->pt.x, pMouse->pt.y);
                }
                // Suppress initial right button down to monitor for hold/drag
                return 1;
            }

            case WM_MOUSEMOVE: {
                if (g_hookCallbacks.on_move) {
                    g_hookCallbacks.on_move(pMouse->pt.x, pMouse->pt.y);
                }
                break;
            }

            case WM_RBUTTONUP: {
                if (g_hookCallbacks.on_up) {
                    int handledGesture = g_hookCallbacks.on_up(pMouse->pt.x, pMouse->pt.y);
                    if (!handledGesture) {
                        // It was a quick tap: replay standard right click to open context menu
                        mouse_hook_replay_right_click(pMouse->pt.x, pMouse->pt.y);
                    }
                    return 1; // Suppress original right-up
                }
                break;
            }
        }
    }

    return CallNextHookEx(g_mouseHook, nCode, wParam, lParam);
}

int mouse_hook_install(MouseHookCallbacks callbacks) {
    if (g_mouseHook != NULL) {
        return 1; // Already installed
    }

    g_hookCallbacks = callbacks;
    HINSTANCE hInst = GetModuleHandle(NULL);
    g_mouseHook = SetWindowsHookEx(WH_MOUSE_LL, LowLevelMouseProc, hInst, 0);

    return (g_mouseHook != NULL) ? 1 : 0;
}

void mouse_hook_uninstall(void) {
    if (g_mouseHook != NULL) {
        UnhookWindowsHookEx(g_mouseHook);
        g_mouseHook = NULL;
    }
    g_hookCallbacks.on_down = NULL;
    g_hookCallbacks.on_move = NULL;
    g_hookCallbacks.on_up = NULL;
}
