# ==============================================================================
# File: .//Description-of-file/move_window_api_py.md
# Overview: Python ctypes wrapper exposing the native move_window.dll C ABI as an idiomatic Python API.
# ==============================================================================

import ctypes # Import ctypes library for native C dynamic library binding
import logging # Import logging module for structured diagnostic events
import os # Import operating system module for filesystem path resolutions
from typing import Optional, Tuple # Import typing helpers for optional and tuple types
from config import AlignmentMode, Point2D, Rect2D, MOVE_WINDOW_SUCCESS # Import project configuration

logger = logging.getLogger(__name__) # Initialize module-level logger

def _find_library_path() -> str: # Define library path locator function
    candidates = [ # List potential relative paths to move_window dynamic library
        os.path.join(os.path.dirname(__file__), "..", "build", "move_window.dll"), # CMake build directory standard name
        os.path.join(os.path.dirname(__file__), "..", "build", "libmove_window.dll"), # CMake build directory GCC prefix name
        os.path.join(os.path.dirname(__file__), "move_window.dll"), # Python script directory standard name
        os.path.join(os.path.dirname(__file__), "libmove_window.dll"), # Python script directory GCC prefix name
        os.path.join(os.path.dirname(__file__), "..", "move_window.dll"), # Project root directory
        "move_window.dll" # Current working directory or system PATH
    ] # End candidate list
    for path in candidates: # Iterate over candidate paths
        if os.path.exists(path): # Check if DLL file exists on disk
            return os.path.abspath(path) # Return absolute canonical path
    return "move_window.dll" # Default fallback to library name

