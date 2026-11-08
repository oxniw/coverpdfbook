// ==============================================================================
// File: .//Description-of-file/window_finder_hpp.md
// Overview: C++ window query resolver supporting title, process, HWND, and active window selection.
// ==============================================================================

#ifndef MOVE_WINDOW_WINDOW_FINDER_HPP // Guard against multiple inclusions
#define MOVE_WINDOW_WINDOW_FINDER_HPP // Define macro for header guard

#include "move_window_api.h" // Include C API definitions
#include <string> // Include C++ string class
#include <optional> // Include std::optional for safe query results

namespace movewindow { // Open movewindow namespace

enum class SelectorType { // Define enumeration for window selection strategies
    ActiveForeground, // Select current active foreground window
    TitleSubstring, // Select window matching title substring
    ProcessName, // Select window matching executable process name
    RawHandle // Select window using direct HWND handle
}; // End enum class SelectorType

struct WindowTargetSpec { // Define target specification data structure
    SelectorType type{SelectorType::ActiveForeground}; // Selector type strategy with default
    std::string query; // Query string (title, process, or hex HWND)
    HWND raw_handle{nullptr}; // Explicit raw window handle if specified
}; // End struct WindowTargetSpec

class WindowFinder { // Define WindowFinder resolver class
public: // Public member declarations
    static std::optional<HWND> resolve(const WindowTargetSpec& spec); // Resolve target HWND from specification
    static std::string get_window_title(HWND hwnd); // Retrieve title text for a given window handle
}; // End class WindowFinder

} // namespace movewindow

#endif // Close header guard MOVE_WINDOW_WINDOW_FINDER_HPP
