// ==============================================================================
// File: .//Description-of-file/window_finder_cpp.md
// Overview: Implementation of C++ multi-strategy window lookup resolver.
// ==============================================================================

#include "window_finder.hpp" // Include WindowFinder header
#include <sstream> // Include stringstream for hex parsing

namespace movewindow { // Open movewindow namespace

std::optional<HWND> WindowFinder::resolve(const WindowTargetSpec& spec) { // Define resolution function
    HWND result = nullptr; // Initialize result handle pointer

    switch (spec.type) { // Branch based on target selector type
        case SelectorType::ActiveForeground: // Active foreground window branch
            result = get_foreground_window_handle(); // Fetch current active window
            break; // Exit switch block

        case SelectorType::TitleSubstring: // Title substring branch
            if (!spec.query.empty()) { // Ensure query string is non-empty
                result = find_window_by_title_substring(spec.query.c_str()); // Search by title
            } // End emptiness check
            break; // Exit switch block

        case SelectorType::ProcessName: // Process name branch
            if (!spec.query.empty()) { // Ensure query string is non-empty
                result = find_window_by_process_name(spec.query.c_str()); // Search by process name
            } // End emptiness check
            break; // Exit switch block

        case SelectorType::RawHandle: // Raw window handle branch
            if (spec.raw_handle != nullptr) { // Check if handle object is non-null
                result = spec.raw_handle; // Use provided raw handle
            } else if (!spec.query.empty()) { // Check if handle was passed as string
                uintptr_t handle_val = 0; // Initialize integer for handle value
                std::stringstream ss; // Allocate stringstream
                if (spec.query.rfind("0x", 0) == 0 || spec.query.rfind("0X", 0) == 0) { // Check for hex prefix
                    ss << std::hex << spec.query; // Parse as hexadecimal
                } else { // Decimal string fallback
                    ss << std::dec << spec.query; // Parse as decimal
                } // End format check
                ss >> handle_val; // Extract numeric handle value
                result = reinterpret_cast<HWND>(handle_val); // Cast integer to HWND
            } // End string check
            break; // Exit switch block
    } // End switch statement

    if (result && IsWindow(result)) { // Validate that resolved handle is a valid window
        return result; // Return found window handle
    } // End validation check

    return std::nullopt; // Return empty optional if resolution failed
} // End function body

std::string WindowFinder::get_window_title(HWND hwnd) { // Define title query function
    if (!hwnd || !IsWindow(hwnd)) { // Check if handle is valid
        return std::string(); // Return empty string on invalid handle
    } // End handle check

    char buffer[512] = {0}; // Allocate buffer initialized to zero
    int length = GetWindowTextA(hwnd, buffer, sizeof(buffer)); // Query window title text
    if (length > 0) { // Check if title was retrieved
        return std::string(buffer, length); // Return populated string
    } // End length check
    return std::string(); // Return empty string if no title
} // End function body

} // namespace movewindow
