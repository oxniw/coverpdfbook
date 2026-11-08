// ==============================================================================
// File: .//Description-of-file/config_h.md
// Overview: Shared configuration constants, error codes, and alignment enumeration for move-window system.
// ==============================================================================

#ifndef MOVE_WINDOW_CONFIG_H // Guard against multiple inclusions
#define MOVE_WINDOW_CONFIG_H // Define macro for header guard

#ifdef __cplusplus // Check if compiling under C++ compiler
extern "C" { // Enable C linkage for C++ callers
#endif // End of C++ language check

#define MOVE_WINDOW_SUCCESS 0 // Return code indicating successful operation
#define MOVE_WINDOW_ERR_INVALID_ARG -1 // Return code indicating invalid parameter passed
#define MOVE_WINDOW_ERR_NOT_FOUND -2 // Return code indicating target window was not located
#define MOVE_WINDOW_ERR_WIN32_CALL -3 // Return code indicating Win32 system call failure
#define MOVE_WINDOW_ERR_HOTKEY_FAIL -4 // Return code indicating global hotkey registration failure

#define DEFAULT_OFFSET_X 0 // Default horizontal coordinate offset in pixels
#define DEFAULT_OFFSET_Y 0 // Default vertical coordinate offset in pixels
#define DEFAULT_CLAMP_SCREEN 1 // Default flag enabling screen edge clamping (1 = enabled, 0 = disabled)
#define DEFAULT_HOTKEY_ID 9001 // Default identifier used when registering global system hotkey
#define DEFAULT_HOTKEY_MODIFIERS 0x0002 | 0x0001 // Default modifier flags: MOD_CONTROL (0x2) | MOD_ALT (0x1)
#define DEFAULT_HOTKEY_VK 0x4D // Default virtual key code for hotkey: 'M' key (0x4D)

typedef enum { // Define enum for window alignment relative to cursor position
    ALIGN_CENTER = 0, // Align center of target window to cursor coordinates
    ALIGN_TOP_LEFT = 1, // Align top-left corner of target window to cursor coordinates
    ALIGN_CUSTOM_OFFSET = 2 // Place top-left at cursor plus custom X and Y offsets
} AlignmentMode; // Type definition for alignment mode

typedef struct { // Define structure representing rectangular coordinates
    int x; // Left horizontal boundary in pixels
    int y; // Top vertical boundary in pixels
    int width; // Total width in pixels
    int height; // Total height in pixels
} Rect2D; // Type definition for 2D bounding rectangle

typedef struct { // Define structure representing 2D screen coordinate point
    int x; // Horizontal coordinate
    int y; // Vertical coordinate
} Point2D; // Type definition for 2D coordinate point

#ifdef __cplusplus // Check if compiling under C++ compiler
} // Close C linkage block
#endif // End of C++ language check

#endif // Close header guard MOVE_WINDOW_CONFIG_H
