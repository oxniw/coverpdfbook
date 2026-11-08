// ==============================================================================
// File: .//Description-of-file/win32_window_adapter_c.md
// Overview: Win32 OS adapter implementation communicating directly with Windows user32 APIs.
// ==============================================================================

#include "win32_window_adapter.h" // Include adapter header definitions

int adapter_get_cursor_position(Point2D* out_pos) { // Define cursor retrieval function
    if (!out_pos) { // Validate pointer argument
        return MOVE_WINDOW_ERR_INVALID_ARG; // Return invalid argument error code
    } // End argument check

    POINT pt; // Allocate Win32 POINT structure
    if (!GetCursorPos(&pt)) { // Call Win32 GetCursorPos to fetch mouse coordinates
        HDESK hDesk = OpenInputDesktop(0, FALSE, 0x01FF); // Attempt attaching to input desktop for background sessions
        if (hDesk) { // Check if desktop handle was obtained
            SetThreadDesktop(hDesk); // Attach current thread to input desktop
            CloseDesktop(hDesk); // Close temporary desktop handle
        } // End desktop check
        if (!GetCursorPos(&pt)) { // Retry GetCursorPos after thread desktop attachment
            return MOVE_WINDOW_ERR_WIN32_CALL; // Return Win32 call failure if API still returns zero
        } // End retry check
    } // End API check

    out_pos->x = pt.x; // Assign horizontal cursor coordinate
    out_pos->y = pt.y; // Assign vertical cursor coordinate
    return MOVE_WINDOW_SUCCESS; // Return success status code
} // End function body

int adapter_get_window_geometry(HWND hwnd, Rect2D* out_geometry) { // Define window geometry query function
    if (!hwnd || !out_geometry) { // Validate window handle and output pointer
        return MOVE_WINDOW_ERR_INVALID_ARG; // Return invalid argument error code
    } // End argument check

    if (!IsWindow(hwnd)) { // Verify handle represents an existing Win32 window
        return MOVE_WINDOW_ERR_NOT_FOUND; // Return window not found error code
    } // End existence check

    RECT rc; // Allocate Win32 RECT structure
    if (!GetWindowRect(hwnd, &rc)) { // Fetch window screen rectangle
        return MOVE_WINDOW_ERR_WIN32_CALL; // Return failure if GetWindowRect fails
    } // End API check

    out_geometry->x = rc.left; // Map left coordinate to structure
    out_geometry->y = rc.top; // Map top coordinate to structure
    out_geometry->width = rc.right - rc.left; // Calculate total window width
    out_geometry->height = rc.bottom - rc.top; // Calculate total window height
    return MOVE_WINDOW_SUCCESS; // Return success status code
} // End function body

int adapter_get_monitor_geometry(Point2D pt, Rect2D* out_monitor_bounds) { // Define monitor query function
    if (!out_monitor_bounds) { // Validate output pointer
        return MOVE_WINDOW_ERR_INVALID_ARG; // Return invalid argument error code
    } // End argument check

    POINT win_pt = { pt.x, pt.y }; // Construct Win32 POINT from domain coordinates
    HMONITOR hMon = MonitorFromPoint(win_pt, MONITOR_DEFAULTTONEAREST); // Locate closest active monitor
    if (!hMon) { // Check if monitor lookup failed
        return MOVE_WINDOW_ERR_WIN32_CALL; // Return Win32 error code
    } // End monitor lookup check

    MONITORINFO mi; // Allocate Win32 MONITORINFO structure
    mi.cbSize = sizeof(MONITORINFO); // Set structure size requirement for API compatibility
    if (!GetMonitorInfoW(hMon, &mi)) { // Retrieve monitor information
        return MOVE_WINDOW_ERR_WIN32_CALL; // Return error if query failed
    } // End query check

    out_monitor_bounds->x = mi.rcWork.left; // Map usable work area left bound
    out_monitor_bounds->y = mi.rcWork.top; // Map usable work area top bound
    out_monitor_bounds->width = mi.rcWork.right - mi.rcWork.left; // Compute work area width
    out_monitor_bounds->height = mi.rcWork.bottom - mi.rcWork.top; // Compute work area height
    return MOVE_WINDOW_SUCCESS; // Return success status code
} // End function body

int adapter_set_window_position(HWND hwnd, Point2D pos) { // Define window positioning function
    if (!hwnd) { // Check for null window handle
        return MOVE_WINDOW_ERR_INVALID_ARG; // Return invalid argument error code
    } // End argument check

    UINT flags = SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE; // Configure flags to preserve Z-order and size
    BOOL status = SetWindowPos(hwnd, NULL, pos.x, pos.y, 0, 0, flags); // Update window coordinates via user32
    if (!status) { // Check if SetWindowPos reported failure
        return MOVE_WINDOW_ERR_WIN32_CALL; // Return Win32 error code
    } // End status check

    return MOVE_WINDOW_SUCCESS; // Return success status code
} // End function body

int adapter_is_window_valid(HWND hwnd) { // Define window validity check function
    if (!hwnd) { // Test for null handle
        return 0; // Return zero indicating invalid
    } // End null check
    if (!IsWindow(hwnd)) { // Test if window handle is active
        return 0; // Return zero if window does not exist
    } // End existence check
    return IsWindowVisible(hwnd) ? 1 : 0; // Return 1 if visible, else 0
} // End function body
