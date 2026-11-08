// ==============================================================================
// File: .//Description-of-file/main_cpp.md
// Overview: Standalone C++ entry point orchestrating radial menu execution.
// ==============================================================================

#include "include/radial_api.h"
#include <windows.h>
#include <stdio.h>

static void on_action_triggered(int32_t sector, void* user_data) {
    (void)user_data;
    switch (sector) {
        case RADIAL_SECTOR_FUNCTION1: // Top-Right: Copy (Ctrl+C)
            OutputDebugStringA("[Radial C++] Function 1 triggered: Copy (Ctrl+C)\n");
            break;
        case RADIAL_SECTOR_FUNCTION2: // Bottom-Right: Paste (Ctrl+V)
            OutputDebugStringA("[Radial C++] Function 2 triggered: Paste (Ctrl+V)\n");
            break;
        case RADIAL_SECTOR_FUNCTION3: // Bottom-Left: Undo (Ctrl+Z)
            OutputDebugStringA("[Radial C++] Function 3 triggered: Undo (Ctrl+Z)\n");
            break;
        case RADIAL_SECTOR_FUNCTION4: // Top-Left: Redo / Enter
            OutputDebugStringA("[Radial C++] Function 4 triggered: Redo / Enter\n");
            break;
        default:
            break;
    }
}

int main(void) {
    if (!radial_init()) {
        return 1;
    }

    radial_set_callback(on_action_triggered, NULL);

    if (!radial_start_hook()) {
        radial_shutdown();
        return 1;
    }

    // Run standard message pump
    radial_run_loop();

    radial_shutdown();
    return 0;
}
