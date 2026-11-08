// ==============================================================================
// File: .//Description-of-file/move_window_api_c.md
// Overview: Implementation of public C ABI exports for the move-window-api library.
// ==============================================================================

#define BUILDING_MOVE_WINDOW_DLL 1 // Enable DLL export attribute macro
#include "move_window_api.h" // Include exported API definitions
#include "geometry_calc.h" // Include pure domain coordinate calculation logic
#include "win32_window_adapter.h" // Include Win32 API adapter seam
#include <string.h> // Include standard string handling functions
#include <ctype.h> // Include character classification and conversion functions
#include <psapi.h> // Include process status API for process name resolution

typedef struct { // Define helper context struct for title search enumeration
    const char* search_term; // Substring to match
    HWND found_hwnd; // Handle of discovered window
} TitleSearchContext; // Context type for title search

typedef struct { // Define helper context struct for process search enumeration
    const char* process_name; // Executable name to match
    HWND found_hwnd; // Handle of discovered window
} ProcessSearchContext; // Context type for process search

static void ensure_input_desktop_attached(void) { // Helper to ensure thread is bound to user interactive desktop
    HDESK hDesk = OpenInputDesktop(0, FALSE, 0x01FF); // Attempt attaching to input desktop for background sessions
    if (hDesk) { // Check if desktop handle was obtained
        SetThreadDesktop(hDesk); // Attach current thread to input desktop
        CloseDesktop(hDesk); // Close temporary desktop handle
    } // End desktop check
} // End function body

static int case_insensitive_contains(const char* haystack, const char* needle) { // Define helper substring check
    if (!haystack || !needle) { // Check for null pointers
        return 0; // Return zero if either pointer is null
    } // End null check
    if (*needle == '\0') { // Check for empty needle string
        return 1; // Empty string always matches
    } // End empty check

    size_t h_len = strlen(haystack); // Measure haystack string length
    size_t n_len = strlen(needle); // Measure needle string length
    if (n_len > h_len) { // Check if needle exceeds haystack size
        return 0; // Cannot contain larger substring
    } // End length comparison

    for (size_t i = 0; i <= h_len - n_len; ++i) { // Iterate through each candidate starting index
        size_t j = 0; // Initialize inner character match index
        while (j < n_len && tolower((unsigned char)haystack[i + j]) == tolower((unsigned char)needle[j])) { // Compare case-insensitively
            j++; // Advance match index
        } // End character loop
        if (j == n_len) { // Verify if entire needle was matched
            return 1; // Return match found
        } // End match confirmation
    } // End outer loop
    return 0; // Return no match found
} // End function body

static BOOL CALLBACK enum_windows_title_proc(HWND hwnd, LPARAM lparam) { // Define window enumeration callback for title
    TitleSearchContext* ctx = (TitleSearchContext*)lparam; // Cast LPARAM to context structure pointer
    if (!adapter_is_window_valid(hwnd)) { // Check if window is valid and visible
        return TRUE; // Continue enumeration to next window
    } // End validity check

    char title_buf[512]; // Allocate buffer for window title
    int length = GetWindowTextA(hwnd, title_buf, sizeof(title_buf)); // Query window title string
    if (length > 0 && case_insensitive_contains(title_buf, ctx->search_term)) { // Check title for matching substring
        ctx->found_hwnd = hwnd; // Record matched window handle
        return FALSE; // Stop enumeration
    } // End match check
    return TRUE; // Continue enumeration
} // End function body

static BOOL CALLBACK enum_windows_process_proc(HWND hwnd, LPARAM lparam) { // Define window enumeration callback for process
    ProcessSearchContext* ctx = (ProcessSearchContext*)lparam; // Cast LPARAM to context structure pointer
    if (!adapter_is_window_valid(hwnd)) { // Check if window is valid and visible
        return TRUE; // Continue enumeration
    } // End validity check

    DWORD pid = 0; // Allocate process ID variable
    GetWindowThreadProcessId(hwnd, &pid); // Query process ID associated with window handle
    if (pid == 0) { // Check if process ID retrieval failed
        return TRUE; // Continue enumeration
    } // End pid check

    HANDLE hProc = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid); // Open process handle with query permissions
    if (hProc) { // Check if process opened successfully
        char image_path[MAX_PATH]; // Allocate buffer for executable image path
        DWORD size = MAX_PATH; // Set buffer capacity
        if (QueryFullProcessImageNameA(hProc, 0, image_path, &size)) { // Fetch full executable path
            const char* base_name = strrchr(image_path, '\\'); // Locate last path separator
            base_name = base_name ? (base_name + 1) : image_path; // Extract filename component
            if (case_insensitive_contains(base_name, ctx->process_name)) { // Match process filename
                ctx->found_hwnd = hwnd; // Record matched window handle
                CloseHandle(hProc); // Close opened process handle
                return FALSE; // Stop enumeration
            } // End process match check
        } // End query check
        CloseHandle(hProc); // Close opened process handle
    } // End handle check
    return TRUE; // Continue enumeration
} // End function body