class MoveWindowNativeAPI: # Define native API wrapper class
    def __init__(self, dll_path: Optional[str] = None): # Initialize wrapper instance
        target_path = dll_path or _find_library_path() # Resolve path to native DLL
        logger.info(f"Loading native move_window library from: {target_path}") # Log loading location
        self._dll = ctypes.CDLL(target_path) # Load dynamic library via ctypes CDLL
        self._setup_signatures() # Configure C function signatures

    def _setup_signatures(self) -> None: # Define function signatures configuration method
        self._dll.move_window_to_cursor.argtypes = [ctypes.c_void_p, ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.c_int] # Set argument types for move_window_to_cursor
        self._dll.move_window_to_cursor.restype = ctypes.c_int # Set return type for move_window_to_cursor

        self._dll.move_window_by_title.argtypes = [ctypes.c_char_p, ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.c_int] # Set argument types for move_window_by_title
        self._dll.move_window_by_title.restype = ctypes.c_int # Set return type for move_window_by_title

        self._dll.move_window_by_process_name.argtypes = [ctypes.c_char_p, ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.c_int] # Set argument types for move_window_by_process_name
        self._dll.move_window_by_process_name.restype = ctypes.c_int # Set return type for move_window_by_process_name

        self._dll.move_active_window_to_cursor.argtypes = [ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.c_int] # Set argument types for move_active_window_to_cursor
        self._dll.move_active_window_to_cursor.restype = ctypes.c_int # Set return type for move_active_window_to_cursor

        self._dll.find_window_by_title_substring.argtypes = [ctypes.c_char_p] # Set argument types for find_window_by_title_substring
        self._dll.find_window_by_title_substring.restype = ctypes.c_void_p # Set return type for find_window_by_title_substring

        self._dll.find_window_by_process_name.argtypes = [ctypes.c_char_p] # Set argument types for find_window_by_process_name
        self._dll.find_window_by_process_name.restype = ctypes.c_void_p # Set return type for find_window_by_process_name

        self._dll.get_foreground_window_handle.argtypes = [] # Set argument types for get_foreground_window_handle
        self._dll.get_foreground_window_handle.restype = ctypes.c_void_p # Set return type for get_foreground_window_handle

        self._dll.get_cursor_coordinates.argtypes = [ctypes.POINTER(ctypes.c_int), ctypes.POINTER(ctypes.c_int)] # Set argument types for get_cursor_coordinates
        self._dll.get_cursor_coordinates.restype = ctypes.c_int # Set return type for get_cursor_coordinates

        self._dll.get_window_bounds.argtypes = [ctypes.c_void_p, ctypes.POINTER(ctypes.c_int), ctypes.POINTER(ctypes.c_int), ctypes.POINTER(ctypes.c_int), ctypes.POINTER(ctypes.c_int)] # Set argument types for get_window_bounds
        self._dll.get_window_bounds.restype = ctypes.c_int # Set return type for get_window_bounds

    def move_window_to_cursor(self, hwnd: int, mode: AlignmentMode = AlignmentMode.CENTER, offset_x: int = 0, offset_y: int = 0, clamp: bool = True) -> int: # Move window by handle
        return self._dll.move_window_to_cursor(ctypes.c_void_p(hwnd), int(mode), offset_x, offset_y, 1 if clamp else 0) # Invoke DLL function

    def move_by_title(self, title: str, mode: AlignmentMode = AlignmentMode.CENTER, offset_x: int = 0, offset_y: int = 0, clamp: bool = True) -> int: # Move window by title substring
        return self._dll.move_window_by_title(title.encode("utf-8"), int(mode), offset_x, offset_y, 1 if clamp else 0) # Invoke DLL function

    def move_by_process(self, process_name: str, mode: AlignmentMode = AlignmentMode.CENTER, offset_x: int = 0, offset_y: int = 0, clamp: bool = True) -> int: # Move window by process name
        return self._dll.move_window_by_process_name(process_name.encode("utf-8"), int(mode), offset_x, offset_y, 1 if clamp else 0) # Invoke DLL function

    def move_active_window(self, mode: AlignmentMode = AlignmentMode.CENTER, offset_x: int = 0, offset_y: int = 0, clamp: bool = True) -> int: # Move active foreground window
        return self._dll.move_active_window_to_cursor(int(mode), offset_x, offset_y, 1 if clamp else 0) # Invoke DLL function

    def find_by_title(self, title: str) -> Optional[int]: # Find window handle by title substring
        hwnd = self._dll.find_window_by_title_substring(title.encode("utf-8")) # Call DLL lookup
        return hwnd if hwnd else None # Return handle integer or None

    def find_by_process(self, process_name: str) -> Optional[int]: # Find window handle by process name
        hwnd = self._dll.find_window_by_process_name(process_name.encode("utf-8")) # Call DLL lookup
        return hwnd if hwnd else None # Return handle integer or None

    def get_foreground_window(self) -> Optional[int]: # Get active foreground window handle
        hwnd = self._dll.get_foreground_window_handle() # Call DLL getter
        return hwnd if hwnd else None # Return handle integer or None

    def get_cursor_position(self) -> Optional[Point2D]: # Get current mouse cursor position
        cx = ctypes.c_int() # Allocate C integer for X
        cy = ctypes.c_int() # Allocate C integer for Y
        if self._dll.get_cursor_coordinates(ctypes.byref(cx), ctypes.byref(cy)) == MOVE_WINDOW_SUCCESS: # Call native getter
            return Point2D(x=cx.value, y=cy.value) # Return Point2D
        return None # Return None on failure

    def get_window_bounds(self, hwnd: int) -> Optional[Rect2D]: # Get window bounding rectangle
        bx = ctypes.c_int() # Allocate C int for X
        by = ctypes.c_int() # Allocate C int for Y
        bw = ctypes.c_int() # Allocate C int for width
        bh = ctypes.c_int() # Allocate C int for height
        if self._dll.get_window_bounds(ctypes.c_void_p(hwnd), ctypes.byref(bx), ctypes.byref(by), ctypes.byref(bw), ctypes.byref(bh)) == MOVE_WINDOW_SUCCESS: # Call native getter
            return Rect2D(x=bx.value, y=by.value, width=bw.value, height=bh.value) # Return Rect2D
        return None # Return None on failure
