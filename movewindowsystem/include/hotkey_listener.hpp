// ==============================================================================
// File: .//Description-of-file/hotkey_listener_hpp.md
// Overview: C++ background global hotkey daemon listener and message dispatcher.
// ==============================================================================

#ifndef MOVE_WINDOW_HOTKEY_LISTENER_HPP // Guard against multiple inclusions
#define MOVE_WINDOW_HOTKEY_LISTENER_HPP // Define macro for header guard

#include "config.h" // Include shared constants
#include <windows.h> // Include Windows Win32 API
#include <functional> // Include std::function for callback handler
#include <atomic> // Include std::atomic for thread-safe state flags
#include <thread> // Include std::thread for background execution

namespace movewindow { // Open movewindow namespace

class HotkeyListener { // Define HotkeyListener class
public: // Public interface
    using ActionCallback = std::function<void()>; // Define callback type signature

    HotkeyListener(int hotkey_id, UINT modifiers, UINT vk_key, ActionCallback callback); // Constructor
    ~HotkeyListener(); // Destructor

    bool start_async(); // Start listener in background thread
    void start_blocking(); // Run listener blocking on current thread
    void stop(); // Stop listener and unregister hotkey
    bool is_running() const; // Check if listener is active

private: // Private state and methods
    void run_loop(); // Internal message pump loop

    int m_hotkey_id; // Unique hotkey identifier
    UINT m_modifiers; // Hotkey modifier flags
    UINT m_vk_key; // Virtual key code
    ActionCallback m_callback; // Action callback invoked upon hotkey trigger
    std::atomic<bool> m_running{false}; // Atomic running indicator
    std::atomic<DWORD> m_thread_id{0}; // ID of thread executing message loop
    std::thread m_worker_thread; // Worker thread instance for async mode
}; // End class HotkeyListener

} // namespace movewindow

#endif // Close header guard MOVE_WINDOW_HOTKEY_LISTENER_HPP
