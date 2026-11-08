// ==============================================================================
// File: .//Description-of-file/main_cpp.md
// Overview: Entry point for C++ CLI orchestrating window discovery, repositioning, and hotkey daemon.
// ==============================================================================

#include "window_finder.hpp" // Include window finder module
#include "hotkey_listener.hpp" // Include hotkey daemon listener module
#include <string> // Include C++ string class
#include <vector> // Include vector container
#include <iostream> // Include stream I/O for CLI status reporting

using namespace movewindow; // Import movewindow namespace symbols

struct CliConfig { // Define CLI options configuration structure
    WindowTargetSpec target; // Window selection specification
    AlignmentMode align_mode{ALIGN_CENTER}; // Alignment strategy with center default
    int offset_x{DEFAULT_OFFSET_X}; // Horizontal pixel offset
    int offset_y{DEFAULT_OFFSET_Y}; // Vertical pixel offset
    int clamp_to_screen{DEFAULT_CLAMP_SCREEN}; // Flag controlling screen clamping
    bool daemon_mode{false}; // Flag indicating whether to run as background hotkey daemon
}; // End struct CliConfig

static CliConfig parse_arguments(int argc, char* argv[]) { // Define argument parser function
    CliConfig config; // Instantiate config struct with defaults
    for (int i = 1; i < argc; ++i) { // Loop through provided command line arguments
        std::string arg = argv[i]; // Convert C string argument to std::string
        if (arg == "--title" && i + 1 < argc) { // Parse window title substring option
            config.target.type = SelectorType::TitleSubstring; // Set selector strategy
            config.target.query = argv[++i]; // Store title argument value
        } else if (arg == "--process" && i + 1 < argc) { // Parse process name option
            config.target.type = SelectorType::ProcessName; // Set selector strategy
            config.target.query = argv[++i]; // Store process name argument value
        } else if (arg == "--hwnd" && i + 1 < argc) { // Parse raw handle option
            config.target.type = SelectorType::RawHandle; // Set selector strategy
            config.target.query = argv[++i]; // Store handle argument value
        } else if (arg == "--active") { // Parse active foreground window flag
            config.target.type = SelectorType::ActiveForeground; // Set selector strategy
        } else if (arg == "--align" && i + 1 < argc) { // Parse alignment mode option
            std::string val = argv[++i]; // Read alignment value
            if (val == "topleft") { // Check for top-left mode
                config.align_mode = ALIGN_TOP_LEFT; // Assign top-left enum
            } else if (val == "custom") { // Check for custom offset mode
                config.align_mode = ALIGN_CUSTOM_OFFSET; // Assign custom offset enum
            } else { // Fallback to center mode
                config.align_mode = ALIGN_CENTER; // Assign center enum
            } // End alignment check
        } else if (arg == "--offset-x" && i + 1 < argc) { // Parse X offset option
            config.offset_x = std::stoi(argv[++i]); // Convert and assign X offset
        } else if (arg == "--offset-y" && i + 1 < argc) { // Parse Y offset option
            config.offset_y = std::stoi(argv[++i]); // Convert and assign Y offset
        } else if (arg == "--no-clamp") { // Parse disable clamping flag
            config.clamp_to_screen = 0; // Disable screen clamping
        } else if (arg == "--daemon") { // Parse daemon mode flag
            config.daemon_mode = true; // Enable daemon mode
        } // End argument matching
    } // End argument loop
    return config; // Return populated configuration structure
} // End parse_arguments

static int execute_move(const CliConfig& config) { // Define move action execution function
    auto target_hwnd = WindowFinder::resolve(config.target); // Resolve window handle using finder
    if (!target_hwnd.has_value()) { // Check if window resolution failed
        return MOVE_WINDOW_ERR_NOT_FOUND; // Return not found error code
    } // End handle existence check

    return move_window_to_cursor( // Execute move operation via C ABI
        *target_hwnd, // Resolved target HWND
        config.align_mode, // Alignment mode
        config.offset_x, // X offset
        config.offset_y, // Y offset
        config.clamp_to_screen // Clamping setting
    ); // End move_window_to_cursor call
} // End execute_move

int main(int argc, char* argv[]) { // Application entry point function
    CliConfig config = parse_arguments(argc, argv); // Parse command line arguments into config

    if (config.daemon_mode) { // Check if daemon mode requested
        HotkeyListener listener( // Construct hotkey listener instance
            DEFAULT_HOTKEY_ID, // Use default hotkey identifier
            DEFAULT_HOTKEY_MODIFIERS, // Use Ctrl+Alt modifiers
            DEFAULT_HOTKEY_VK, // Use 'M' keycode
            [&config]() { execute_move(config); } // Lambda invoking execute_move on hotkey press
        ); // End constructor call
        listener.start_blocking(); // Run message loop synchronously
        return MOVE_WINDOW_SUCCESS; // Return success status code
    } // End daemon mode check

    return execute_move(config); // Execute one-shot move operation and return status code
} // End main function body
