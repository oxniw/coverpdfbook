// ==============================================================================
// File: .//Description-of-file/radial_menu_integration.md
// Overview: Implementation of the 5-sector right-click gesture radial menu bridge,
//           folder discovery, and action routing to the PDF bookshelf overlay.
// ==============================================================================

#include "radial_menu_integration.h" // Include radial menu integration header
#include "config.h" // Include application configuration and IDs
#include "redialmenu/include/radial_api.h" // Include radial menu DLL API
#include <algorithm> // Include standard algorithms

namespace { // Begin anonymous namespace for local callback handling
    void onRadialSectorAction(int32_t sector, void* user_data) { // Callback handler for radial menu sector actions
        HWND hwnd = (HWND)user_data; // Cast user data to overlay window handle
        if (!hwnd || !IsWindow(hwnd)) return; // Validate window handle

        switch (sector) { // Dispatch action based on selected sector index
            case 1: // Close Program action (Top-Right / 12 to 2 o'clock)
                PostMessage(hwnd, WM_COMMAND, MAKEWPARAM(IDC_BTN_CLOSE_APP, 0), 0); // Post Close App command message
                break; // Break from switch
            case 2: // Next Page action (Mid-Right / 2 to 4 o'clock)
                PostMessage(hwnd, WM_COMMAND, MAKEWPARAM(IDC_BTN_NEXT_PAGE, 0), 0); // Post Next Page command message
                break; // Break from switch
            case 3: // Settings Panel action (Bottom-Right / 4 to 6 o'clock)
                PostMessage(hwnd, WM_COMMAND, MAKEWPARAM(IDC_CMD_OPEN_SETTINGS_AT_CURSOR, 0), 0); // Post Open Settings At Cursor command message
                break; // Break from switch
            case 4: // Toggle Click-Through action (Bottom-Left / 6 to 8 o'clock)
                PostMessage(hwnd, WM_COMMAND, MAKEWPARAM(IDC_BTN_TOGGLE_CLICKTHROUGH, 0), 0); // Post Toggle Click-Through command message
                break; // Break from switch
            case 5: // Prev Page action (Mid-Left / 8 to 10 o'clock)
                PostMessage(hwnd, WM_COMMAND, MAKEWPARAM(IDC_BTN_PREV_PAGE, 0), 0); // Post Prev Page command message
                break; // Break from switch
            case 6: // Toggle Covers visibility action (Top-Left / 10 to 12 o'clock)
                PostMessage(hwnd, WM_COMMAND, MAKEWPARAM(IDC_BTN_TOGGLE_COVERS, 0), 0); // Post Toggle Covers command message
                break; // Break from switch
            default: // Unrecognized sector or center cancel
                break; // Do nothing
        } // Close switch statement
    } // Close onRadialSectorAction callback
} // Close anonymous namespace

bool initRadialMenuIntegration(HWND hwndOverlay) { // Define initRadialMenuIntegration implementation
    if (!radial_init()) { // Initialize radial menu DLL subsystem
        return false; // Return false if initialization fails
    } // Close if statement

    // Configure 6-sector wheel with Navigation, Controls, Settings, and Click-Through
    radial_set_sector_count(6); // Set radial menu sector count to 6

    // Assign clear action labels
    radial_set_sector_label(1, L"Close Program"); // Sector 1 label: Close Program
    radial_set_sector_label(2, L"Next Page >>"); // Sector 2 label: Next Page
    radial_set_sector_label(3, L"Settings"); // Sector 3 label: Settings
    radial_set_sector_label(4, L"Click-Through"); // Sector 4 label: Click-Through
    radial_set_sector_label(5, L"<< Prev Page"); // Sector 5 label: Prev Page
    radial_set_sector_label(6, L"Hide/Show"); // Sector 6 label: Hide/Show Covers

    // Register callback connected to main overlay window
    radial_set_callback(onRadialSectorAction, (void*)hwndOverlay); // Register action callback function

    // Install right-click drag hook
    return (radial_start_hook() != 0); // Start low-level mouse hook and return status
} // Close initRadialMenuIntegration function

void shutdownRadialMenuIntegration(void) { // Define shutdownRadialMenuIntegration implementation
    radial_stop_hook(); // Stop and unhook low-level mouse hook
    radial_shutdown(); // Shutdown radial menu subsystem and free resources
} // Close shutdownRadialMenuIntegration function

std::vector<std::wstring> discoverPdfFolders(void) { // Define discoverPdfFolders implementation
    std::vector<std::wstring> folders; // Vector to hold discovered folder names
    WIN32_FIND_DATAW fd; // Allocate find data structure for directory iteration
    HANDLE h = FindFirstFileW(L"*", &fd); // Start search for directories in app directory

    if (h != INVALID_HANDLE_VALUE) { // Verify directory search handle is valid
        do { // Loop through all entries in working directory
            if ((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && // Check if entry is directory
                wcscmp(fd.cFileName, L".") != 0 && // Skip current directory dot
                wcscmp(fd.cFileName, L"..") != 0 && // Skip parent directory dot
                wcscmp(fd.cFileName, L".cache") != 0 && // Skip cache directory
                wcscmp(fd.cFileName, L".git") != 0 && // Skip git directory
                wcscmp(fd.cFileName, L"Description-of-file") != 0 && // Skip documentation directory
                wcscmp(fd.cFileName, L"redialmenu") != 0 && // Skip radial menu submodule directory
                wcscmp(fd.cFileName, L"movewindowsystem") != 0) { // Skip movewindowsystem directory
                folders.push_back(fd.cFileName); // Add discovered directory to list
            } // Close if statement
        } while (FindNextFileW(h, &fd)); // Continue search until all entries examined
        FindClose(h); // Close search handle
    } // Close if statement

    // Sort folders alphabetically
    std::sort(folders.begin(), folders.end()); // Sort folder names alphabetically

    // Ensure 'pdf' is first if it exists
    auto it = std::find(folders.begin(), folders.end(), L"pdf"); // Find 'pdf' folder in list
    if (it != folders.end() && it != folders.begin()) { // Check if 'pdf' found and not at beginning
        std::rotate(folders.begin(), it, it + 1); // Move 'pdf' to the beginning of list
    } // Close if statement

    if (folders.empty()) { // Check if no subfolders found
        folders.push_back(L"pdf"); // Default to 'pdf' folder
    } // Close if statement

    return folders; // Return list of available folders
} // Close discoverPdfFolders function