int move_window_to_cursor(HWND hwnd, AlignmentMode mode, int offset_x, int offset_y, int clamp_to_screen) { // Define move to cursor function
    if (!adapter_is_window_valid(hwnd)) { // Check if target window exists and is visible
        return MOVE_WINDOW_ERR_NOT_FOUND; // Return window not found error code
    } // End validity check

    ensure_input_desktop_attached(); // Ensure thread has access to active desktop
    if (IsIconic(hwnd)) { // Check if window is currently minimized
        ShowWindow(hwnd, SW_RESTORE); // Restore minimized window before repositioning
        Sleep(50); // Yield briefly to allow DWM window frame restoration
    } // End iconic check

    Point2D mouse_pos; // Allocate mouse position point
    if (adapter_get_cursor_position(&mouse_pos) != MOVE_WINDOW_SUCCESS) { // Query current cursor coordinates
        return MOVE_WINDOW_ERR_WIN32_CALL; // Return Win32 error code
    } // End cursor check

    Rect2D win_geom; // Allocate window geometry structure
    if (adapter_get_window_geometry(hwnd, &win_geom) != MOVE_WINDOW_SUCCESS) { // Query window dimensions
        return MOVE_WINDOW_ERR_WIN32_CALL; // Return Win32 error code
    } // End geometry check

    Point2D target_pos = calculate_window_target_position(mouse_pos, win_geom.width, win_geom.height, mode, offset_x, offset_y); // Calculate raw target coordinates

    if (clamp_to_screen) { // Check if screen boundary clamping is requested
        Rect2D mon_bounds; // Allocate monitor boundary rectangle
        if (adapter_get_monitor_geometry(mouse_pos, &mon_bounds) == MOVE_WINDOW_SUCCESS) { // Query monitor work area
            target_pos = clamp_position_to_bounds(target_pos, win_geom.width, win_geom.height, mon_bounds); // Clamp target coordinates inside monitor
        } // End monitor query check
    } // End clamp check

    return adapter_set_window_position(hwnd, target_pos); // Apply updated position to window
} // End function body

int move_window_by_title(const char* title_substring, AlignmentMode mode, int offset_x, int offset_y, int clamp_to_screen) { // Define move by title function
    HWND hwnd = find_window_by_title_substring(title_substring); // Locate window handle by title substring
    if (!hwnd) { // Verify window handle was found
        return MOVE_WINDOW_ERR_NOT_FOUND; // Return window not found error code
    } // End handle check
    return move_window_to_cursor(hwnd, mode, offset_x, offset_y, clamp_to_screen); // Delegate to core positioning function
} // End function body

int move_window_by_process_name(const char* process_name, AlignmentMode mode, int offset_x, int offset_y, int clamp_to_screen) { // Define move by process function
    HWND hwnd = find_window_by_process_name(process_name); // Locate window handle by process name
    if (!hwnd) { // Verify window handle was found
        return MOVE_WINDOW_ERR_NOT_FOUND; // Return window not found error code
    } // End handle check
    return move_window_to_cursor(hwnd, mode, offset_x, offset_y, clamp_to_screen); // Delegate to core positioning function
} // End function body

int move_active_window_to_cursor(AlignmentMode mode, int offset_x, int offset_y, int clamp_to_screen) { // Define move active window function
    HWND hwnd = get_foreground_window_handle(); // Fetch currently focused window handle
    if (!hwnd) { // Verify active window exists
        return MOVE_WINDOW_ERR_NOT_FOUND; // Return window not found error code
    } // End handle check
    return move_window_to_cursor(hwnd, mode, offset_x, offset_y, clamp_to_screen); // Delegate to core positioning function
} // End function body

HWND find_window_by_title_substring(const char* title_substring) { // Define find by title function
    if (!title_substring) { // Check for null search string
        return NULL; // Return null pointer on invalid parameter
    } // End argument check
    ensure_input_desktop_attached(); // Ensure thread has access to active desktop
    TitleSearchContext ctx = { title_substring, NULL }; // Initialize search context structure
    EnumWindows(enum_windows_title_proc, (LPARAM)&ctx); // Enumerate top-level windows
    return ctx.found_hwnd; // Return discovered handle or null
} // End function body

HWND find_window_by_process_name(const char* process_name) { // Define find by process function
    if (!process_name) { // Check for null search string
        return NULL; // Return null pointer on invalid parameter
    } // End argument check
    ensure_input_desktop_attached(); // Ensure thread has access to active desktop
    ProcessSearchContext ctx = { process_name, NULL }; // Initialize search context structure
    EnumWindows(enum_windows_process_proc, (LPARAM)&ctx); // Enumerate top-level windows
    return ctx.found_hwnd; // Return discovered handle or null
} // End function body

HWND get_foreground_window_handle(void) { // Define foreground window getter
    ensure_input_desktop_attached(); // Ensure thread has access to active desktop
    return GetForegroundWindow(); // Delegate to Win32 GetForegroundWindow API
} // End function body

int get_cursor_coordinates(int* out_x, int* out_y) { // Define cursor coordinates getter
    if (!out_x || !out_y) { // Validate output pointers
        return MOVE_WINDOW_ERR_INVALID_ARG; // Return invalid argument error code
    } // End argument check
    Point2D pt; // Allocate domain point structure
    int status = adapter_get_cursor_position(&pt); // Query cursor from adapter
    if (status == MOVE_WINDOW_SUCCESS) { // Check for query success
        *out_x = pt.x; // Assign output X
        *out_y = pt.y; // Assign output Y
    } // End status check
    return status; // Return status code
} // End function body

int get_window_bounds(HWND hwnd, int* out_x, int* out_y, int* out_w, int* out_h) { // Define window bounds getter
    if (!hwnd || !out_x || !out_y || !out_w || !out_h) { // Validate handle and output pointers
        return MOVE_WINDOW_ERR_INVALID_ARG; // Return invalid argument error code
    } // End argument check
    Rect2D geom; // Allocate geometry rectangle structure
    int status = adapter_get_window_geometry(hwnd, &geom); // Query window geometry from adapter
    if (status == MOVE_WINDOW_SUCCESS) { // Check for query success
        *out_x = geom.x; // Assign output X
        *out_y = geom.y; // Assign output Y
        *out_w = geom.width; // Assign output width
        *out_h = geom.height; // Assign output height
    } // End status check
    return status; // Return status code
} // End function body
