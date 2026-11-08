# ==============================================================================
# File: .//Description-of-file/config_py.md
# Overview: Shared configuration constants, error codes, and alignment enum for the Python move-window API.
# ==============================================================================

from enum import IntEnum # Import IntEnum for typed integer enumerations
from typing import NamedTuple # Import NamedTuple for lightweight data structures

MOVE_WINDOW_SUCCESS = 0 # Return code indicating successful operation
MOVE_WINDOW_ERR_INVALID_ARG = -1 # Return code indicating invalid argument
MOVE_WINDOW_ERR_NOT_FOUND = -2 # Return code indicating target window not found
MOVE_WINDOW_ERR_WIN32_CALL = -3 # Return code indicating Win32 system call error
MOVE_WINDOW_ERR_HOTKEY_FAIL = -4 # Return code indicating hotkey registration failure

DEFAULT_OFFSET_X = 0 # Default horizontal coordinate offset in pixels
DEFAULT_OFFSET_Y = 0 # Default vertical coordinate offset in pixels
DEFAULT_CLAMP_SCREEN = 1 # Default screen boundary clamping flag
DEFAULT_HOTKEY_MODIFIERS = 0x0002 | 0x0001 # Default hotkey modifiers: MOD_CONTROL | MOD_ALT
DEFAULT_HOTKEY_VK = 0x4D # Default virtual key code for 'M' key (0x4D)

class AlignmentMode(IntEnum): # Define AlignmentMode enumeration
    CENTER = 0 # Snap center of window to mouse cursor
    TOP_LEFT = 1 # Snap top-left corner of window to mouse cursor
    CUSTOM_OFFSET = 2 # Apply custom X and Y pixel offset from cursor

class Point2D(NamedTuple): # Define 2D point coordinate tuple
    x: int # Horizontal screen coordinate
    y: int # Vertical screen coordinate

class Rect2D(NamedTuple): # Define 2D bounding rectangle tuple
    x: int # Left boundary coordinate
    y: int # Top boundary coordinate
    width: int # Total width
    height: int # Total height
