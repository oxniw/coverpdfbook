// ==============================================================================
// File: .//Description-of-file/hotkey_listener_cpp.md
// Overview: Implementation of C++ global hotkey listener with Win32 message pump.
// ==============================================================================

#include "hotkey_listener.hpp" // Include HotkeyListener header

namespace movewindow { // Open movewindow namespace

HotkeyListener::HotkeyListener(int hotkey_id, UINT modifiers, UINT vk_key, ActionCallback callback) // Define constructor
    : m_hotkey_id(hotkey_id), // Initialize hotkey ID member
      m_modifiers(modifiers), // Initialize modifier flags member
      m_vk_key(vk_key), // Initialize virtual key code member
      m_callback(callback), // Initialize callback member
      m_running(false), // Set running flag to false
      m_thread_id(0) { // Set thread ID to zero
} // End constructor body

HotkeyListener::~HotkeyListener() { // Define destructor
    stop(); // Ensure listener is stopped and thread joined
} // End destructor body

bool HotkeyListener::start_async() { // Define asynchronous start method
    if (m_running.load()) { // Check if already running
        return false; // Return false if already active
    } // End running check

    m_running.store(true); // Mark state as active
    m_worker_thread = std::thread(&HotkeyListener::run_loop, this); // Spawn worker thread executing message loop
    return true; // Return success status
} // End start_async body

void HotkeyListener::start_blocking() { // Define blocking start method
    if (m_running.load()) { // Check if already running
        return; // Return immediately if active
    } // End running check

    m_running.store(true); // Mark state as active
    run_loop(); // Execute message pump directly on current thread
} // End start_blocking body

void HotkeyListener::stop() { // Define stop method
    if (!m_running.load()) { // Check if already stopped
        return; // Return if not running
    } // End stopped check

    m_running.store(false); // Mark running flag as false
    DWORD target_tid = m_thread_id.load(); // Fetch message loop thread ID
    if (target_tid != 0) { // Check if valid thread ID exists
        PostThreadMessage(target_tid, WM_QUIT, 0, 0); // Post WM_QUIT to terminate GetMessage pump
    } // End thread ID check

    if (m_worker_thread.joinable()) { // Check if worker thread can be joined
        m_worker_thread.join(); // Wait for worker thread termination
    } // End joinable check
} // End stop body

bool HotkeyListener::is_running() const { // Define running query method
    return m_running.load(); // Return atomic running state
} // End is_running body

void HotkeyListener::run_loop() { // Define internal message pump
    m_thread_id.store(GetCurrentThreadId()); // Store current thread ID for message posting

    BOOL reg_ok = RegisterHotKey(NULL, m_hotkey_id, m_modifiers, m_vk_key); // Register hotkey with OS
    if (!reg_ok) { // Check registration outcome
        m_running.store(false); // Clear running flag on failure
        return; // Terminate execution loop
    } // End registration check

    MSG msg; // Allocate Win32 MSG structure
    while (m_running.load()) { // Loop while running state is active
        BOOL res = GetMessage(&msg, NULL, 0, 0); // Wait and pull message from queue
        if (res <= 0) { // Check for WM_QUIT (0) or error (-1)
            break; // Exit message pump loop
        } // End return code check

        if (msg.message == WM_HOTKEY && (int)msg.wParam == m_hotkey_id) { // Check if message matches registered hotkey ID
            if (m_callback) { // Verify callback handler is valid
                m_callback(); // Trigger registered callback action
            } // End callback check
        } // End hotkey message check

        TranslateMessage(&msg); // Translate virtual-key messages
        DispatchMessage(&msg); // Dispatch message to window procedure if applicable
    } // End message pump loop

    UnregisterHotKey(NULL, m_hotkey_id); // Unregister hotkey with Windows subsystem
    m_running.store(false); // Reset running flag
    m_thread_id.store(0); // Reset thread ID
} // End run_loop body

} // namespace movewindow
