# ==============================================================================
# File: .//Description-of-file/hotkey_listener_py.md
# Overview: Python background hotkey listener daemon utilizing Win32 RegisterHotKey and ctypes message pump.
# ==============================================================================

import ctypes # Import ctypes for Win32 API calls
import ctypes.wintypes # Import standard Windows data types
import logging # Import logging for structured output
import threading # Import threading for background worker management
from typing import Callable # Import Callable type hint
from config import DEFAULT_HOTKEY_MODIFIERS, DEFAULT_HOTKEY_VK # Import default hotkey constants

logger = logging.getLogger(__name__) # Initialize module-level logger

user32 = ctypes.windll.user32 # Bind Windows user32 library

class HotkeyDaemon: # Define HotkeyDaemon listener class
    def __init__( # Constructor definition
        self, # Instance pointer
        callback: Callable[[], None], # Action callback triggered on hotkey
        modifiers: int = DEFAULT_HOTKEY_MODIFIERS, # Modifier flags (Ctrl+Alt)
        vk: int = DEFAULT_HOTKEY_VK, # Virtual key code ('M')
        hotkey_id: int = 1 # Hotkey identifier integer
    ): # End parameter list
        self._callback = callback # Store action callback
        self._modifiers = modifiers # Store modifier flags
        self._vk = vk # Store virtual key code
        self._hotkey_id = hotkey_id # Store hotkey identifier
        self._is_running = False # Flag indicating active state
        self._thread: Optional[threading.Thread] = None # Worker thread handle
        self._thread_id: int = 0 # Worker thread ID

    def start_blocking(self) -> None: # Run listener loop synchronously
        self._is_running = True # Set running flag to true
        self._thread_id = threading.get_native_id() # Save thread ID
        logger.info(f"Registering hotkey ID {self._hotkey_id} (Modifiers: {self._modifiers}, VK: {hex(self._vk)})") # Log registration details

        if not user32.RegisterHotKey(None, self._hotkey_id, self._modifiers, self._vk): # Register hotkey with OS
            logger.error("Failed to register Windows system hotkey") # Log failure error
            self._is_running = False # Clear running flag
            return # Exit method

        msg = ctypes.wintypes.MSG() # Allocate Win32 MSG struct
        WM_HOTKEY = 0x0312 # Windows hotkey message identifier
        try: # Begin message loop try block
            while self._is_running: # Loop while running flag is set
                res = user32.GetMessageW(ctypes.byref(msg), None, 0, 0) # Pull next message from queue
                if res <= 0: # Check for WM_QUIT (0) or error (-1)
                    break # Break out of loop
                if msg.message == WM_HOTKEY and msg.wParam == self._hotkey_id: # Match hotkey message
                    logger.info("Hotkey triggered! Executing action callback...") # Log trigger event
                    self._callback() # Invoke registered callback
                user32.TranslateMessage(ctypes.byref(msg)) # Translate keyboard input
                user32.DispatchMessageW(ctypes.byref(msg)) # Dispatch message to window proc
        finally: # Ensure cleanup always executes
            user32.UnregisterHotKey(None, self._hotkey_id) # Unregister hotkey with OS
            self._is_running = False # Reset running flag
            logger.info("Hotkey listener unregistered and terminated") # Log shutdown completion

    def start_async(self) -> None: # Run listener asynchronously in background thread
        if self._is_running: # Check if already running
            return # Exit if active
        self._thread = threading.Thread(target=self.start_blocking, daemon=True) # Create daemon thread
        self._thread.start() # Start background thread

    def stop(self) -> None: # Stop message pump and listener
        self._is_running = False # Set running flag to false
        if self._thread_id != 0: # Check if thread ID is recorded
            WM_QUIT = 0x0012 # Win32 WM_QUIT constant
            user32.PostThreadMessageW(self._thread_id, WM_QUIT, 0, 0) # Post WM_QUIT to terminate GetMessage
        if self._thread and self._thread.is_alive(): # Check if worker thread is alive
            self._thread.join(timeout=2.0) # Join worker thread with timeout
